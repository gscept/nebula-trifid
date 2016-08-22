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
#include "vktypes.h"
#include "vktransformdevice.h"
#include "vkshaderserver.h"

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
		n_error("VULKAN ERROR: [%s], code %d : %s\n", layerPrefix, msgCode, msg);
	}
	else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		n_warning("VULKAN WARNING: [%s], code %d : %s\n", layerPrefix, msgCode, msg);
	} 
	return ret;
}

namespace Vulkan
{
__ImplementClass(Vulkan::VkRenderDevice, 'VURD', Base::RenderDeviceBase);
__ImplementSingleton(Vulkan::VkRenderDevice);

VkDevice VkRenderDevice::dev;
VkDescriptorPool VkRenderDevice::descPool;
VkQueue VkRenderDevice::drawQueue;
VkQueue VkRenderDevice::computeQueue;
VkQueue VkRenderDevice::transferQueue;
VkInstance VkRenderDevice::instance;
VkPhysicalDevice VkRenderDevice::physicalDev;
VkPipelineCache VkRenderDevice::cache;

VkCommandPool VkRenderDevice::mainCmdCmpPool;
VkCommandPool VkRenderDevice::mainCmdTransPool;
VkCommandPool VkRenderDevice::mainCmdDrawPool;
VkCommandBuffer VkRenderDevice::mainCmdDrawBuffer;
VkCommandBuffer VkRenderDevice::mainCmdCmpBuffer;
VkCommandBuffer VkRenderDevice::mainCmdTransBuffer;

//------------------------------------------------------------------------------
/**
*/
VkRenderDevice::VkRenderDevice() :
	frameId(0),
	drawQueueIdx(-1),
	computeQueueIdx(-1),
	transferQueueIdx(-1),
	currentDrawThread(NumDrawThreads-1),
	numActiveThreads(0),
	currentTransThread(0),
	currentProgram(0),
	commands(NumCommandPasses)
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
		//VK_MAKE_VERSION(1, 0, 4)
		VK_MAKE_VERSION(1, 0, 8)		// API version
	};

	this->usedExtensions = 0;
	uint32_t requiredExtensionsNum;
	const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionsNum);
	uint32_t i;
	for (i = 0; i < (uint32_t)requiredExtensionsNum; i++)
	{
		this->extensions[this->usedExtensions++] = requiredExtensions[i];
	}
	
	const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
#if NEBULAT_VULKAN_DEBUG
	this->extensions[this->usedExtensions++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
	const int numLayers = 0;// sizeof(layers) / sizeof(const char*);
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
	this->debugCallbackPtr = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugReportCallbackEXT");
	VkDebugReportCallbackCreateInfoEXT dbgInfo;
	dbgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	dbgInfo.pNext = NULL;
	dbgInfo.pfnCallback = NebulaVulkanDebugCallback;
	dbgInfo.pUserData = NULL;
	res = this->debugCallbackPtr(this->instance, &dbgInfo, NULL, &this->debugCallbackHandle);
	n_assert(res == VK_SUCCESS);
#endif

	// get device props and features
	vkGetPhysicalDeviceProperties(this->physicalDev, &this->deviceProps);
	vkGetPhysicalDeviceFeatures(this->physicalDev, &this->deviceFeatures);

	// get number of queues
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDev, &this->numQueues, NULL);
	n_assert(this->numQueues > 0);

	// now get queues from device
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDev, &this->numQueues, this->queuesProps);
	vkGetPhysicalDeviceMemoryProperties(this->physicalDev, &this->memoryProps);

	// setup swap chain in display
	VkDisplayDevice::Instance()->SetupSwapchain();

	Util::FixedArray<VkBool32> canPresent(numQueues);
	for (i = 0; i < numQueues; i++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(VkRenderDevice::Instance()->physicalDev, i, VkDisplayDevice::Instance()->surface, &canPresent[i]);
		//this->surfaceSupport();
	}

	this->drawQueueIdx = UINT32_MAX;
	this->computeQueueIdx = UINT32_MAX;
	this->transferQueueIdx = UINT32_MAX;

	// create three queues for each family
	Util::FixedArray<uint> indexMap;
	indexMap.Resize(numQueues);
	indexMap.Fill(0);
	for (i = 0; i < numQueues; i++)
	{
		if (this->queuesProps[i].queueFlags == VK_QUEUE_TRANSFER_BIT && this->transferQueueIdx == UINT32_MAX)
		{
			if (this->queuesProps[i].queueCount == indexMap[i]) continue;
			this->transferQueueFamily = i;
			this->transferQueueIdx = indexMap[i]++;
			goto skip;
		}
		if (this->queuesProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && this->drawQueueIdx == UINT32_MAX)
		{
			if (this->queuesProps[i].queueCount == indexMap[i]) continue;
			uint32_t j;
			for (j = 0; j < numQueues; j++)
			{
				if (canPresent[i] == VK_TRUE)
				{
					this->drawQueueIdx = indexMap[i]++;
					this->drawQueueFamily = i;
					goto skip;
				}
			}
		}
		// also setup compute and transfer queues
		if (this->queuesProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT && this->computeQueueIdx == UINT32_MAX)
		{
			if (this->queuesProps[i].queueCount == indexMap[i]) continue;
			this->computeQueueFamily = i;
			this->computeQueueIdx = indexMap[i]++;
			goto skip;
		}
	skip:
		continue;
	}

	if (this->transferQueueIdx == UINT32_MAX)
	{
		// assert the draw queue can do both transfers and computes
		n_assert(this->queuesProps[this->drawQueueFamily].queueFlags & VK_QUEUE_TRANSFER_BIT);
		n_assert(this->queuesProps[this->drawQueueFamily].queueFlags & VK_QUEUE_COMPUTE_BIT);

		// this is actually sub-optimal, but on my AMD card, using the compute queue transfer or the sparse queue doesn't work
		this->transferQueueFamily = this->drawQueueFamily;
		this->transferQueueIdx = this->drawQueueIdx;
	}	

	if (this->drawQueueFamily == UINT32_MAX)		n_error("VkDisplayDevice: Could not find a queue for graphics and present.\n");
	if (this->computeQueueFamily == UINT32_MAX)		n_error("VkDisplayDevice: Could not find a queue for compute.\n");
	if (this->transferQueueFamily == UINT32_MAX)	n_error("VkDisplayDevice: Could not find a queue for transfers.\n");

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

	// setup display queue in render device (gfxIdx is different, because it's family doesn't have to be the displayable one)
	vkGetDeviceQueue(VkRenderDevice::dev, this->drawQueueFamily, this->drawQueueIdx, &VkRenderDevice::drawQueue);
	vkGetDeviceQueue(VkRenderDevice::dev, this->computeQueueFamily, this->computeQueueIdx, &VkRenderDevice::computeQueue);
	vkGetDeviceQueue(VkRenderDevice::dev, this->transferQueueFamily, this->transferQueueIdx, &VkRenderDevice::transferQueue);

	// find available surface formats
	uint32_t numFormats;
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(VkRenderDevice::physicalDev, VkDisplayDevice::Instance()->surface, &numFormats, NULL);
	n_assert(res == VK_SUCCESS);

	Util::FixedArray<VkSurfaceFormatKHR> formats(numFormats);
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(VkRenderDevice::physicalDev, VkDisplayDevice::Instance()->surface, &numFormats, formats.Begin());
	n_assert(res == VK_SUCCESS);
	this->format = formats[0].format;
	this->colorSpace = formats[0].colorSpace;
	for (i = 0; i < numFormats; i++)
	{
		if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
		{
			this->format = formats[i].format;
			this->colorSpace = formats[i].colorSpace;
			break;
		}
	}

	// get surface capabilities
	VkSurfaceCapabilitiesKHR surfCaps;
	res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDev, VkDisplayDevice::Instance()->surface, &surfCaps);
	n_assert(res == VK_SUCCESS);

	uint32_t numPresentModes;
	res = vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDev, VkDisplayDevice::Instance()->surface, &numPresentModes, NULL);
	n_assert(res == VK_SUCCESS);

	// get present modes
	Util::FixedArray<VkPresentModeKHR> presentModes(numPresentModes);
	res = vkGetPhysicalDeviceSurfacePresentModesKHR(this->physicalDev, VkDisplayDevice::Instance()->surface, &numPresentModes, presentModes.Begin());
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
		this->drawQueueIdx,
		NULL,
		transform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		swapchainPresentMode,
		true,
		NULL
	};

	// create swapchain
	res = vkCreateSwapchainKHR(this->dev, &swapchainInfo, NULL, &this->swapchain);
	n_assert(res == VK_SUCCESS);

	// get back buffers
	res = vkGetSwapchainImagesKHR(this->dev, this->swapchain, &this->numBackbuffers, NULL);
	n_assert(res == VK_SUCCESS);

	this->backbuffers.Resize(this->numBackbuffers);
	this->backbufferMem.Resize(this->numBackbuffers);
	res = vkGetSwapchainImagesKHR(this->dev, this->swapchain, &this->numBackbuffers, this->backbuffers.Begin());

	this->backbufferViews.Resize(this->numBackbuffers);
	for (i = 0; i < this->numBackbuffers; i++)
	{
		// allocate memory for back buffers
		uint32_t size;
		VkRenderDevice::Instance()->AllocateImageMemory(this->backbuffers[i], this->backbufferMem[i], VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
		vkBindImageMemory(this->dev, this->backbuffers[i], this->backbufferMem[i], 0);

		// setup view
		VkImageViewCreateInfo backbufferViewInfo = 
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			NULL,
			0,
			this->backbuffers[i],
			VK_IMAGE_VIEW_TYPE_2D,
			this->format,
			{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
			{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		};
		res = vkCreateImageView(this->dev, &backbufferViewInfo, NULL, &this->backbufferViews[i]);
		n_assert(res == VK_SUCCESS);
	}

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
	uint32_t descCounts[] =
	{
		256,
		256,
		262140,
		512,
		32,
		32,
		32,
		32,
		65535,
		256,
		64
	};
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
		sizes[i].descriptorCount = descCounts[i];
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
		this->drawQueueFamily
	};

	res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->mainCmdDrawPool);
	n_assert(res == VK_SUCCESS);

	cmdPoolInfo.queueFamilyIndex = this->computeQueueFamily;
	res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->mainCmdCmpPool);
	n_assert(res == VK_SUCCESS);

	cmdPoolInfo.queueFamilyIndex = this->transferQueueFamily;
	res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->mainCmdTransPool);
	n_assert(res == VK_SUCCESS);

	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->immediateCmdTransPool);
	n_assert(res == VK_SUCCESS);

	for (i = 0; i < NumDrawThreads; i++)
	{
		VkCommandPoolCreateInfo cmdPoolInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			NULL,
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			this->drawQueueFamily
		};
		res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->dispatchableCmdDrawBufferPool[i]);
	}

	for (i = 0; i < NumTransferThreads; i++)
	{
		VkCommandPoolCreateInfo cmdPoolInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			NULL,
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			this->transferQueueFamily
		};
		res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->dispatchableCmdTransBufferPool[i]);
	}

	for (i = 0; i < NumComputeThreads; i++)
	{
		VkCommandPoolCreateInfo cmdPoolInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			NULL,
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			this->computeQueueFamily
		};
		res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->dispatchableCmdCompBufferPool[i]);
	}
	
	// create main command buffer for graphics
	VkCommandBufferAllocateInfo cmdAllocInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->mainCmdDrawPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};
	res = vkAllocateCommandBuffers(VkRenderDevice::dev, &cmdAllocInfo, &this->mainCmdDrawBuffer);
	n_assert(res == VK_SUCCESS);

	// create main command buffer for computes
	cmdAllocInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->mainCmdCmpPool,
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
		this->mainCmdTransPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};
	res = vkAllocateCommandBuffers(VkRenderDevice::dev, &cmdAllocInfo, &this->mainCmdTransBuffer);
	n_assert(res == VK_SUCCESS);

	// setup draw threads
	Util::String threadName;
	for (i = 0; i < NumDrawThreads; i++)
	{
		threadName.Format("DrawCmdBufferThread%d", i);
		this->drawThreads[i] = VkCmdBufferThread::Create();
		this->drawThreads[i]->SetPriority(Threading::Thread::Low);
		this->drawThreads[i]->SetCoreId(System::Cpu::RenderThreadFirstCore + i);
		this->drawThreads[i]->SetName(threadName);
		this->drawThreads[i]->Start();

		this->drawCompletionEvents[i] = Threading::Event(true);
	}

	// setup transfer threads
	for (i = 0; i < NumTransferThreads; i++)
	{
		threadName.Format("TransferCmdBufferThread%d", i);
		this->transThreads[i] = VkCmdBufferThread::Create();
		this->transThreads[i]->SetPriority(Threading::Thread::Low);
		this->transThreads[i]->SetCoreId(System::Cpu::RenderThreadFirstCore + NumDrawThreads + i);
		this->transThreads[i]->SetName(threadName);
		this->transThreads[i]->Start();

		this->transCompletionEvents[i] = Threading::Event(true);
	}

	// setup compute threads
	for (i = 0; i < NumComputeThreads; i++)
	{
		threadName.Format("ComputeCmdBufferThread%d", i);
		this->compThreads[i] = VkCmdBufferThread::Create();
		this->compThreads[i]->SetPriority(Threading::Thread::Low);
		this->compThreads[i]->SetCoreId(System::Cpu::RenderThreadFirstCore + NumDrawThreads + NumTransferThreads + i);
		this->compThreads[i]->SetName(threadName);
		this->compThreads[i]->Start();

		this->compCompletionEvents[i] = Threading::Event(true);
	}

	// create interlock thread
	this->interlockThread = VkCpuGpuInterlockThread::Create();
	this->interlockThread->SetPriority(Threading::Thread::Low);
	this->interlockThread->SetCoreId(System::Cpu::RenderThreadFirstCore + NumDrawThreads + NumTransferThreads + NumComputeThreads);
	this->interlockThread->SetName("CPU-GPU interlock thread");
	this->interlockThread->dev = this->dev;
	this->interlockThread->Start();

	VkFenceCreateInfo fenceInfo =
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		NULL,
		0
	};

	res = vkCreateFence(this->dev, &fenceInfo, NULL, &this->mainCmdDrawFence);
	n_assert(res == VK_SUCCESS);
	res = vkCreateFence(this->dev, &fenceInfo, NULL, &this->mainCmdCmpFence);
	n_assert(res == VK_SUCCESS);
	res = vkCreateFence(this->dev, &fenceInfo, NULL, &this->mainCmdTransFence);
	n_assert(res == VK_SUCCESS);

	this->passInfo = 
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		NULL,
		0,
	};

	// setup pipeline construction struct
	this->currentPipelineInfo.pNext = NULL;
	this->currentPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	this->currentPipelineInfo.flags = 0;
	this->currentPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	this->currentPipelineInfo.basePipelineIndex = -1;
	this->currentPipelineInfo.pColorBlendState = &this->blendInfo;
	this->currentPipelineInfo.pVertexInputState = &this->vertexInfo;

	this->inputInfo.pNext = NULL;
	this->inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	this->inputInfo.flags = 0;

	this->blendInfo.pNext = NULL;
	this->blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	this->blendInfo.flags = 0;

	const CoreGraphics::DisplayMode& displayMode = DisplayDevice::Instance()->GetDisplayMode();
	this->displayRect.offset.x = 0;
	this->displayRect.offset.y = 0;
	this->displayRect.extent.width = displayMode.GetWidth();
	this->displayRect.extent.height = displayMode.GetHeight();

	const VkSemaphoreCreateInfo semInfo =
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		NULL,
		0
	};
	vkCreateSemaphore(this->dev, &semInfo, NULL, &this->displaySemaphore);

	// yay, Vulkan!
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::CloseVulkanDevice()
{
	vkDestroySwapchainKHR(this->dev, this->swapchain, NULL);

	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		this->drawThreads[i]->Stop();
		this->drawThreads[i] = 0;

		vkDestroyCommandPool(this->dev, this->dispatchableCmdDrawBufferPool[i], NULL);
	}

	for (i = 0; i < NumTransferThreads; i++)
	{
		this->transThreads[i]->Stop();
		this->transThreads[i] = 0;

		vkDestroyCommandPool(this->dev, this->dispatchableCmdTransBufferPool[i], NULL);
	}

	for (i = 0; i < NumComputeThreads; i++)
	{
		this->compThreads[i]->Stop();
		this->compThreads[i] = 0;

		vkDestroyCommandPool(this->dev, this->dispatchableCmdCompBufferPool[i], NULL);
	}

	// wait for all commands to be done first
	VkResult res = vkQueueWaitIdle(this->drawQueue);
	n_assert(res == VK_SUCCESS);

	// free our main buffers, our secondary buffers should be fine so the pools should be free to destroy
	vkFreeCommandBuffers(this->dev, this->mainCmdDrawPool, 1, &this->mainCmdDrawBuffer);
	vkFreeCommandBuffers(this->dev, this->mainCmdCmpPool, 1, &this->mainCmdCmpBuffer);
	vkFreeCommandBuffers(this->dev, this->mainCmdTransPool, 1, &this->mainCmdTransBuffer);
	vkDestroyCommandPool(this->dev, this->mainCmdDrawPool, NULL);
	vkDestroyCommandPool(this->dev, this->mainCmdDrawPool, NULL);
	vkDestroyCommandPool(this->dev, this->mainCmdCmpPool, NULL);
	vkDestroyCommandPool(this->dev, this->immediateCmdTransPool, NULL);
	vkDestroyFence(this->dev, this->mainCmdDrawFence, NULL);
	vkDestroyFence(this->dev, this->mainCmdCmpFence, NULL);
	vkDestroyFence(this->dev, this->mainCmdTransFence, NULL);

	for (i = 0; i < (int32_t)this->numBackbuffers; i++)
	{
		vkDestroyImage(this->dev, this->backbuffers[i], NULL);
		vkFreeMemory(this->dev, this->backbufferMem[i], NULL);
	}

	vkDestroyDevice(this->dev, NULL);
	vkDestroyInstance(this->instance, NULL);
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
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		VK_NULL_HANDLE
	};
	vkBeginCommandBuffer(this->mainCmdDrawBuffer, &cmdInfo);
	vkBeginCommandBuffer(this->mainCmdCmpBuffer, &cmdInfo);
	vkBeginCommandBuffer(this->mainCmdTransBuffer, &cmdInfo);

	// handle fences
	this->RunCommandPass(OnHandleTransferFences);
	this->RunCommandPass(OnHandleDrawFences);
	this->RunCommandPass(OnHandleComputeFences);

	// run command pass for beginning frame
	this->RunCommandPass(OnBeginFrame);

	// reset current thread
	this->currentDrawThread = NumDrawThreads - 1;
	this->currentPipelineBits = 0;

	return RenderDeviceBase::BeginFrame(frameIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetStreamVertexBuffer(IndexT streamIndex, const Ptr<CoreGraphics::VertexBuffer>& vb, IndexT offsetVertexIndex)
{
	// hmm, build pipeline before we start setting this stuff
	this->BuildRenderPipeline();

	RenderDeviceBase::SetStreamVertexBuffer(streamIndex, vb, offsetVertexIndex);
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::InputAssemblyVertex;
	cmd.vbo.buffer = vb->GetVkBuffer();
	cmd.vbo.index = streamIndex;
	cmd.vbo.offset = offsetVertexIndex;
	this->PushToThread(cmd, this->currentDrawThread);
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
	/*
	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		this->PushToThread(cmd, i);
		//this->drawThreads[i]->PushCommand(cmd);
	}
	*/
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetVertexLayout(const Ptr<CoreGraphics::VertexLayout>& vl)
{
	n_assert(this->currentProgram.isvalid());
	RenderDeviceBase::SetVertexLayout(vl);
	this->SetVertexLayoutPipelineInfo(vl->GetDerivative(this->currentProgram));
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& ib)
{
	// hmm, build pipeline before we start setting this stuff
	this->BuildRenderPipeline();

	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::InputAssemblyIndex;
	cmd.ibo.buffer = ib->GetVkBuffer();
	cmd.ibo.indexType = ib->GetIndexType() == IndexType::Index16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
	cmd.ibo.offset = 0;
	this->PushToThread(cmd, this->currentDrawThread);
	/*
	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		//this->PushToThread(cmd, i);
		//this->drawThreads[i]->PushCommand(cmd);
	}
	*/
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetPrimitiveGroup(const CoreGraphics::PrimitiveGroup& pg)
{
	RenderDeviceBase::SetPrimitiveGroup(pg);
	this->inputInfo.primitiveRestartEnable = VK_FALSE;
	this->inputInfo.topology = VkTypes::AsVkPrimitiveType(pg.GetPrimitiveTopology());
	this->SetInputLayoutInfo(&this->inputInfo);
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
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::RenderTarget>& rt)
{
	RenderDeviceBase::BeginPass(rt);

	const Ptr<DepthStencilTarget>& dst = rt->GetDepthStencilTarget();
	if (dst.isvalid()) dst->BeginPass();

	// set framebuffer info
	this->SetFramebufferLayoutInfo(rt->GetVkPipelineInfo());

	VkRect2D rect;
	rect.offset.x = 0;
	rect.offset.y = 0;
	rect.extent.width = rt->GetWidth();
	rect.extent.height = rt->GetHeight();

	const Util::FixedArray<VkClearValue>& clear = rt->GetVkClearValues();
	
	VkRenderPassBeginInfo info =
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		NULL,
		rt->GetVkRenderPass(),
		rt->GetVkFramebuffer(),
		rect,
		clear.Size(),
		clear.Size() > 0 ? clear.Begin() : VK_NULL_HANDLE
	};
	vkCmdBeginRenderPass(this->mainCmdDrawBuffer, &info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	//this->currentPipelineBits = 0;

	// run begin pass commands
	this->RunCommandPass(OnBeginPass);
	

	this->blendInfo.attachmentCount = 1;
	this->passInfo.framebuffer = rt->GetVkFramebuffer();
	this->passInfo.renderPass = rt->GetVkRenderPass();
	this->passInfo.subpass = 0;
	this->passInfo.pipelineStatistics = 0;
	this->passInfo.queryFlags = 0;
	this->passInfo.occlusionQueryEnable = VK_FALSE;

	const Util::FixedArray<VkRect2D>& scissors = rt->GetVkScissorRects();
	this->numScissors = scissors.Size();
	this->scissors = scissors.Begin();
	const Util::FixedArray<VkViewport>& viewports = rt->GetVkViewports();
	this->numViewports = viewports.Size();
	this->viewports = viewports.Begin();

	//this->BeginDrawThreadCluster();
	//this->BindSharedDescriptorSets();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::MultipleRenderTarget>& mrt)
{
	RenderDeviceBase::BeginPass(mrt);

	const Ptr<DepthStencilTarget>& dst = mrt->GetDepthStencilTarget();
	if (dst.isvalid()) dst->BeginPass();

	// set framebuffer info
	this->SetFramebufferLayoutInfo(mrt->GetVkPipelineInfo());

	VkRect2D rect;
	rect.offset.x = 0;
	rect.offset.y = 0;
	rect.extent.width = mrt->GetRenderTarget(0)->GetWidth();
	rect.extent.height = mrt->GetRenderTarget(0)->GetHeight();

	const Util::Array<VkClearValue>& clear = mrt->GetVkClearValues();

	VkRenderPassBeginInfo info =
	{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		NULL,
		mrt->GetVkRenderPass(),
		mrt->GetVkFramebuffer(),
		rect,
		clear.Size(),
		clear.Size() > 0 ? clear.Begin() : VK_NULL_HANDLE
	};
	vkCmdBeginRenderPass(this->mainCmdDrawBuffer, &info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	//this->currentPipelineBits = 0;

	// run begin pass commands
	this->RunCommandPass(OnBeginPass);

	this->blendInfo.attachmentCount = mrt->GetNumRendertargets();
	this->passInfo.framebuffer = mrt->GetVkFramebuffer();
	this->passInfo.renderPass = mrt->GetVkRenderPass();
	this->passInfo.subpass = 0;
	this->passInfo.pipelineStatistics = 0;
	this->passInfo.queryFlags = 0;
	this->passInfo.occlusionQueryEnable = VK_FALSE;

	const Util::Array<VkRect2D>& scissors = mrt->GetVkScissorRects();
	this->numScissors = scissors.Size();
	this->scissors = scissors.Begin();
	const Util::Array<VkViewport>& viewports = mrt->GetVkViewports();
	this->numViewports = viewports.Size();
	this->viewports = viewports.Begin();

	//this->BeginDrawThreadCluster();
	//this->BindSharedDescriptorSets();
}

//------------------------------------------------------------------------------
/**
	FIXME!
*/
void
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::RenderTargetCube>& rtc)
{
	RenderDeviceBase::BeginPass(rtc);
	this->currentPipelineBits = 0;
	this->SetFramebufferLayoutInfo(rtc->GetVkPipelineInfo());

	// submit
	this->SubmitToQueue(this->drawQueue, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 1, &this->mainCmdDrawBuffer);

	this->blendInfo.attachmentCount = 1;

	VkResult res = vkResetCommandBuffer(this->mainCmdDrawBuffer, 0);
	n_assert(res == VK_SUCCESS);

	// start constructing draws
	//this->BeginCmdThreads();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb, CoreGraphics::PrimitiveTopology::Code primType)
{
	RenderDeviceBase::BeginFeedback(fb, primType);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginBatch(CoreGraphics::FrameBatchType::Code batchType)
{
	RenderDeviceBase::BeginBatch(batchType);
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
	this->PushToThread(cmd, this->currentDrawThread);
	//this->threadCmds[this->currentDrawThread].Append(cmd);
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);

	// go to next thread
	//this->NextThread();
	_incr_counter(RenderDeviceNumDrawCalls, 1);
	_incr_counter(RenderDeviceNumPrimitives, this->primitiveGroup.GetNumVertices()/3);
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
	this->PushToThread(cmd, this->currentDrawThread);
	//this->threadCmds[this->currentDrawThread].Append(cmd);
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);

	// go to next thread
	//this->NextThread();
	_incr_counter(RenderDeviceNumDrawCalls, 1);
	_incr_counter(RenderDeviceNumPrimitives, this->primitiveGroup.GetNumIndices()/3);
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
	// submit threaded buffers
	//this->SubmitToQueue(this->renderQueue, NumDrawThreads, this->dispatchableDrawCmdBuffers);


	// free up our threaded command buffers
	//vkFreeCommandBuffers(this->dev, this->cmdGfxPool[Transient], NumDrawThreads, this->dispatchableDrawCmdBuffers);

	this->currentPipelineBits = FramebufferLayoutInfoSet;
	//Memory::Clear(&this->currentPipelineInfo, sizeof(VkGraphicsPipelineCreateInfo));

	RenderDeviceBase::EndBatch();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndPass()
{
	this->currentPipelineBits = 0;

	/// end draw threads, if any are remaining
	//this->EndDrawThreads();
	this->EndDrawThreads();

	// finish command buffer threads
	//this->EndCmdThreads();

	// execute subpass buffers
	//this->EndSubpassCommands();

	// run end command pass before we actually end the render pass
	this->RunCommandPass(OnEndPass);

	// end render pass
	vkCmdEndRenderPass(this->mainCmdDrawBuffer);

	if (this->passRenderTarget.isvalid())
	{
		const Ptr<DepthStencilTarget>& dst = this->passRenderTarget->GetDepthStencilTarget();
		if (dst.isvalid()) dst->EndPass();
	}
	else if (this->passMultipleRenderTarget.isvalid())
	{
		const Ptr<DepthStencilTarget>& dst = this->passMultipleRenderTarget->GetDepthStencilTarget();
		if (dst.isvalid()) dst->EndPass();
	}
	RenderDeviceBase::EndPass();
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
	Ideally implement some way of using double-triple-N buffering here, by having more than one fence per frame.
*/
void
VkRenderDevice::EndFrame(IndexT frameIndex)
{
	RenderDeviceBase::EndFrame(frameIndex);

	// run end-of-frame commands
	this->RunCommandPass(OnEndFrame);

	VkFenceCreateInfo info =
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		0,
		0
	};

	// submit transfer stuff
	this->SubmitToQueue(this->transferQueue, VK_PIPELINE_STAGE_TRANSFER_BIT, 1, &this->mainCmdTransBuffer);
	this->SubmitToQueue(this->transferQueue, this->mainCmdTransFence);
	if (this->putTransferFenceThisFrame)
	{
		VkFence fence;
		VkResult res = vkCreateFence(this->dev, &info, NULL, &fence);
		n_assert(res == VK_SUCCESS);
		this->transferFenceCommands.Add(fence, this->commands[OnHandleTransferFences]);
		this->commands[OnHandleTransferFences].Clear();
		this->SubmitToQueue(this->transferQueue, fence);
		this->putTransferFenceThisFrame = false;
	}
	this->RunCommandPass(OnMainTransferSubmitted);

	// submit draw stuff
	this->SubmitToQueue(this->drawQueue, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 1, &this->mainCmdDrawBuffer);
	this->SubmitToQueue(this->drawQueue, this->mainCmdDrawFence);
	if (this->putDrawFenceThisFrame)
	{
		VkFence fence;
		VkResult res = vkCreateFence(this->dev, &info, NULL, &fence);
		n_assert(res == VK_SUCCESS);
		this->drawFenceCommands.Add(fence, this->commands[OnHandleDrawFences]);
		this->commands[OnHandleDrawFences].Clear();
		this->SubmitToQueue(this->drawQueue, fence);
		this->putDrawFenceThisFrame = false;
	}
	this->RunCommandPass(OnMainDrawSubmitted);
	
	// submit comput stuff
	this->SubmitToQueue(this->computeQueue, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 1, &this->mainCmdCmpBuffer);
	this->SubmitToQueue(this->computeQueue, this->mainCmdCmpFence);	
	if (this->putComputeFenceThisFrame)
	{
		VkFence fence;
		VkResult res = vkCreateFence(this->dev, &info, NULL, &fence);
		n_assert(res == VK_SUCCESS);
		this->computeFenceCommands.Add(fence, this->commands[OnHandleComputeFences]);
		this->commands[OnHandleComputeFences].Clear();
		this->SubmitToQueue(this->computeQueue, fence);
		this->putComputeFenceThisFrame = false;
	}
	this->RunCommandPass(OnMainComputeSubmitted);

	VkFence fences[] = { this->mainCmdDrawFence, this->mainCmdTransFence, this->mainCmdCmpFence };
	this->WaitForFences(fences, 3, true);

	VkResult res = vkResetCommandBuffer(this->mainCmdTransBuffer, 0);
	n_assert(res == VK_SUCCESS);
	res = vkResetCommandBuffer(this->mainCmdCmpBuffer, 0);
	n_assert(res == VK_SUCCESS);
	res = vkResetCommandBuffer(this->mainCmdDrawBuffer, 0);
	n_assert(res == VK_SUCCESS);
	//vkEndCommandBuffer(this->mainCmdGfxBuffer);
	//vkEndCommandBuffer(this->mainCmdCmpBuffer);
	//vkEndCommandBuffer(this->mainCmdTransBuffer);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::Present()
{
	this->frameId++; 

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
	VkResult res = vkQueueSubmit(this->drawQueue, 1, &submitInfo, NULL);
	n_assert(res == VK_SUCCESS);
	res = vkQueueWaitIdle(this->drawQueue);
	n_assert(res == VK_SUCCESS);

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
	res = vkQueuePresentKHR(this->drawQueue, &info);

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
VkRenderDevice::SetScissorRect(const Math::rectangle<int>& rect, int index)
{
	VkRect2D sc;
	sc.extent.height = rect.height();
	sc.extent.width = rect.width();
	sc.offset.x = rect.left;
	sc.offset.y = rect.top;
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::ScissorRect;
	cmd.scissorRect.index = index;
	cmd.scissorRect.sc = sc;
	this->PushToThread(cmd, this->currentDrawThread);
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
	//vkCmdSetScissor(this->mainCmdGfxBuffer, index, 1, &sc);
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
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::Viewport;
	cmd.viewport.index = index;
	cmd.viewport.vp = vp;
	this->PushToThread(cmd, this->currentDrawThread);
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
	//vkCmdSetViewport(this->mainCmdGfxBuffer, index, 1, &vp);
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
VkRenderDevice::SetShaderPipelineInfo(const VkGraphicsPipelineCreateInfo& shader, const Ptr<VkShaderProgram>& program)
{
#define uint_min(a, b) (a < b ? a : b)
	this->currentProgram = program;
	this->currentPipelineBits |= ShaderInfoSet;

	this->blendInfo.pAttachments = shader.pColorBlendState->pAttachments;
	memcpy(this->blendInfo.blendConstants, shader.pColorBlendState->blendConstants, sizeof(float) * 4);
	this->blendInfo.logicOp = shader.pColorBlendState->logicOp;
	this->blendInfo.logicOpEnable = shader.pColorBlendState->logicOpEnable;

	this->currentPipelineInfo.pDepthStencilState = shader.pDepthStencilState;
	this->currentPipelineInfo.pRasterizationState = shader.pRasterizationState;
	this->currentPipelineInfo.pMultisampleState = shader.pMultisampleState;
	this->currentPipelineInfo.pDynamicState = shader.pDynamicState;
	this->currentPipelineInfo.pTessellationState = shader.pTessellationState;
	this->currentPipelineInfo.stageCount = shader.stageCount;
	this->currentPipelineInfo.pStages = shader.pStages;
	this->currentPipelineInfo.layout = shader.layout;
	this->currentPipelineBits &= ~PipelineBuilt;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetVertexLayoutPipelineInfo(const VkPipelineVertexInputStateCreateInfo& vertexLayout)
{
#define uint_min(a, b) (a < b ? a : b)

	this->currentPipelineBits |= VertexLayoutInfoSet;
	this->vertexInfo = vertexLayout;

	this->currentPipelineBits &= ~PipelineBuilt;
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
	this->currentPipelineInfo.pViewportState = framebufferLayout.pViewportState;
	this->currentPipelineBits &= ~PipelineBuilt;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetInputLayoutInfo(VkPipelineInputAssemblyStateCreateInfo* inputLayout)
{
	this->currentPipelineBits |= InputLayoutInfoSet;
	this->currentPipelineInfo.pInputAssemblyState = inputLayout;
	this->currentPipelineBits &= ~PipelineBuilt;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BindDescriptorsGraphics(const VkDescriptorSet* descriptors, const VkPipelineLayout& layout, uint32_t baseSet, uint32_t setCount, const uint32_t* offsets, uint32_t offsetCount)
{
	if (this->numActiveThreads > 0)
	{
		VkCmdBufferThread::Command cmd;
		cmd.type = VkCmdBufferThread::BindDescriptors;
		cmd.descriptor.baseSet = baseSet;
		cmd.descriptor.numSets = setCount;
		cmd.descriptor.sets = descriptors;
		cmd.descriptor.layout = layout;
		cmd.descriptor.numOffsets = offsetCount;
		cmd.descriptor.offsets = offsets;
		cmd.descriptor.type = VK_PIPELINE_BIND_POINT_GRAPHICS;
		/*
		IndexT i;
		for (i = 0; i < NumDrawThreads; i++)
		{
			//this->PushToThread(cmd, i);
			//this->drawThreads[i]->PushCommand(cmd);
		}
		*/
		this->PushToThread(cmd, this->currentDrawThread);
		//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
	}
	else
	{
		VkDeferredCommand cmd;
		cmd.del.type = VkDeferredCommand::BindDescriptorSets;
		cmd.del.descSetBind.baseSet = baseSet;
		cmd.del.descSetBind.numSets = setCount;
		cmd.del.descSetBind.sets = descriptors;
		cmd.del.descSetBind.layout = layout;
		cmd.del.descSetBind.numOffsets = offsetCount;
		cmd.del.descSetBind.offsets = offsets;		
		cmd.del.descSetBind.type = VK_PIPELINE_BIND_POINT_GRAPHICS;
		cmd.dev = this->dev;
		this->PushCommandPass(cmd, OnBindGraphicsPipeline);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BindDescriptorsCompute(const VkDescriptorSet* descriptors, const VkPipelineLayout& layout, uint32_t baseSet, uint32_t setCount, const uint32_t* offsets, uint32_t offsetCount)
{
	if (this->inBeginFrame)
	{
		vkCmdBindDescriptorSets(this->mainCmdCmpBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, layout, baseSet, setCount, descriptors, offsetCount, offsets);
	}
	else
	{
		VkDeferredCommand cmd;
		cmd.del.type = VkDeferredCommand::BindDescriptorSets;
		cmd.del.descSetBind.baseSet = baseSet;
		cmd.del.descSetBind.layout = layout;
		cmd.del.descSetBind.numOffsets = offsetCount;
		cmd.del.descSetBind.offsets = offsets;
		cmd.del.descSetBind.numSets = setCount;
		cmd.del.descSetBind.sets = descriptors;
		cmd.del.descSetBind.type = VK_PIPELINE_BIND_POINT_COMPUTE;
		cmd.dev = this->dev;
		this->PushCommandPass(cmd, OnBindComputePipeline);
	}
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
	this->PushToThread(cmd, this->currentDrawThread);
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BufferUpdate(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, const void* data)
{
	VkDeviceSize totalSize = size;
	VkDeviceSize totalOffset = offset;
	while (totalSize > 0)
	{
		const uint8_t* ptr = (const uint8_t*)data + totalOffset;
		VkDeviceSize uploadSize = totalSize < 65536 ? totalSize : 65536;
		vkCmdUpdateBuffer(this->mainCmdTransBuffer, buf, totalOffset, uploadSize, ptr);
		totalSize -= uploadSize;
		totalOffset += uploadSize;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BufferUpdate(VkCommandBuffer cmd, const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, const void* data)
{
	VkDeviceSize totalSize = size;
	VkDeviceSize totalOffset = offset;
	while (totalSize > 0)
	{
		const uint8_t* ptr = (const uint8_t*)data + totalOffset;
		VkDeviceSize uploadSize = totalSize < 65536 ? totalSize : 65536;
		/*
		VkCmdBufferThread::Command cmd;
		cmd.updBuffer.buf = buf;
		cmd.updBuffer.data = (uint32_t*)data;
		cmd.updBuffer.offset = offset;
		cmd.updBuffer.size = size;
		cmd.updBuffer.deleteWhenDone = false;
		cmd.type = VkCmdBufferThread::UpdateBuffer;
		*/
		//this->PushToThread(cmd, this->currentDrawThread, true);
		vkCmdUpdateBuffer(cmd, buf, totalOffset, uploadSize, ptr);
		totalSize -= uploadSize;
		totalOffset += uploadSize;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::ImageUpdate(const VkImage& img, const VkImageCreateInfo& info, uint32_t mip, uint32_t face, VkDeviceSize size, uint32_t* data)
{
	// create transfer buffer
	VkBufferCreateInfo bufInfo =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, 
		NULL, 
		0,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		1,
		&this->transferQueueFamily
	};
	VkBuffer buf;
	vkCreateBuffer(this->dev, &bufInfo, NULL, &buf);

	// allocate memory
	VkDeviceMemory bufMem;
	uint32_t bufsize;
	this->AllocateBufferMemory(buf, bufMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, bufsize);
	vkBindBufferMemory(this->dev, buf, bufMem, 0);

	// map memory
	void* mapped;
	VkResult res = vkMapMemory(this->dev, bufMem, 0, size, 0, &mapped);
	n_assert(res == VK_SUCCESS);
	memcpy(mapped, data, VK_DEVICE_SIZE_CONV(size));
	vkUnmapMemory(this->dev, bufMem);

	// perform update of buffer, and stage a copy of buffer data to image
	VkBufferImageCopy copy;
	copy.bufferOffset = 0;
	copy.bufferImageHeight = info.extent.height;
	copy.bufferRowLength = VK_DEVICE_SIZE_CONV(size) / info.extent.height;
	copy.imageExtent = info.extent;
	copy.imageOffset = { 0, 0, 0};
	copy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, mip, face, 1 };
	vkCmdCopyBufferToImage(this->mainCmdTransBuffer, buf, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

	// finally push delegates to dealloc all our staging data
	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::FreeBuffer;
	del.del.buffer.buf = buf;
	del.del.buffer.mem = bufMem;
	del.del.queue = VkDeferredCommand::Transfer;
	del.dev = this->dev;
	this->PushCommandPass(del, OnHandleTransferFences);

	// finally push delegates to dealloc all our staging data
	VkDeferredCommand del2;
	del2.del.type = VkDeferredCommand::FreeMemory;
	del2.del.memory.data = data;
	del2.del.queue = VkDeferredCommand::Transfer;
	del2.dev = this->dev;
	this->PushCommandPass(del2, OnHandleTransferFences);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushImageUpdate(const VkImage& img, const VkImageCreateInfo& info, uint32_t mip, uint32_t face, VkDeviceSize size, uint32_t* data)
{
	uint32_t* imgCopy = (uint32_t*)n_new_array(uint32_t, VK_DEVICE_SIZE_CONV(size));
	Memory::Copy(data, imgCopy, VK_DEVICE_SIZE_CONV(size));

	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::UpdateImage;
	del.del.imageUpd.img = img;
	del.del.imageUpd.info = info;
	del.del.imageUpd.mip = mip;
	del.del.imageUpd.face = face;
	del.del.imageUpd.size = size;
	del.del.imageUpd.data = imgCopy;
	del.del.queue = VkDeferredCommand::Transfer;
	del.dev = this->dev;
	this->PushCommandPass(del, OnBeginFrame);
}

//------------------------------------------------------------------------------
/**
	Begins an immediate command buffer for data transfers, and returns the buffer within which the image data is contained.
*/
void
VkRenderDevice::ReadImage(const Ptr<VkTexture>& tex, VkImageCopy copy, uint32_t& outMemSize, VkDeviceMemory& outMem, VkImage& outImage)
{
	VkCommandBuffer cmdBuf = this->BeginImmediateTransfer();

	VkFormat fmt = VkTypes::AsVkFormat(tex->GetPixelFormat());
	Texture::Type type = tex->GetType();
	VkImageCreateInfo info =
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		NULL,
		0,
		type == Texture::Texture2D ? VK_IMAGE_TYPE_2D :
		type == Texture::Texture3D ? VK_IMAGE_TYPE_3D :
		type == Texture::TextureCube ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_1D,
		fmt,
		copy.extent,
		1,
		type == Texture::TextureCube ? 6 : type == Texture::Texture3D ? tex->GetDepth() : 1,
		VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_LINEAR,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		NULL,
		VK_IMAGE_LAYOUT_PREINITIALIZED
	};
	VkImage img;
	vkCreateImage(this->dev, &info, NULL, &img);

	// allocate memory
	VkDeviceMemory imgMem;
	uint32_t memSize;
	this->AllocateImageMemory(img, imgMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memSize);
	vkBindImageMemory(this->dev, img, imgMem, 0);

	VkImageSubresourceRange srcSubres;
	srcSubres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	srcSubres.baseArrayLayer = copy.srcSubresource.baseArrayLayer;
	srcSubres.layerCount = copy.srcSubresource.layerCount;
	srcSubres.baseMipLevel = copy.srcSubresource.mipLevel;
	srcSubres.levelCount = 1;
	VkImageSubresourceRange dstSubres;
	dstSubres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dstSubres.baseArrayLayer = copy.dstSubresource.baseArrayLayer;
	dstSubres.layerCount = copy.dstSubresource.layerCount;
	dstSubres.baseMipLevel = copy.dstSubresource.mipLevel;
	dstSubres.levelCount = 1;

	// perform update of buffer, and stage a copy of buffer data to image
	this->ImageLayoutTransition(cmdBuf, this->ImageMemoryBarrier(img, dstSubres, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL));
	this->ImageLayoutTransition(cmdBuf, this->ImageMemoryBarrier(tex->GetVkImage(), srcSubres, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL));
	vkCmdCopyImage(cmdBuf, tex->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
	this->ImageLayoutTransition(cmdBuf, this->ImageMemoryBarrier(tex->GetVkImage(), srcSubres, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	//vkCmdCopyImageToBuffer(this->mainCmdCmpBuffer, img, VK_IMAGE_LAYOUT_GENERAL, buf, 1, &copy);

	// end immediate command buffer
	this->EndImmediateTransfer(cmdBuf);

	outImage = img;
	outMem = imgMem;
	outMemSize = VK_DEVICE_SIZE_CONV(memSize);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::WriteImage(const VkImage& srcImg, const VkImage& dstImg, VkImageCopy copy)
{
	//VkCommandBuffer cmdBuf = this->BeginImmediateTransfer();
	//vkCmdCopyBufferToImage(cmdBuf, buf, img, VK_IMAGE_LAYOUT_GENERAL, 1, &copy);
	//this->EndImmediateTransfer(cmdBuf);
}

//------------------------------------------------------------------------------
/**
*/
VkCommandBuffer
VkRenderDevice::BeginImmediateTransfer()
{
	// allocate command buffer we can use to execute 
	VkCommandBufferAllocateInfo cmdAlloc =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->immediateCmdTransPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};
	VkCommandBuffer cmdBuf;
	vkAllocateCommandBuffers(this->dev, &cmdAlloc, &cmdBuf);

	// this is why this is slow, we must perform a begin-end-submit of the command buffer for this to work
	VkCommandBufferBeginInfo begin =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		0,
		NULL
	};
	vkBeginCommandBuffer(cmdBuf, &begin);
	return cmdBuf;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndImmediateTransfer(VkCommandBuffer cmdBuf)
{
	// end command
	vkEndCommandBuffer(cmdBuf);

	VkSubmitInfo submit =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		NULL,
		0, NULL, NULL,
		1, &cmdBuf,
		0, NULL
	};

	VkFenceCreateInfo fence =
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		NULL,
		0
	};

	// create a fence we can wait for, and execute this very tiny command buffer
	VkResult res;
	VkFence sync;
	res = vkCreateFence(this->dev, &fence, NULL, &sync);
	n_assert(res == VK_SUCCESS);
	res = vkQueueSubmit(this->transferQueue, 1, &submit, sync);
	n_assert(res == VK_SUCCESS);

	// wait for fences, this waits for our commands to finish
	res = vkWaitForFences(this->dev, 1, &sync, true, UINT_MAX);
	n_assert(res == VK_SUCCESS);

	// cleanup fence, buffer and buffer memory
	vkDestroyFence(this->dev, sync, NULL);
	vkFreeCommandBuffers(this->dev, this->immediateCmdTransPool, 1, &cmdBuf);
}

//------------------------------------------------------------------------------
/**
*/
VkCommandBuffer
VkRenderDevice::BeginInterlockedTransfer()
{
	// allocate command buffer we can use to execute 
	VkCommandBufferAllocateInfo cmdAlloc =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->immediateCmdTransPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};
	VkCommandBuffer cmdBuf;
	vkAllocateCommandBuffers(this->dev, &cmdAlloc, &cmdBuf);

	// this is why this is slow, we must perform a begin-end-submit of the command buffer for this to work
	VkCommandBufferBeginInfo begin =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		0,
		NULL
	};
	vkBeginCommandBuffer(cmdBuf, &begin);
	return cmdBuf;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndInterlockedTransfer(VkCommandBuffer cmdBuf)
{
	// end command
	vkEndCommandBuffer(cmdBuf);

	VkSubmitInfo submit =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		NULL,
		0, NULL, NULL,
		1, &cmdBuf,
		0, NULL
	};

	VkFenceCreateInfo fence =
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		NULL,
		0
	};

	// create a fence we can wait for, and execute this very tiny command buffer
	VkResult res;
	res = vkQueueSubmit(this->transferQueue, 1, &submit, VK_NULL_HANDLE);
	n_assert(res == VK_SUCCESS);

	// add delegate to delete command buffer
	VkDeferredCommand cmd;
	cmd.del.cmdbufferfree.buffers[0] = cmdBuf;
	cmd.del.cmdbufferfree.numBuffers = 1;
	cmd.del.cmdbufferfree.pool = this->immediateCmdTransPool;
	cmd.dev = this->dev;
	this->PushCommandPass(cmd, OnHandleTransferFences);
}

//------------------------------------------------------------------------------
/**
*/
void
Vulkan::VkRenderDevice::SubmitToQueue(VkQueue queue, VkPipelineStageFlags flags, uint32_t numBuffers, VkCommandBuffer* buffers)
{
	uint32_t i;
	for (i = 0; i < numBuffers; i++)
	{
		VkResult res = vkEndCommandBuffer(buffers[i]);
		n_assert(res == VK_SUCCESS);
	}

	// submit to queue
	const VkSubmitInfo submitInfo =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		NULL,
		0,
		NULL,
		&flags,
		numBuffers,
		buffers,
		0,
		NULL
	};

	// submit to queue
	VkResult res = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	n_assert(res == VK_SUCCESS);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SubmitToQueue(VkQueue queue, VkFence fence)
{
	// submit to queue
	VkResult res = vkQueueSubmit(queue, 0, VK_NULL_HANDLE, fence);
	n_assert(res == VK_SUCCESS);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BindComputePipeline(const VkPipeline& pipeline, const VkPipelineLayout& layout)
{
	// bind compute pipeline
	vkCmdBindPipeline(this->mainCmdCmpBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, layout);

	// run command pass
	this->RunCommandPass(OnBindComputePipeline);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::CreateAndBindGraphicsPipeline()
{
	VkPipeline pipeline;
	VkResult res = vkCreateGraphicsPipelines(this->dev, this->cache, 1, &this->currentPipelineInfo, NULL, &pipeline);
	n_assert(res == VK_SUCCESS);

	// if all threads are in use, finish them
	if (this->currentDrawThread == NumDrawThreads-1 && this->numActiveThreads > 0)
	//if (this->numActiveThreads > 0)
	{
		this->EndDrawThreads();
		//this->EndDrawThreadCluster();
	}

	// begin new draw thread
	this->currentDrawThread = (this->currentDrawThread + 1) % NumDrawThreads;
	//this->BeginDrawThreadCluster();
	this->BeginDrawThread();

	VkCmdBufferThread::Command cmd;

	// send pipeline bind command, this is the first step in our procedure, so we use this as a trigger to switch threads
	cmd.type = VkCmdBufferThread::GraphicsPipeline;
	cmd.pipeline = pipeline;
	this->PushToThread(cmd, this->currentDrawThread);
	/*
	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		this->PushToThread(cmd, i);
		//this->drawThreads[i]->PushCommand(cmd);
	}
	*/
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);

	// push scissors
	cmd.type = VkCmdBufferThread::ScissorRectArray;
	cmd.scissorRectArray.first = 0;
	cmd.scissorRectArray.num = this->numScissors;
	cmd.scissorRectArray.scs = this->scissors;
	this->PushToThread(cmd, this->currentDrawThread);
	/*
	for (i = 0; i < NumDrawThreads; i++)
	{
		this->PushToThread(cmd, i);
		//this->drawThreads[i]->PushCommand(cmd);
	}
	*/
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);

	// push viewports
	cmd.type = VkCmdBufferThread::ViewportArray;
	cmd.viewportArray.first = 0;
	cmd.viewportArray.num = this->numViewports;
	cmd.viewportArray.vps = this->viewports;
	this->PushToThread(cmd, this->currentDrawThread);
	/*
	for (i = 0; i < NumDrawThreads; i++)
	{
		this->PushToThread(cmd, i);
		//this->drawThreads[i]->PushCommand(cmd);
	}
	*/
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);

	this->BindSharedDescriptorSets();

	// run command pass
	this->RunCommandPass(OnBindGraphicsPipeline);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BuildRenderPipeline()
{
	n_assert((this->currentPipelineBits & AllInfoSet) != 0);
	if ((this->currentPipelineBits & PipelineBuilt) == 0)
	{
		this->CreateAndBindGraphicsPipeline();
		this->currentPipelineBits |= PipelineBuilt;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushImageLayoutTransition(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier)
{
	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::ImageLayoutTransition;
	del.del.imgBarrier.barrier = barrier;
	del.del.queue = queue;
	del.dev = this->dev;
	this->PushCommandPass(del, OnBeginFrame);	
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::ImageLayoutTransition(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier)
{
	VkCommandBuffer buf;
	VkPipelineStageFlags flags;
	switch (queue)
	{
	case VkDeferredCommand::Graphics: buf = this->mainCmdDrawBuffer; flags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT; break;
	case VkDeferredCommand::Transfer: buf = this->mainCmdTransBuffer; flags = VK_PIPELINE_STAGE_TRANSFER_BIT; break;
	case VkDeferredCommand::Compute: buf = this->mainCmdCmpBuffer; flags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; break;
	}

	// execute command
	vkCmdPipelineBarrier(buf, 
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
		0,
		0, VK_NULL_HANDLE,
		0, VK_NULL_HANDLE,
		1, &barrier);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::ImageLayoutTransition(VkCommandBuffer buf, VkImageMemoryBarrier barrier)
{
	// execute command
	vkCmdPipelineBarrier(buf,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, VK_NULL_HANDLE,
		0, VK_NULL_HANDLE,
		1, &barrier);
}

//------------------------------------------------------------------------------
/**
*/
VkImageMemoryBarrier
VkRenderDevice::ImageMemoryBarrier(const VkImage& img, VkImageSubresourceRange subres, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = NULL;
	barrier.image = img;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange = subres;
	switch (oldLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
	case VK_IMAGE_LAYOUT_UNDEFINED:
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	}

	switch (newLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.srcAccessMask = barrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = barrier.srcAccessMask | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.srcAccessMask = barrier.srcAccessMask | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		barrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	}
	return barrier;
}

//------------------------------------------------------------------------------
/**
*/
VkBufferMemoryBarrier
VkRenderDevice::BufferMemoryBarrier(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, VkAccessFlags srcAccess, VkAccessFlags dstAccess)
{
	VkBufferMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.pNext = NULL;
	barrier.buffer = buf;
	barrier.dstAccessMask = dstAccess;
	barrier.srcAccessMask = srcAccess;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.size = size;
	barrier.offset = offset;
	return barrier;
}

//------------------------------------------------------------------------------
/**
*/
VkImageMemoryBarrier
VkRenderDevice::ImageMemoryBarrier(const VkImage& img, VkImageSubresourceRange subres, VkDeferredCommand::CommandQueueType fromQueue, VkDeferredCommand::CommandQueueType toQueue, VkImageLayout layout)
{
	uint32_t from;
	switch (fromQueue)
	{
	case VkDeferredCommand::Graphics: from = VkRenderDevice::Instance()->drawQueueFamily; break;
	case VkDeferredCommand::Compute: from = VkRenderDevice::Instance()->computeQueueFamily; break;
	case VkDeferredCommand::Transfer: from = VkRenderDevice::Instance()->transferQueueFamily; break;
	}

	uint32_t to;
	switch (toQueue)
	{
	case VkDeferredCommand::Graphics: to = VkRenderDevice::Instance()->drawQueueFamily; break;
	case VkDeferredCommand::Compute: to = VkRenderDevice::Instance()->computeQueueFamily; break;
	case VkDeferredCommand::Transfer: to = VkRenderDevice::Instance()->transferQueueFamily; break;
	}

	VkImageMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = NULL;
	barrier.image = img;
	barrier.oldLayout = layout;
	barrier.newLayout = layout;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.srcQueueFamilyIndex = from;
	barrier.dstQueueFamilyIndex = to;
	barrier.subresourceRange = subres;
	return barrier;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushImageOwnershipChange(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier)
{
	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::ImageOwnershipChange;
	del.del.imgOwnerChange.barrier = barrier;
	del.del.queue = queue;
	del.dev = this->dev;
	this->PushCommandPass(del, OnBeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::ImageOwnershipChange(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier)
{
	VkCommandBuffer buf;
	VkPipelineStageFlags flags;
	switch (queue)
	{
	case VkDeferredCommand::Graphics: buf = this->mainCmdDrawBuffer; flags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT; break;
	case VkDeferredCommand::Transfer: buf = this->mainCmdTransBuffer; flags = VK_PIPELINE_STAGE_TRANSFER_BIT; break;
	case VkDeferredCommand::Compute: buf = this->mainCmdCmpBuffer; flags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; break;
	}

	// execute command
	vkCmdPipelineBarrier(buf,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, VK_NULL_HANDLE,
		0, VK_NULL_HANDLE,
		1, &barrier);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushImageColorClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearColorValue clearValue, VkImageSubresourceRange subres)
{
	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::ClearColorImage;
	del.del.imgColorClear.clearValue = clearValue;
	del.del.imgColorClear.img = image;
	del.del.imgColorClear.layout = layout;
	del.del.imgColorClear.region = subres;
	del.del.queue = queue;
	del.dev = this->dev;
	this->PushCommandPass(del, OnBeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::ImageColorClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearColorValue clearValue, VkImageSubresourceRange subres)
{
	VkCommandBuffer buf;
	switch (queue)
	{
	case VkDeferredCommand::Graphics: buf = this->mainCmdDrawBuffer; break;
	case VkDeferredCommand::Transfer: buf = this->mainCmdTransBuffer; break;
	case VkDeferredCommand::Compute: buf = this->mainCmdCmpBuffer; break;
	}
	vkCmdClearColorImage(buf, image, layout, &clearValue, 1, &subres);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushImageDepthStencilClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearDepthStencilValue clearValue, VkImageSubresourceRange subres)
{
	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::ClearDepthStencilImage;
	del.del.imgDepthStencilClear.clearValue = clearValue;
	del.del.imgDepthStencilClear.img = image;
	del.del.imgDepthStencilClear.layout = layout;
	del.del.imgDepthStencilClear.region = subres;
	del.del.queue = queue;
	del.dev = this->dev;
	this->PushCommandPass(del, OnBeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::ImageDepthStencilClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearDepthStencilValue clearValue, VkImageSubresourceRange subres)
{
	VkCommandBuffer buf;
	switch (queue)
	{
	case VkDeferredCommand::Graphics: buf = this->mainCmdDrawBuffer; break;
	case VkDeferredCommand::Transfer: buf = this->mainCmdTransBuffer; break;
	case VkDeferredCommand::Compute: buf = this->mainCmdCmpBuffer; break;
	}
	vkCmdClearDepthStencilImage(buf, image, layout, &clearValue, 1, &subres);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::WaitForFences(VkFence* fences, uint32_t numFences, bool waitAll)
{
	VkResult res = vkWaitForFences(this->dev, numFences, fences, waitAll, UINT_MAX);
	n_assert(res == VK_SUCCESS);
	res = vkResetFences(this->dev, numFences, fences);
	n_assert(res == VK_SUCCESS);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BindSharedDescriptorSets()
{
	VkTransformDevice::Instance()->BindCameraDescriptorSets();
	VkShaderServer::Instance()->BindTextureDescriptorSets();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushCommandPass(const VkDeferredCommand& del, const CommandPass pass)
{
	if (pass == OnHandleDrawFences)				this->putDrawFenceThisFrame = true;
	else if (pass == OnHandleComputeFences)		this->putComputeFenceThisFrame = true;	
	else if (pass == OnHandleTransferFences)	this->putTransferFenceThisFrame = true;
	this->commands[pass].Append(del);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::RunCommandPass(const CommandPass pass)
{
	if (pass == OnHandleDrawFences)
	{
		IndexT i;
		for (i = 0; i < this->drawFenceCommands.Size(); i++)
		{
			const VkFence& fence = this->drawFenceCommands.KeyAtIndex(i);
			VkResult res = vkGetFenceStatus(this->dev, fence);
			if (res == VK_SUCCESS)
			{
				const Util::Array<VkDeferredCommand>& cmds = this->drawFenceCommands.ValueAtIndex(i);
				IndexT j;
				for (j = 0; j < cmds.Size(); j++)
				{
					cmds[j].RunDelegate();
				}
				vkDestroyFence(this->dev, fence, NULL);
				this->drawFenceCommands.EraseAtIndex(i--);
			}
		}
	}
	else if (pass == OnHandleComputeFences)
	{
		IndexT i;
		for (i = 0; i < this->computeFenceCommands.Size(); i++)
		{
			const VkFence& fence = this->computeFenceCommands.KeyAtIndex(i);
			VkResult res = vkGetFenceStatus(this->dev, fence);
			if (res == VK_SUCCESS)
			{
				const Util::Array<VkDeferredCommand>& cmds = this->computeFenceCommands.ValueAtIndex(i);
				IndexT j;
				for (j = 0; j < cmds.Size(); j++)
				{
					cmds[j].RunDelegate();
				}
				vkDestroyFence(this->dev, fence, NULL);
				this->computeFenceCommands.EraseAtIndex(i--);
			}
		}
	}
	else if (pass == OnHandleTransferFences)
	{
		IndexT i;
		for (i = 0; i < this->transferFenceCommands.Size(); i++)
		{
			const VkFence& fence = this->transferFenceCommands.KeyAtIndex(i);
			VkResult res = vkGetFenceStatus(this->dev, fence);
			if (res == VK_SUCCESS)
			{
				const Util::Array<VkDeferredCommand>& cmds = this->transferFenceCommands.ValueAtIndex(i);
				IndexT j;
				for (j = 0; j < cmds.Size(); j++)
				{
					cmds[j].RunDelegate();
				}
				vkDestroyFence(this->dev, fence, NULL);
				this->transferFenceCommands.EraseAtIndex(i--);
			}
		}
	}
	else
	{
		const Util::Array<VkDeferredCommand>& cmds = this->commands[pass];
		IndexT i;
		for (i = 0; i < cmds.Size(); i++)
		{
			cmds[i].RunDelegate();
		}
		this->commands[pass].Clear();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginDrawThread()
{
	n_assert(this->numActiveThreads < NumDrawThreads);

	// allocate command buffer
	VkCommandBufferAllocateInfo info =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->dispatchableCmdDrawBufferPool[this->currentDrawThread],
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		1
	};
	vkAllocateCommandBuffers(this->dev, &info, &this->dispatchableDrawCmdBuffers[this->currentDrawThread]);

	// tell thread to begin command buffer recording
	VkCommandBufferBeginInfo begin =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
		&this->passInfo
	};

	// unpause thread
	this->drawThreads[this->currentDrawThread]->Pause(false);

	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::BeginCommand;
	cmd.bgCmd.buf = this->dispatchableDrawCmdBuffers[this->currentDrawThread];
	cmd.bgCmd.info = begin;
	this->PushToThread(cmd, this->currentDrawThread);
	//this->drawThreads[this->currentDrawThread]->PushCommand(cmd);

	// run begin command buffer pass
	this->RunCommandPass(OnBeginDrawThread);
	this->numActiveThreads++;
}


//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginDrawThreadCluster()
{
	n_assert(this->numActiveThreads == 0);
	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		// unpause thread
		this->drawThreads[i]->Pause(false);

		// allocate command buffer
		VkCommandBufferAllocateInfo info =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			NULL,
			this->dispatchableCmdDrawBufferPool[i],
			VK_COMMAND_BUFFER_LEVEL_SECONDARY,
			1
		};
		vkAllocateCommandBuffers(this->dev, &info, &this->dispatchableDrawCmdBuffers[i]);

		// tell thread to begin command buffer recording
		VkCommandBufferBeginInfo begin =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			NULL,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
			&this->passInfo
		};
		VkCmdBufferThread::Command cmd;
		cmd.type = VkCmdBufferThread::BeginCommand;
		cmd.bgCmd.buf = this->dispatchableDrawCmdBuffers[i];
		cmd.bgCmd.info = begin;

		// begin command buffer in thread
		//this->drawThreads[i]->PushCommand(cmd);
		this->PushToThread(cmd, i);
	}
	this->currentDrawThread = 0;
	this->numActiveThreads = NumDrawThreads;
	this->numUsedThreads = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndDrawThreads()
{
	IndexT i;
	for (i = 0; i < this->numActiveThreads; i++)
	{
		// push remaining cmds to thread
		this->FlushToThread(i);

		// end thread
		VkCmdBufferThread::Command cmd;
		cmd.type = VkCmdBufferThread::EndCommand;
		this->PushToThread(cmd, i, false);
		//this->drawThreads[i]->PushCommand(cmd);

		cmd.type = VkCmdBufferThread::Sync;
		cmd.syncEvent = &this->drawCompletionEvents[i];
		this->PushToThread(cmd, i, false);
		//this->drawThreads[i]->PushCommand(cmd);
		this->drawCompletionEvents[i].Wait();
		this->drawCompletionEvents[i].Reset();

		// stop thread from running
		this->drawThreads[i]->Pause(true);
	}

	// run end-of-threads pass
	this->RunCommandPass(OnDrawThreadsSubmitted);

	// execute commands
	vkCmdExecuteCommands(this->mainCmdDrawBuffer, this->numActiveThreads, this->dispatchableDrawCmdBuffers);

	// destroy command buffers
	for (i = 0; i < this->numActiveThreads; i++)
	{
		VkDeferredCommand cmd;
		cmd.del.type = VkDeferredCommand::FreeCmdBuffers;
		cmd.del.cmdbufferfree.buffers[0] = this->dispatchableDrawCmdBuffers[i];
		cmd.del.cmdbufferfree.numBuffers = 1;
		cmd.del.cmdbufferfree.pool = this->dispatchableCmdDrawBufferPool[i];
		cmd.dev = this->dev;
		this->PushCommandPass(cmd, OnHandleDrawFences);
	}
	this->currentDrawThread = NumDrawThreads-1;
	this->numActiveThreads = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndDrawThreadCluster()
{
	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		VkCmdBufferThread::Command cmd;
		cmd.type = VkCmdBufferThread::EndCommand;
		this->PushToThread(cmd, i);
		//this->drawThreads[i]->PushCommand(cmd);

		cmd.type = VkCmdBufferThread::Sync;
		cmd.syncEvent = &this->drawCompletionEvents[i];
		this->PushToThread(cmd, i);
		//this->drawThreads[i]->PushCommand(cmd);
		//this->drawCompletionEvents[i].Wait();
		//this->drawCompletionEvents[i].Reset();

		// actually push to thread
		//this->drawThreads[i]->PushCommands(this->threadCmds[i]);
		//this->threadCmds[i].Clear();

		// wait for threads to finish, but this is retarded, we can't do any work
		this->drawCompletionEvents[i].Wait();
		this->drawCompletionEvents[i].Reset();
		this->drawThreads[i]->Pause(true);
	}

	// run end-of-threads pass
	this->RunCommandPass(OnDrawThreadsSubmitted);

	// execute commands
	vkCmdExecuteCommands(this->mainCmdDrawBuffer, this->numUsedThreads, this->dispatchableDrawCmdBuffers);

	// destroy command buffers
	for (i = 0; i < NumDrawThreads; i++)
	{
		VkDeferredCommand cmd;
		cmd.del.type = VkDeferredCommand::FreeCmdBuffers;
		cmd.del.cmdbufferfree.buffers[0] = this->dispatchableDrawCmdBuffers[i];
		cmd.del.cmdbufferfree.numBuffers = 1;
		cmd.del.cmdbufferfree.pool = this->dispatchableCmdDrawBufferPool[i];
		cmd.dev = this->dev;
		this->PushCommandPass(cmd, OnHandleDrawFences);
	}
	this->numActiveThreads = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::NextThread()
{
	this->currentDrawThread = (this->currentDrawThread + 1) % NumDrawThreads;
	this->numUsedThreads = Math::n_min(this->numUsedThreads + 1, NumDrawThreads);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushToThread(const VkCmdBufferThread::Command& cmd, const IndexT& index, bool allowStaging)
{
	//this->threadCmds[index].Append(cmd);
	if (allowStaging)
	{
		this->threadCmds[index].Append(cmd);
		if (this->threadCmds[index].Size() == 2500)
		{
			this->drawThreads[index]->PushCommands(this->threadCmds[index]);
			this->threadCmds[index].Clear();
		}
		//this->drawThreads[index]->PushCommand(cmd);
	}
	else
	{
		this->drawThreads[index]->PushCommand(cmd);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::FlushToThread(const IndexT& index)
{
	this->drawThreads[index]->PushCommands(this->threadCmds[index]);
	this->threadCmds[index].Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
VkRenderDevice::AsyncTransferSupported()
{
	return VkRenderDevice::transferQueue != VkRenderDevice::drawQueue;
}

//------------------------------------------------------------------------------
/**
*/
bool
VkRenderDevice::AsyncComputeSupported()
{
	return VkRenderDevice::computeQueue != VkRenderDevice::drawQueue;
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushToInterlockThread(const VkCpuGpuInterlockThread::Command& cmd)
{
	n_assert(this->interlockThread.isvalid());
	this->interlockThread->PushCommand(cmd);
}

//------------------------------------------------------------------------------
/**
	Have the GPU wait for the interlock thread, do this on the 'right' side.
*/
void
VkRenderDevice::InterlockWaitGPU(VkPipelineStageFlags waitStage, VkPipelineStageFlags signalStage, VkBufferMemoryBarrier& buffer)
{
	// wait for GPU event to trigger
	VkCpuGpuInterlockThread::Command cpuCmd;
	cpuCmd.type = VkCpuGpuInterlockThread::WaitEvent;
	cpuCmd.waitEvent.usage = VkCpuGpuInterlockThread::GPUEvent;
	this->PushToInterlockThread(cpuCmd);

	// have GPU signal the event
	VkCmdBufferThread::Command gpuCmd;
	gpuCmd.type = VkCmdBufferThread::SetEvent;
	gpuCmd.setEvent.stages = signalStage;
	gpuCmd.setEvent.event = this->interlockThread->event[VkCpuGpuInterlockThread::GPUEvent];
	this->PushToThread(gpuCmd, this->currentDrawThread);

	// GPU will stall just after signaling event
	gpuCmd.type = VkCmdBufferThread::WaitForEvent;
	gpuCmd.waitEvent.numEvents = 1;
	gpuCmd.waitEvent.events = &this->interlockThread->event[VkCpuGpuInterlockThread::CPUEvent];
	gpuCmd.waitEvent.bufferBarrierCount = 1;
	gpuCmd.waitEvent.bufferBarriers = &buffer;
	gpuCmd.waitEvent.memoryBarrierCount = 0;
	gpuCmd.waitEvent.memoryBarriers = NULL;
	gpuCmd.waitEvent.imageBarrierCount = 0;
	gpuCmd.waitEvent.imageBarriers = NULL;
	gpuCmd.waitEvent.waitingStage = waitStage;
	gpuCmd.waitEvent.signalingStage = signalStage;
	this->PushToThread(gpuCmd, this->currentDrawThread);

	// thread can reset GPU event
	cpuCmd.type = VkCpuGpuInterlockThread::ResetEvent;
	cpuCmd.resetEvent.usage = VkCpuGpuInterlockThread::GPUEvent;
	this->PushToInterlockThread(cpuCmd);

	// GPU is locked now
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::InterlockWaitGPU(VkPipelineStageFlags waitStage, VkPipelineStageFlags signalStage, VkImageMemoryBarrier& image)
{
	// wait for GPU event to trigger
	VkCpuGpuInterlockThread::Command cpuCmd;
	cpuCmd.type = VkCpuGpuInterlockThread::WaitEvent;
	cpuCmd.waitEvent.usage = VkCpuGpuInterlockThread::GPUEvent;
	this->PushToInterlockThread(cpuCmd);

	// have GPU signal the event
	VkCmdBufferThread::Command gpuCmd;
	gpuCmd.type = VkCmdBufferThread::SetEvent;
	gpuCmd.setEvent.stages = signalStage;
	gpuCmd.setEvent.event = this->interlockThread->event[VkCpuGpuInterlockThread::GPUEvent];
	this->PushToThread(gpuCmd, this->currentDrawThread);

	// GPU will stall just after signaling event
	gpuCmd.type = VkCmdBufferThread::WaitForEvent;
	gpuCmd.waitEvent.numEvents = 1;
	gpuCmd.waitEvent.events = &this->interlockThread->event[VkCpuGpuInterlockThread::CPUEvent];
	gpuCmd.waitEvent.bufferBarrierCount = 0;
	gpuCmd.waitEvent.bufferBarriers = NULL;
	gpuCmd.waitEvent.memoryBarrierCount = 0;
	gpuCmd.waitEvent.memoryBarriers = NULL;
	gpuCmd.waitEvent.imageBarrierCount = 1;
	gpuCmd.waitEvent.imageBarriers = &image;
	gpuCmd.waitEvent.waitingStage = waitStage;
	gpuCmd.waitEvent.signalingStage = signalStage;
	this->PushToThread(gpuCmd, this->currentDrawThread);

	// thread can reset GPU event
	cpuCmd.type = VkCpuGpuInterlockThread::ResetEvent;
	cpuCmd.resetEvent.usage = VkCpuGpuInterlockThread::GPUEvent;
	this->PushToInterlockThread(cpuCmd);

	// GPU is locked now
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::InterlockWaitGPU(VkPipelineStageFlags waitStage, VkPipelineStageFlags signalStage, VkMemoryBarrier& barrier)
{
	// wait for GPU event to trigger
	VkCpuGpuInterlockThread::Command cpuCmd;
	cpuCmd.type = VkCpuGpuInterlockThread::WaitEvent;
	cpuCmd.waitEvent.usage = VkCpuGpuInterlockThread::GPUEvent;
	this->PushToInterlockThread(cpuCmd);

	// have GPU signal the event
	VkCmdBufferThread::Command gpuCmd;
	gpuCmd.type = VkCmdBufferThread::SetEvent;
	gpuCmd.setEvent.stages = signalStage;
	gpuCmd.setEvent.event = this->interlockThread->event[VkCpuGpuInterlockThread::GPUEvent];
	this->PushToThread(gpuCmd, this->currentDrawThread);

	// GPU will stall just after signaling event
	gpuCmd.type = VkCmdBufferThread::WaitForEvent;
	gpuCmd.waitEvent.numEvents = 1;
	gpuCmd.waitEvent.events = &this->interlockThread->event[VkCpuGpuInterlockThread::CPUEvent];
	gpuCmd.waitEvent.bufferBarrierCount = 0;
	gpuCmd.waitEvent.bufferBarriers = NULL;
	gpuCmd.waitEvent.memoryBarrierCount = 1;
	gpuCmd.waitEvent.memoryBarriers = &barrier;
	gpuCmd.waitEvent.imageBarrierCount = 0;
	gpuCmd.waitEvent.imageBarriers = NULL;
	gpuCmd.waitEvent.waitingStage = waitStage;
	gpuCmd.waitEvent.signalingStage = signalStage;
	this->PushToThread(gpuCmd, this->currentDrawThread);

	// thread can reset GPU event
	cpuCmd.type = VkCpuGpuInterlockThread::ResetEvent;
	cpuCmd.resetEvent.usage = VkCpuGpuInterlockThread::GPUEvent;
	this->PushToInterlockThread(cpuCmd);

	// GPU is locked now
}

//------------------------------------------------------------------------------
/**
	Have interlock thread wait for GPU, do this on the 'left' side of a synchronized operation
*/
void
VkRenderDevice::InterlockWaitCPU(VkPipelineStageFlags stage)
{
	// release GPU by signaling event
	VkCpuGpuInterlockThread::Command cpuCmd;
	cpuCmd.type = VkCpuGpuInterlockThread::SignalEvent;
	cpuCmd.waitEvent.usage = VkCpuGpuInterlockThread::CPUEvent;
	this->PushToInterlockThread(cpuCmd);

	// have GPU reset CPU event
	VkCmdBufferThread::Command gpuCmd;
	gpuCmd.type = VkCmdBufferThread::ResetEvent;
	gpuCmd.resetEvent.event = this->interlockThread->event[VkCpuGpuInterlockThread::CPUEvent];
	gpuCmd.resetEvent.stages = stage;
	this->PushToThread(gpuCmd, this->currentDrawThread);

	// GPU is now unlocked
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::InterlockMemcpy(uint32_t size, uint32_t offset, const void* data, void* mappedData)
{
	VkCpuGpuInterlockThread::Command cmd;
	cmd.type = VkCpuGpuInterlockThread::Memcpy;
	cmd.memCpy.data = data;
	cmd.memCpy.size = size;
	cmd.memCpy.offset = offset;
	cmd.memCpy.mappedData = mappedData;
	//this->PushToInterlockThread(cmd);
}

} // namespace Vulkan
