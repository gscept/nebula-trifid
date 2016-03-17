//------------------------------------------------------------------------------
// vkrenderdevice.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/config.h"
#include "vkrenderdevice.h"
#include "coregraphics/displaydevice.h"
#include "app/application.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "jobs/tp/tpjobthreadpool.h"
#include "system/cpu.h"

using namespace CoreGraphics;
//------------------------------------------------------------------------------
/**
*/
VKAPI_ATTR VkBool32 VKAPI_CALL
NebulaVulkanDebugCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objectType, uint64_t src, size_t location, int32_t msgCode, const char* layerPrefix, const char* msg, void* userData)
{

#if NEBULAT_VULKAN_DEBUG
	bool ret = true;
#else
	bool ret = false;
#endif

	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		n_error("VULKAN ERROR: [%s], code %d : %s", layerPrefix, msgCode, msg);
	}
	else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		n_error("VULKAN WARNING: [%s], code %d : %s", layerPrefix, msgCode, msg);
	} 
	return ret;
}

namespace Vulkan
{
__ImplementClass(Vulkan::VkRenderDevice, 'VURD', Base::RenderDeviceBase);
__ImplementSingleton(Vulkan::VkRenderDevice);

VkDevice VkRenderDevice::dev;
VkDescriptorPool VkRenderDevice::descPool;
VkQueue VkRenderDevice::displayQueue;
VkQueue VkRenderDevice::computeQueue;
VkQueue VkRenderDevice::transferQueue;
VkInstance VkRenderDevice::instance;
VkPhysicalDevice VkRenderDevice::physicalDev;
VkPipelineCache VkRenderDevice::cache;

VkCommandPool VkRenderDevice::cmdCmpPool[2];
VkCommandPool VkRenderDevice::cmdTransPool[2];
VkCommandPool VkRenderDevice::cmdGfxPool[2];
VkCommandBuffer VkRenderDevice::mainCmdGfxBuffer;
VkCommandBuffer VkRenderDevice::mainCmdCmpBuffer;
VkCommandBuffer VkRenderDevice::mainCmdTransBuffer;

//------------------------------------------------------------------------------
/**
*/
VkRenderDevice::VkRenderDevice() :
	renderQueueIdx(-1),
	computeQueueIdx(-1),
	transferQueueIdx(-1),
	currentDrawThread(0),
	currentTransThread(0)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
VkRenderDevice::~VkRenderDevice()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
VkRenderDevice::Open()
{
	n_assert(!this->IsOpen());
	bool success = false;
	if (this->OpenVulkanContext())
	{
		// hand to parent class, this will notify event handlers
		success = RenderDeviceBase::Open();
	}
	return success;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::Close()
{
	n_assert(this->IsOpen());
	this->CloseVulkanDevice();
	RenderDeviceBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
bool
VkRenderDevice::OpenVulkanContext()
{
	DisplayDevice* displayDevice = DisplayDevice::Instance();
	n_assert(displayDevice->IsOpen());

	// create result
	VkResult res;

	// setup application
	VkApplicationInfo appInfo =
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		NULL,
		App::Application::Instance()->GetAppTitle().AsCharPtr(),
		1,					// application version
		"Nebula Trifid",	// engine name
		1,					// engine version
		VK_API_VERSION		// API version
	};

	this->usedExtensions = 0;
	int32_t requiredExtensionsNum;
	const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionsNum);
	uint32_t i;
	for (i = 0; i < (uint32_t)requiredExtensionsNum; i++)
	{
		this->extensions[this->usedExtensions++] = requiredExtensions[i];
	}
	
	const char* layers[] = { "VK_LAYER_LUNARG_mem_tracker", "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_device_limits", "VK_LAYER_LUNARG_threading", "VK_LAYER_LUNARG_param_checker", "VK_LAYER_LUNARG_draw_state" };
#if NEBULAT_VULKAN_DEBUG
	this->extensions[this->usedExtensions++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
	const int numLayers = 5;
#else
	const int numLayers = 0;
#endif

	// setup instance
	VkInstanceCreateInfo instanceInfo =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,		// type of struct
		NULL,										// pointer to next
		0,											// flags
		&appInfo,									// application
		numLayers,
		layers,
		this->usedExtensions,
		this->extensions
	};
	
	// create instance
	res = vkCreateInstance(&instanceInfo, NULL, &this->instance);
	if (res == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		n_error("VkRenderDevice::OpenVulkanContext(): Your GPU driver is not compatible with Vulkan.\n");
	}
	else if (res == VK_ERROR_EXTENSION_NOT_PRESENT)
	{
		n_error("VkRenderDevice::OpenVulkanContext(): Vulkan extension failed to load.\n");
	}
	else if (res == VK_ERROR_LAYER_NOT_PRESENT)
	{
		n_error("VkRenderDevice::OpenVulkanContext(): Vulkan layer failed to load.\n");
	}
	n_assert(res == VK_SUCCESS);

	// setup adapter
	this->SetupAdapter();

#if NEBULAT_VULKAN_DEBUG
	this->debugCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugReportCallbackEXT");
	n_assert(this->debugCallback != NULL);

	VkDebugReportCallbackCreateInfoEXT dbgInfo;
	dbgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	dbgInfo.pNext = NULL;
	dbgInfo.pfnCallback = NebulaVulkanDebugCallback;
	dbgInfo.pUserData = NULL;
	res = this->debugCallback(this->instance, &dbgInfo, NULL, &this->debugCallbackHandle);
	n_assert(res == VK_SUCCESS);
#endif

	// get number of queues
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDev, &this->numQueues, NULL);
	n_assert(this->numQueues > 0);

	// now get queues from device
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDev, &this->numQueues, this->queuesProps);
	vkGetPhysicalDeviceMemoryProperties(this->physicalDev, &this->memoryProps);

	// setup swap chain in display
	VkDisplayDevice::Instance()->SetupSwapchain();

	this->surfaceSupport = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(this->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	this->surfaceFormats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(this->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	this->surfaceCaps = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(this->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	this->presentModes = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(this->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	n_assert(surfaceSupport != NULL);
	n_assert(surfaceFormats != NULL);
	VkBool32* canPresent = n_new_array(VkBool32, numQueues);
	for (i = 0; i < numQueues; i++)
	{
		this->surfaceSupport(VkRenderDevice::Instance()->physicalDev, i, VkDisplayDevice::Instance()->surface, &canPresent[i]);
	}

	uint32_t gfxIdx = UINT32_MAX;
	uint32_t computeIdx = UINT32_MAX;
	uint32_t transferIdx = UINT32_MAX;
	uint32_t queueIdx = UINT32_MAX;
	this->renderQueueIdx = UINT32_MAX;
	this->computeQueueIdx = UINT32_MAX;
	this->transferQueueIdx = UINT32_MAX;

	// create three queues for each family
	Util::FixedArray<uint> indexMap;
	indexMap.Resize(numQueues);
	indexMap.Fill(0);
	for (i = 0; i < numQueues; i++)
	{
		if (this->queuesProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			uint32_t j;
			for (j = 0; j < numQueues; j++)
			{
				if (canPresent[i] == VK_TRUE)
				{
					this->renderQueueIdx = j;
					gfxIdx = indexMap[i];
					indexMap[i]++;
					break;
				}
			}
		}

		// also setup compute and transfer queues
		if (this->queuesProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT && this->computeQueueIdx == UINT32_MAX)
		{
			if (this->queuesProps[i].queueCount == indexMap[i]) continue;
			computeIdx = i;
			this->computeQueueIdx = indexMap[i];
			indexMap[i]++;
		}
		if (this->queuesProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT && this->transferQueueIdx == UINT32_MAX)
		{
			if (this->queuesProps[i].queueCount == indexMap[i]) continue;
			transferIdx = i;
			this->transferQueueIdx = indexMap[i];
			indexMap[i]++;
		}
	}

	if (this->renderQueueIdx == UINT32_MAX || gfxIdx == UINT32_MAX) n_error("VkDisplayDevice: Could not find a queue that supported screen present and graphics.\n");

	// delete array of present flags
	n_delete_array(canPresent);

	// create device
	Util::FixedArray<Util::FixedArray<float>> prios;
	Util::Array<VkDeviceQueueCreateInfo> queueInfos;
	prios.Resize(numQueues);

	for (i = 0; i < numQueues; i++)
	{
		if (indexMap[i] == 0) continue;
		prios[i].Resize(indexMap[i]);
		prios[i].Fill(0.0f);
		queueInfos.Append(
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			NULL,
			0,
			i,
			indexMap[i],
			&prios[i][0]
		});
	}

	// get physical device features
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(this->physicalDev, &features);

	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(this->physicalDev, &props);

	VkDeviceCreateInfo deviceInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		NULL,
		0,
		queueInfos.Size(),
		&queueInfos[0],
		numLayers,
		layers,
		this->usedPhysicalExtensions,
		this->deviceExtensionStrings,
		&features
	};

	// create device
	res = vkCreateDevice(this->physicalDev, &deviceInfo, NULL, &this->dev);
	n_assert(res == VK_SUCCESS);

	// grab implementation functions for swap chain management.
	this->swapChainCtor = (PFN_vkCreateSwapchainKHR)vkGetInstanceProcAddr(this->instance, "vkCreateSwapchainKHR");
	this->swapChainDtor = (PFN_vkDestroySwapchainKHR)vkGetInstanceProcAddr(this->instance, "vkDestroySwapchainKHR");
	this->swapChainNextImage = (PFN_vkAcquireNextImageKHR)vkGetInstanceProcAddr(this->instance, "vkAcquireNextImageKHR");
	this->swapChainPresent = (PFN_vkQueuePresentKHR)vkGetInstanceProcAddr(this->instance, "vkQueuePresentKHR");
	this->swapChainGetImages = (PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(this->instance, "vkGetSwapchainImagesKHR");
	n_assert(this->swapChainCtor != NULL);
	n_assert(this->swapChainDtor != NULL);
	n_assert(this->swapChainNextImage != NULL);
	n_assert(this->swapChainPresent != NULL);

	// setup display queue in render device (gfxIdx is different, because it's family doesn't have to be the displayable one)
	vkGetDeviceQueue(VkRenderDevice::dev, gfxIdx, this->renderQueueIdx, &VkRenderDevice::displayQueue);
	vkGetDeviceQueue(VkRenderDevice::dev, computeIdx, this->computeQueueIdx, &VkRenderDevice::computeQueue);
	vkGetDeviceQueue(VkRenderDevice::dev, transferIdx, this->transferQueueIdx, &VkRenderDevice::transferQueue);

	// find available surface formats
	uint32_t numFormats;
	res = this->surfaceFormats(VkRenderDevice::physicalDev, VkDisplayDevice::Instance()->surface, &numFormats, NULL);
	n_assert(res == VK_SUCCESS);

	VkSurfaceFormatKHR* formats = n_new_array(VkSurfaceFormatKHR, numFormats);
	res = this->surfaceFormats(VkRenderDevice::physicalDev, VkDisplayDevice::Instance()->surface, &numFormats, formats);
	n_assert(res == VK_SUCCESS);
	if (numFormats == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		// is this really the goto format?
		// perhaps assuming sRGB is a bit risky when we can't even get a format to begin with
		this->format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		this->format = formats[0].format;
	}
	this->colorSpace = formats[0].colorSpace;

	// get surface capabilities
	VkSurfaceCapabilitiesKHR surfCaps;
	res = this->surfaceCaps(this->physicalDev, VkDisplayDevice::Instance()->surface, &surfCaps);
	n_assert(res == VK_SUCCESS);

	uint32_t numPresentModes;
	res = this->presentModes(this->physicalDev, VkDisplayDevice::Instance()->surface, &numPresentModes, NULL);
	n_assert(res == VK_SUCCESS);

	// get present modes
	VkPresentModeKHR* presentModes = n_new_array(VkPresentModeKHR, numPresentModes);
	res = this->presentModes(this->physicalDev, VkDisplayDevice::Instance()->surface, &numPresentModes, presentModes);
	n_assert(res == VK_SUCCESS);

	VkExtent2D swapchainExtent;
	if (surfCaps.currentExtent.width == -1)
	{
		const DisplayMode& mode = DisplayDevice::Instance()->GetDisplayMode();
		swapchainExtent.width = mode.GetWidth();
		swapchainExtent.height = mode.GetHeight();
	}
	else
	{
		swapchainExtent = surfCaps.currentExtent;
	}

	// figure out the best present mode, mailo
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (i = 0; i < numPresentModes; i++)
	{
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
		{
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	// get the optimal set of swap chain images, the more the better
	uint32_t numSwapchainImages = surfCaps.minImageCount + 1;
	if ((surfCaps.maxImageCount > 0) && (numSwapchainImages > surfCaps.maxImageCount)) numSwapchainImages = surfCaps.maxImageCount;

	// create a transform
	VkSurfaceTransformFlagBitsKHR transform;
	if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else																	  transform = surfCaps.currentTransform;

	VkSwapchainCreateInfoKHR swapchainInfo =
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		NULL,
		0,
		VkDisplayDevice::Instance()->surface,
		numSwapchainImages,
		this->format,
		this->colorSpace,
		swapchainExtent,
		1,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		this->renderQueueIdx,
		NULL,
		transform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		swapchainPresentMode,
		true,
		NULL
	};

	// create swapchain
	res = this->swapChainCtor(this->dev, &swapchainInfo, NULL, &this->swapchain);
	n_assert(res == VK_SUCCESS);

	// get back buffers
	uint32_t numSwapchainBackbuffers;
	res = this->swapChainGetImages(this->dev, this->swapchain, &numSwapchainBackbuffers, NULL);
	n_assert(res == VK_SUCCESS);

	this->backbuffers = n_new_array(VkImage, numSwapchainBackbuffers);
	res = this->swapChainGetImages(this->dev, this->swapchain, &numSwapchainBackbuffers, this->backbuffers);

	VkPipelineCacheCreateInfo cacheInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		NULL,
		0,
		0,
		NULL
	};

	// create cache
	res = vkCreatePipelineCache(this->dev, &cacheInfo, NULL, &this->cache);
	n_assert(res == VK_SUCCESS);

	VkDescriptorPoolSize sizes[11];
	VkDescriptorType types[] =
	{
		VK_DESCRIPTOR_TYPE_SAMPLER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
	};

	for (i = 0; i < 11; i++)
	{
		sizes[i].descriptorCount = VkPoolSetSize;
		sizes[i].type = types[i];
	}
	
	VkDescriptorPoolCreateInfo poolInfo =
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		NULL,
		VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		VkPoolMaxSets,
		11,
		sizes
	};

	res = vkCreateDescriptorPool(this->dev, &poolInfo, NULL, &this->descPool);
	n_assert(res == VK_SUCCESS);

	// create command pool for graphics
	VkCommandPoolCreateInfo cmdPoolInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		NULL,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		this->renderQueueIdx
	};

	for (i = 0; i < NumCmdCreationUsages; i++)
	{
		cmdPoolInfo.flags = i == Transient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cmdPoolInfo.queueFamilyIndex = this->renderQueueIdx;
		res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->cmdGfxPool[i]);
		n_assert(res == VK_SUCCESS);

		// create command pool for computes
		cmdPoolInfo.queueFamilyIndex = this->computeQueueIdx;

		res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->cmdCmpPool[i]);
		n_assert(res == VK_SUCCESS);

		// create command pool for transfers
		cmdPoolInfo.queueFamilyIndex = this->transferQueueIdx;

		res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->cmdTransPool[i]);
		n_assert(res == VK_SUCCESS);
	}
	
	// create main command buffer for graphics
	VkCommandBufferAllocateInfo cmdAllocInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->cmdGfxPool[Persistent],
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};
	res = vkAllocateCommandBuffers(VkRenderDevice::dev, &cmdAllocInfo, &this->mainCmdGfxBuffer);
	n_assert(res == VK_SUCCESS);

	// create main command buffer for computes
	cmdAllocInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->cmdCmpPool[Persistent],
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};
	res = vkAllocateCommandBuffers(VkRenderDevice::dev, &cmdAllocInfo, &this->mainCmdCmpBuffer);
	n_assert(res == VK_SUCCESS);

	// create main command buffer for transfers
	cmdAllocInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->cmdTransPool[Persistent],
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};
	res = vkAllocateCommandBuffers(VkRenderDevice::dev, &cmdAllocInfo, &this->mainCmdTransBuffer);
	n_assert(res == VK_SUCCESS);

	// setup threads
	Util::String threadName;
	for (i = 0; i < NumDrawThreads; i++)
	{
		threadName.Format("DrawCmdBufferThread%d", i);
		this->drawThreads[i] = VkCmdBufferThread::Create();
		this->drawThreads[i]->SetPriority(Threading::Thread::High);
		this->drawThreads[i]->SetCoreId(System::Cpu::RenderThreadFirstCore + i);
		this->drawThreads[i]->SetName(threadName);
		this->drawThreads[i]->Start();
	}

	for (i = 0; i < NumTransferThreads; i++)
	{
		threadName.Format("TransferCmdBufferThread%d", i);
		this->transThreads[i] = VkCmdBufferThread::Create();
		this->transThreads[i]->SetPriority(Threading::Thread::Normal);
		this->transThreads[i]->SetCoreId(System::Cpu::RenderThreadFirstCore + NumDrawThreads + i);
		this->transThreads[i]->SetName(threadName);
		this->transThreads[i]->Start();
	}

	// yay, Vulkan!
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::CloseVulkanDevice()
{
	this->swapChainDtor(this->dev, this->swapchain, NULL);
	delete[] this->backbuffers;

	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		this->drawThreads[i]->Stop();
		this->drawThreads[i] = 0;
	}

	for (i = 0; i < NumTransferThreads; i++)
	{
		this->transThreads[i]->Stop();
		this->transThreads[i] = 0;
	}

	// wait for all commands to be done first
	VkResult res = vkQueueWaitIdle(this->displayQueue);
	n_assert(res == VK_SUCCESS);

	// free our main buffers, our secondary buffers should be fine so the pools should be free to destroy
	vkFreeCommandBuffers(this->dev, this->cmdGfxPool[0], 1, &this->mainCmdGfxBuffer);
	vkFreeCommandBuffers(this->dev, this->cmdCmpPool[0], 1, &this->mainCmdCmpBuffer);
	vkFreeCommandBuffers(this->dev, this->cmdTransPool[0], 1, &this->mainCmdTransBuffer);
	vkDestroyCommandPool(this->dev, this->cmdGfxPool[0], NULL);
	vkDestroyCommandPool(this->dev, this->cmdGfxPool[1], NULL);
	vkDestroyCommandPool(this->dev, this->cmdCmpPool[0], NULL);
	vkDestroyCommandPool(this->dev, this->cmdCmpPool[1], NULL);
	vkDestroyCommandPool(this->dev, this->cmdTransPool[0], NULL);
	vkDestroyCommandPool(this->dev, this->cmdTransPool[1], NULL);

	vkDestroyDevice(this->dev, NULL);
	vkDestroyInstance(this->instance, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetupQueue(uint32_t queueFamily, uint32_t queueIndex)
{
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetupAdapter()
{
	// retrieve available GPUs
	uint32_t gpuCount;
	VkResult res;
	res = vkEnumeratePhysicalDevices(this->instance, &gpuCount, NULL);
	n_assert(res == VK_SUCCESS);

	if (gpuCount > 0)
	{
		res = vkEnumeratePhysicalDevices(this->instance, &gpuCount, this->devices);
		n_assert(res == VK_SUCCESS);

		// hmm, this is ugly, perhaps implement a way to get a proper device
		this->physicalDev = devices[0];
	}
	else
	{
		n_error("VkRenderDevice::SetupAdapter(): No GPU available.\n");
	}

	res = vkEnumerateDeviceExtensionProperties(this->physicalDev, NULL, &this->usedPhysicalExtensions, NULL);
	n_assert(res == VK_SUCCESS);

	if (this->usedPhysicalExtensions > 0)
	{
		res = vkEnumerateDeviceExtensionProperties(this->physicalDev, NULL, &this->usedPhysicalExtensions, this->physicalExtensions);

		uint32_t i;
		for (i = 0; i < this->usedPhysicalExtensions; i++)
		{
			this->deviceExtensionStrings[i] = this->physicalExtensions[i].extensionName;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetupBufferFormats()
{
	
}

//------------------------------------------------------------------------------
/**
*/
bool
VkRenderDevice::BeginFrame(IndexT frameIndex)
{
	const VkCommandBufferBeginInfo cmdInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		VK_NULL_HANDLE
	};
	vkBeginCommandBuffer(this->mainCmdGfxBuffer, &cmdInfo);
	vkBeginCommandBuffer(this->mainCmdCmpBuffer, &cmdInfo);
	vkBeginCommandBuffer(this->mainCmdTransBuffer, &cmdInfo);

	// reset current thread
	this->currentDrawThread = 0;

	const VkSemaphoreCreateInfo semInfo =
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		NULL,
		0
	};
	vkCreateSemaphore(this->dev, &semInfo, NULL, &this->displaySemaphore);

	VkResult res = this->swapChainNextImage(this->dev, this->swapchain, UINT64_MAX, this->displaySemaphore, VK_NULL_HANDLE, &this->currentBackbuffer);
	if (res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// this means our swapchain needs a resize!
	}
	else
	{
		n_assert(res == VK_SUCCESS);
	}
	return RenderDeviceBase::BeginFrame(frameIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetStreamVertexBuffer(IndexT streamIndex, const Ptr<CoreGraphics::VertexBuffer>& vb, IndexT offsetVertexIndex)
{
	RenderDeviceBase::SetStreamVertexBuffer(streamIndex, vb, offsetVertexIndex);
	VkCmdBufferThread::Command cmd;
	cmd.vbo.buffer = vb->GetVkBuffer();
	cmd.vbo.index = streamIndex;
	cmd.vbo.offset = offsetVertexIndex;
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetVertexLayout(const Ptr<CoreGraphics::VertexLayout>& vl)
{
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& ib)
{
	VkCmdBufferThread::Command cmd;
	cmd.ibo.buffer = ib->GetVkBuffer();
	cmd.ibo.indexType = ib->GetIndexType() == IndexType::Index16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
	cmd.ibo.offset = 0;
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::Compute(int dimX, int dimY, int dimZ, uint flag /*= NoBarrier*/)
{
	RenderDeviceBase::Compute(dimX, dimY, dimZ);
	vkCmdDispatch(this->mainCmdCmpBuffer, dimX, dimY, dimZ);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::RenderTarget>& rt, const Ptr<CoreGraphics::Shader>& passShader)
{
	RenderDeviceBase::BeginPass(rt, passShader);
	this->SetFramebufferLayoutInfo(rt->GetVkPipelineInfo());
	const Util::FixedArray<VkViewport>& viewports = rt->GetVkViewports();
	vkCmdSetViewport(this->mainCmdGfxBuffer, 0, viewports.Size(), &viewports[0]);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::MultipleRenderTarget>& mrt, const Ptr<CoreGraphics::Shader>& passShader)
{
	RenderDeviceBase::BeginPass(mrt, passShader);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::RenderTargetCube>& rtc, const Ptr<CoreGraphics::Shader>& passShader)
{
	RenderDeviceBase::BeginPass(rtc, passShader);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb, CoreGraphics::PrimitiveTopology::Code primType, const Ptr<CoreGraphics::Shader>& passShader)
{
	RenderDeviceBase::BeginFeedback(fb, primType, passShader);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginBatch(CoreGraphics::FrameBatchType::Code batchType)
{
	RenderDeviceBase::BeginBatch(batchType);
	
	// allocate a command buffer per thread
	const VkCommandBufferAllocateInfo info =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->cmdGfxPool[Transient],
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		NumDrawThreads
	};
	VkResult res = vkAllocateCommandBuffers(this->dev, &info, this->dispatchableDrawCmdBuffers);
	n_assert(res == VK_SUCCESS);

	// begin buffers
	const VkCommandBufferBeginInfo beginInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		VK_NULL_HANDLE
	};

	for (IndexT i = 0; i < NumDrawThreads; i++)
	{
		vkBeginCommandBuffer(this->dispatchableDrawCmdBuffers[i], &beginInfo);
		this->drawThreads[i]->SetCommandBuffer(this->dispatchableDrawCmdBuffers[i]);
	}

}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::Draw()
{
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::Draw;
	cmd.draw.baseIndex = this->primitiveGroup.GetBaseIndex();
	cmd.draw.baseVertex = this->primitiveGroup.GetBaseVertex();
	cmd.draw.baseInstance = 0;
	cmd.draw.numIndices = this->primitiveGroup.GetNumIndices();
	cmd.draw.numVerts = this->primitiveGroup.GetNumVertices();
	cmd.draw.numInstances = 1;
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::DrawIndexedInstanced(SizeT numInstances, IndexT baseInstance)
{
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::Draw;
	cmd.draw.baseIndex = this->primitiveGroup.GetBaseIndex();
	cmd.draw.baseVertex = this->primitiveGroup.GetBaseVertex();
	cmd.draw.baseInstance = baseInstance;
	cmd.draw.numIndices = this->primitiveGroup.GetNumIndices();
	cmd.draw.numVerts = this->primitiveGroup.GetNumVertices();
	cmd.draw.numInstances = numInstances;
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::DrawFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb)
{
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::DrawFeedbackInstanced(const Ptr<CoreGraphics::FeedbackBuffer>& fb, SizeT numInstances)
{
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndBatch()
{
	// wait for our jobs to finish
	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{ 
		VkCmdBufferThread::Command cmd;
		cmd.type = VkCmdBufferThread::Sync;
		cmd.syncEvent = &this->drawCompletionEvent[i];
		this->drawThreads[i]->PushCommand(cmd);
		this->drawCompletionEvent[i].Wait();
		this->drawCompletionEvent[i].Reset();
	}
	
	// stop recording command buffers
	for (IndexT i = 0; i < NumDrawThreads; i++)
	{
		vkEndCommandBuffer(this->dispatchableDrawCmdBuffers[i]);
	}

	const VkPipelineStageFlags flags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	const VkSubmitInfo info =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		NULL,
		0,
		NULL,
		&flags,
		NumDrawThreads,
		this->dispatchableDrawCmdBuffers,
		0,
		NULL
	};

	// submit to queue
	VkResult res = vkQueueSubmit(this->displayQueue, 1, &info, VK_NULL_HANDLE);
	n_assert(res == VK_SUCCESS);

	// free up our command buffers
	vkFreeCommandBuffers(this->dev, this->cmdGfxPool[Transient], NumDrawThreads, this->dispatchableDrawCmdBuffers);
	RenderDeviceBase::EndBatch();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndPass()
{
	this->currentPipelineBits = 0;
	Memory::Clear(&this->currentPipelineInfo, sizeof(VkGraphicsPipelineCreateInfo));
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndFeedback()
{
	RenderDeviceBase::EndFeedback();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndFrame(IndexT frameIndex)
{
	RenderDeviceBase::EndFrame(frameIndex);
	vkEndCommandBuffer(this->mainCmdGfxBuffer);
	vkEndCommandBuffer(this->mainCmdCmpBuffer);
	vkEndCommandBuffer(this->mainCmdTransBuffer);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::Present()
{
	// submit a sync point
	VkPipelineStageFlags flags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	const VkSubmitInfo submitInfo = 
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		NULL,
		1,
		&this->displaySemaphore,
		&flags, 
		0,
		NULL,
		0,
		NULL
	};
	VkResult res = vkQueueSubmit(this->displayQueue, 1, &submitInfo, NULL);
	n_assert(res == VK_SUCCESS);

	// destroy semaphore
	vkDestroySemaphore(this->dev, this->displaySemaphore, NULL);

	// present frame
	const VkPresentInfoKHR info =
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		NULL,
		0,
		NULL,
		1,
		&this->swapchain,
		&this->currentBackbuffer
	};
	res = this->swapChainPresent(this->displayQueue, &info);

	if (res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// window has been resized!
	}
	else
	{
		n_assert(res == VK_SUCCESS);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::AddRenderTarget(const GLuint& renderTarget) const
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetScissorRect(const Math::rectangle<int>& rect, int index)
{
	VkRect2D sc;
	sc.extent.height = rect.height();
	sc.extent.width = rect.width();
	sc.offset.x = rect.left;
	sc.offset.y = rect.top;
	vkCmdSetScissor(this->mainCmdGfxBuffer, index, 1, &sc);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetViewport(const Math::rectangle<int>& rect, int index)
{
	VkViewport vp;
	vp.width = (float)rect.width();
	vp.height = (float)rect.height();
	vp.x = (float)rect.left;
	vp.y = (float)rect.top;
	vkCmdSetViewport(this->mainCmdGfxBuffer, index, 1, &vp);
}

//------------------------------------------------------------------------------
/**
*/
CoreGraphics::ImageFileFormat::Code
VkRenderDevice::SaveScreenshot(CoreGraphics::ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream)
{
	return CoreGraphics::ImageFileFormat::InvalidImageFileFormat;
}

//------------------------------------------------------------------------------
/**
*/
CoreGraphics::ImageFileFormat::Code
VkRenderDevice::SaveScreenshot(CoreGraphics::ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream, const Math::rectangle<int>& rect, int x, int y)
{
	return CoreGraphics::ImageFileFormat::InvalidImageFileFormat;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::DisplayResized(SizeT width, SizeT height)
{

}

//------------------------------------------------------------------------------
/**
*/
VkResult
VkRenderDevice::GetMemoryType(uint32_t bits, VkMemoryPropertyFlags flags, uint32_t& index)
{
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++)
	{
		if ((bits & 1) == 1)
		{
			if ((this->memoryProps.memoryTypes[i].propertyFlags & flags) == flags)
			{
				index = i;
				return VK_SUCCESS;
			}
		}
		bits >>= 1;
	}
	return VK_ERROR_FEATURE_NOT_PRESENT;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::AllocateBufferMemory(const VkBuffer& buf, VkDeviceMemory& bufmem, VkMemoryPropertyFlagBits flags, uint32_t& bufsize)
{
	// now attain memory requirements so we get a properly aligned memory storage
	VkMemoryRequirements req;
	vkGetBufferMemoryRequirements(VkRenderDevice::dev, buf, &req);

	uint32_t memtype;
	VkResult err = this->GetMemoryType(req.memoryTypeBits, flags, memtype);
	n_assert(err == VK_SUCCESS);
	VkMemoryAllocateInfo meminfo =
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		NULL,
		req.size,
		memtype
	};

	// now allocate memory
	err = vkAllocateMemory(VkRenderDevice::dev, &meminfo, NULL, &bufmem);
	if (err == VK_ERROR_OUT_OF_DEVICE_MEMORY || err == VK_ERROR_OUT_OF_HOST_MEMORY)
	{
		n_error("VkRenderDevice::AllocateBufferMemory(): Could not allocate '%d' bytes, out of memory\n.", req.size);
	}
	n_assert(err == VK_SUCCESS);
	bufsize = (uint32_t)req.size;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::AllocateImageMemory(const VkImage& img, VkDeviceMemory& imgmem, VkMemoryPropertyFlagBits flags, uint32_t& imgsize)
{
	// now attain memory requirements so we get a properly aligned memory storage
	VkMemoryRequirements req;
	vkGetImageMemoryRequirements(VkRenderDevice::dev, img, &req);

	uint32_t memtype;
	VkResult err = this->GetMemoryType(req.memoryTypeBits, flags, memtype);
	n_assert(err == VK_SUCCESS);
	VkMemoryAllocateInfo meminfo =
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		NULL,
		req.size,
		memtype
	};

	// now allocate memory
	err = vkAllocateMemory(VkRenderDevice::dev, &meminfo, NULL, &imgmem);
	if (err == VK_ERROR_OUT_OF_DEVICE_MEMORY || err == VK_ERROR_OUT_OF_HOST_MEMORY)
	{
		n_error("VkRenderDevice::AllocateImageMemory(): Could not allocate '%d' bytes, out of memory\n.", req.size);
	}
	n_assert(err == VK_SUCCESS);
	imgsize = (uint32_t)req.size;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetShaderPipelineInfo(const VkGraphicsPipelineCreateInfo& shader)
{
	this->currentPipelineBits |= ShaderInfoSet;

	this->currentPipelineInfo.pColorBlendState = shader.pColorBlendState;
	this->currentPipelineInfo.pDepthStencilState = shader.pDepthStencilState;
	this->currentPipelineInfo.pRasterizationState = shader.pRasterizationState;
	this->currentPipelineInfo.pMultisampleState = shader.pMultisampleState;
	this->currentPipelineInfo.stageCount = shader.stageCount;
	this->currentPipelineInfo.pStages = shader.pStages;
	this->currentPipelineInfo.layout = shader.layout;
	
	if (this->currentPipelineBits & AllInfoSet)
	{
		this->CreatePipeline();
	}
}

//------------------------------------------------------------------------------
/**
	This is most likely to cause pipeline creations, because the stack is most usually
	Framebuffer
		Shader
			Vertex layout
*/
void
VkRenderDevice::SetVertexLayoutPipelineInfo(const VkGraphicsPipelineCreateInfo& vertexLayout)
{
	this->currentPipelineBits |= VertexLayoutInfoSet;
	this->currentPipelineInfo.pVertexInputState = vertexLayout.pVertexInputState;
	this->currentPipelineInfo.pInputAssemblyState = vertexLayout.pInputAssemblyState;

	// if all bits are set, create a graphics pipeline and set it
	if (this->currentPipelineBits & AllInfoSet)
	{
		this->CreatePipeline();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetFramebufferLayoutInfo(const VkGraphicsPipelineCreateInfo& framebufferLayout)
{
	this->currentPipelineBits |= FramebufferLayoutInfoSet;
	this->currentPipelineInfo.renderPass = framebufferLayout.renderPass;
	this->currentPipelineInfo.subpass = framebufferLayout.subpass;

	if (this->currentPipelineBits & AllInfoSet)
	{
		this->CreatePipeline();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::CreatePipeline()
{
	VkPipeline pipeline;
	VkResult res = vkCreateGraphicsPipelines(this->dev, this->cache, 1, &this->currentPipelineInfo, NULL, &pipeline);
	n_assert(res);

	// send pipeline bind command, this is the first step in our procedure, so we use this as a trigger to switch threads
	this->currentDrawThread = (this->currentDrawThread + 1) % NumDrawThreads;
	VkCmdBufferThread::Command cmd;
	cmd.pipeline = pipeline;
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::UpdateDescriptors(const Util::FixedArray<VkDescriptorSet>& descriptors, const VkPipelineLayout& layout, uint32_t baseSet, uint32_t setCount, uint32_t* offsets, uint32_t offsetCount)
{
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::BindDescriptors;
	cmd.descriptor.layout = layout;
	cmd.descriptor.baseSet = baseSet;
	cmd.descriptor.numSets = setCount;
	cmd.descriptor.sets = &descriptors[0];
	cmd.descriptor.numOffsets = offsetCount;
	cmd.descriptor.offsets = offsets;
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::UpdatePushRanges(const VkShaderStageFlags& stages, const VkPipelineLayout& layout, uint32_t offset, uint32_t size, void* data)
{
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::PushRange;
	cmd.pushranges.layout = layout;
	cmd.pushranges.offset = offset;
	cmd.pushranges.size = size;
	cmd.pushranges.stages = stages;
	cmd.pushranges.data = data;
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
	Heh, we have to truncate VkDeviceSize to int if we run in 32 bit...
*/
void
VkRenderDevice::PushBufferUpdate(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, uint32_t* data, bool deleteWhenDone)
{
	// if we are going to handle the deletes ourself, then we must copy the data
	uint32_t* dataCopy = data;
	if (deleteWhenDone)
	{
		dataCopy = n_new_array(uint32_t, (SizeT)size);
		Memory::Copy(data, dataCopy, (SizeT)size);
	}

	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::UpdateBuffer;
	cmd.updBuffer.deleteWhenDone = deleteWhenDone;
	cmd.updBuffer.buf = buf;
	cmd.updBuffer.data = dataCopy;
	cmd.updBuffer.size = size;
	cmd.updBuffer.offset = offset;
	this->transThreads[this->currentTransThread]->PushCommand(cmd);
}

} // namespace Vulkan