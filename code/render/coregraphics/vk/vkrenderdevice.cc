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
VkQueue VkRenderDevice::renderQueue;
VkQueue VkRenderDevice::computeQueue;
VkQueue VkRenderDevice::transferQueue;
VkInstance VkRenderDevice::instance;
VkPhysicalDevice VkRenderDevice::physicalDev;
VkPipelineCache VkRenderDevice::cache;

VkCommandPool VkRenderDevice::mainCmdCmpPool;
VkCommandPool VkRenderDevice::mainCmdTransPool;
VkCommandPool VkRenderDevice::mainCmdGfxPool;
VkCommandBuffer VkRenderDevice::mainCmdGfxBuffer;
VkCommandBuffer VkRenderDevice::mainCmdCmpBuffer;
VkCommandBuffer VkRenderDevice::mainCmdTransBuffer;

//------------------------------------------------------------------------------
/**
*/
VkRenderDevice::VkRenderDevice() :
	frameId(0),
	renderQueueIdx(-1),
	computeQueueIdx(-1),
	transferQueueIdx(-1),
	currentDrawThread(0),
	currentTransThread(0),
	currentProgram(0),
	delegateBucketIndex(0),
	fenceDelegateBuckets(NumDeferredDelegates)
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
	
	const char* layers[] = { "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_device_limits", "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_image", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_GOOGLE_unique_objects" };
#if NEBULAT_VULKAN_DEBUG
	this->extensions[this->usedExtensions++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
	const int numLayers = sizeof(layers) / sizeof(const char*);
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
			this->renderQueueFamily = i;
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
			this->computeQueueFamily = i;
			if (this->queuesProps[i].queueCount == indexMap[i]) continue;
			computeIdx = i;
			this->computeQueueIdx = indexMap[i];
			indexMap[i]++;
		}
		if (this->queuesProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT && this->transferQueueIdx == UINT32_MAX)
		{
			this->transferQueueFamily = i;
			if (this->queuesProps[i].queueCount == indexMap[i]) continue;
			transferIdx = i;
			this->transferQueueIdx = indexMap[i];
			indexMap[i]++;
		}
	}

	if (this->renderQueueIdx == UINT32_MAX || gfxIdx == UINT32_MAX) n_error("VkDisplayDevice: Could not find a queue that supported screen present and graphics.\n");

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
	vkGetDeviceQueue(VkRenderDevice::dev, gfxIdx, this->renderQueueIdx, &VkRenderDevice::renderQueue);
	vkGetDeviceQueue(VkRenderDevice::dev, computeIdx, this->computeQueueIdx, &VkRenderDevice::computeQueue);
	vkGetDeviceQueue(VkRenderDevice::dev, transferIdx, this->transferQueueIdx, &VkRenderDevice::transferQueue);

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
		this->renderQueueIdx,
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
			{ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
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
		this->renderQueueFamily
	};

	res = vkCreateCommandPool(this->dev, &cmdPoolInfo, NULL, &this->mainCmdGfxPool);
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
			this->renderQueueFamily
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
		this->mainCmdGfxPool,
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

		this->drawCompletionEvent[i] = Threading::Event(true);
	}

	for (i = 0; i < NumTransferThreads; i++)
	{
		threadName.Format("TransferCmdBufferThread%d", i);
		this->transThreads[i] = VkCmdBufferThread::Create();
		this->transThreads[i]->SetPriority(Threading::Thread::Normal);
		this->transThreads[i]->SetCoreId(System::Cpu::RenderThreadFirstCore + NumDrawThreads + i);
		this->transThreads[i]->SetName(threadName);
		this->transThreads[i]->Start();

		this->transCompletionEvent[i] = Threading::Event(true);
	}

	VkFenceCreateInfo fenceInfo =
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		NULL,
		0
	};
	this->currentDeferredDelegate = 0;
	this->freeFences.Reserve(NumDeferredDelegates);
	this->usedFences.Reserve(NumDeferredDelegates);
	for (i = 0; i < NumDeferredDelegates; i++)
	{
		res = vkCreateFence(this->dev, &fenceInfo, NULL, &this->deferredDelegateFences[i]);
		n_assert(res == VK_SUCCESS);
		this->freeFences.Enqueue(i);
	}

	res = vkCreateFence(this->dev, &fenceInfo, NULL, &this->mainCmdGfxFence);
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
	VkResult res = vkQueueWaitIdle(this->renderQueue);
	n_assert(res == VK_SUCCESS);

	// free our main buffers, our secondary buffers should be fine so the pools should be free to destroy
	vkFreeCommandBuffers(this->dev, this->mainCmdGfxPool, 1, &this->mainCmdGfxBuffer);
	vkFreeCommandBuffers(this->dev, this->mainCmdCmpPool, 1, &this->mainCmdCmpBuffer);
	vkFreeCommandBuffers(this->dev, this->mainCmdTransPool, 1, &this->mainCmdTransBuffer);
	vkDestroyCommandPool(this->dev, this->mainCmdGfxPool, NULL);
	vkDestroyCommandPool(this->dev, this->mainCmdGfxPool, NULL);
	vkDestroyCommandPool(this->dev, this->mainCmdCmpPool, NULL);
	vkDestroyCommandPool(this->dev, this->immediateCmdTransPool, NULL);
	vkDestroyFence(this->dev, this->mainCmdGfxFence, NULL);
	vkDestroyFence(this->dev, this->mainCmdCmpFence, NULL);
	vkDestroyFence(this->dev, this->mainCmdTransFence, NULL);

	for (i = 0; i < (int32_t)this->numBackbuffers; i++)
	{
		vkDestroyImage(this->dev, this->backbuffers[i], NULL);
		vkFreeMemory(this->dev, this->backbufferMem[i], NULL);
	}

	for (i = 0; i < NumDeferredDelegates; i++)
	{
		vkDestroyFence(this->dev, this->deferredDelegateFences[i], NULL);
	}

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

	VkResult res = vkAcquireNextImageKHR(this->dev, this->swapchain, UINT64_MAX, this->displaySemaphore, VK_NULL_HANDLE, &this->currentBackbuffer);
	if (res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// this means our swapchain needs a resize!
	}
	else
	{
		n_assert(res == VK_SUCCESS);
	}

	// if we have no free delegates for this frame, wait for the other to finish up
	while (this->freeFences.IsEmpty()) this->UpdateDelegates();
	IndexT freeIndex = this->freeFences.Dequeue();

	// runs through delegate list and updates them one by one
	this->UpdateDelegates();

	// enqueue this frames sync
	this->usedFences.Enqueue(freeIndex);
	this->delegateBucketIndex = freeIndex;
	this->fenceDelegateBuckets[freeIndex] = this->nextFrameFenceDelegates;
	this->nextFrameFenceDelegates.Clear();	

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
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
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
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
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
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::RenderTarget>& rt, const Ptr<CoreGraphics::Shader>& passShader)
{
	RenderDeviceBase::BeginPass(rt, passShader);

	const Ptr<DepthStencilTarget>& dst = rt->GetDepthStencilTarget();
	if (dst.isvalid()) dst->BeginPass();

	// remember to swap, kids!
	if (rt->IsDefaultRenderTarget())
	{
		rt->SwapBuffers();
	}
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
	vkCmdBeginRenderPass(this->mainCmdGfxBuffer, &info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	this->currentPipelineBits = 0;
	this->SetFramebufferLayoutInfo(rt->GetVkPipelineInfo());

	this->blendInfo.attachmentCount = 1;

	this->passInfo.framebuffer = rt->GetVkFramebuffer();
	this->passInfo.renderPass = rt->GetVkRenderPass();
	this->passInfo.subpass = 0;
	this->passInfo.pipelineStatistics = 0;
	this->passInfo.queryFlags = 0;
	this->passInfo.occlusionQueryEnable = VK_FALSE;

	const Util::FixedArray<VkRect2D>& scissors = rt->GetVkScissorRects();
	const Util::FixedArray<VkViewport>& viewports = rt->GetVkViewports();

	// start constructing draws
	this->BeginCmdThreads();

	// hmm, because we have dynamic scissor rects, we need to 'init' all buffers with the default scissors first
	VkCmdBufferThread::Command scissorCmd;
	scissorCmd.type = VkCmdBufferThread::ScissorRectArray;
	scissorCmd.scissorRectArray.first = 0;
	scissorCmd.scissorRectArray.num = scissors.Size();
	scissorCmd.scissorRectArray.scs = scissors.Begin();

	for (IndexT i = 0; i < NumDrawThreads; i++) this->drawThreads[i]->PushCommand(scissorCmd);

	VkCmdBufferThread::Command viewportCmd;
	viewportCmd.type = VkCmdBufferThread::Viewport;
	viewportCmd.viewport.index = 0;
	viewportCmd.viewport.vp = viewports[0];

	for (IndexT i = 0; i < NumDrawThreads; i++) this->drawThreads[i]->PushCommand(viewportCmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::MultipleRenderTarget>& mrt, const Ptr<CoreGraphics::Shader>& passShader)
{
	RenderDeviceBase::BeginPass(mrt, passShader);

	const Ptr<DepthStencilTarget>& dst = mrt->GetDepthStencilTarget();
	if (dst.isvalid()) dst->BeginPass();

	VkRect2D rect;
	rect.offset.x = 0;
	rect.offset.y = 0;
	rect.extent.width = mrt->GetRenderTarget(0)->GetWidth();
	rect.extent.height = mrt->GetRenderTarget(0)->GetHeight();

	const Util::FixedArray<VkClearValue>& clear = mrt->GetVkClearValues();

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
	vkCmdBeginRenderPass(this->mainCmdGfxBuffer, &info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	this->currentPipelineBits = 0;
	this->SetFramebufferLayoutInfo(mrt->GetVkPipelineInfo());

	this->blendInfo.attachmentCount = mrt->GetNumRendertargets();

	this->passInfo.framebuffer = mrt->GetVkFramebuffer();
	this->passInfo.renderPass = mrt->GetVkRenderPass();
	this->passInfo.subpass = 0;
	this->passInfo.pipelineStatistics = 0;
	this->passInfo.queryFlags = 0;
	this->passInfo.occlusionQueryEnable = VK_FALSE;

	const Util::FixedArray<VkRect2D>& scissors = mrt->GetVkScissorRects();
	const Util::FixedArray<VkViewport>& viewports = mrt->GetVkViewports();

	// start constructing draws
	this->BeginCmdThreads();

	// hmm, because we have dynamic scissor rects, we need to 'init' all buffers with the default scissors first
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::ScissorRectArray;
	cmd.scissorRectArray.first = 0;
	cmd.scissorRectArray.num = scissors.Size();
	cmd.scissorRectArray.scs = scissors.Begin();

	for (IndexT i = 0; i < NumDrawThreads; i++) this->drawThreads[i]->PushCommand(cmd);

	VkCmdBufferThread::Command viewportCmd;
	viewportCmd.type = VkCmdBufferThread::Viewport;
	viewportCmd.viewport.index = 0;
	viewportCmd.viewport.vp = viewports[0];

	for (IndexT i = 0; i < NumDrawThreads; i++) this->drawThreads[i]->PushCommand(viewportCmd);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginPass(const Ptr<CoreGraphics::RenderTargetCube>& rtc, const Ptr<CoreGraphics::Shader>& passShader)
{
	RenderDeviceBase::BeginPass(rtc, passShader);
	this->currentPipelineBits = 0;
	this->SetFramebufferLayoutInfo(rtc->GetVkPipelineInfo());

	// submit
	this->SubmitToQueue(this->renderQueue, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 1, &this->mainCmdGfxBuffer);

	this->blendInfo.attachmentCount = 1;

	VkResult res = vkResetCommandBuffer(this->mainCmdGfxBuffer, 0);
	n_assert(res == VK_SUCCESS);

	// start constructing draws
	this->BeginCmdThreads();
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

	// finish command buffer threads
	this->EndCmdThreads();

	// execute subpass buffers
	this->EndSubpassCommands();

	// end render pass
	vkCmdEndRenderPass(this->mainCmdGfxBuffer);

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

	// submit transfer stuff
	this->SubmitToQueue(this->transferQueue, VK_PIPELINE_STAGE_TRANSFER_BIT, 1, &this->mainCmdTransBuffer);
	this->SubmitToQueue(this->transferQueue, this->deferredDelegateFences[this->delegateBucketIndex]);
	//this->SubmitToQueue(this->transferQueue, this->mainCmdTransFence);
	this->WaitForFence(this->mainCmdTransFence);
	VkResult res = vkResetCommandBuffer(this->mainCmdTransBuffer, 0);
	n_assert(res == VK_SUCCESS);

	// submit main render buffer to queue
	this->SubmitToQueue(this->renderQueue, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 1, &this->mainCmdGfxBuffer);
	this->SubmitToQueue(this->renderQueue, this->mainCmdGfxFence);
	this->WaitForFence(this->mainCmdGfxFence);
	res = vkResetCommandBuffer(this->mainCmdGfxBuffer, 0);
	n_assert(res == VK_SUCCESS);

	this->SubmitToQueue(this->computeQueue, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 1, &this->mainCmdCmpBuffer);
	this->SubmitToQueue(this->computeQueue, this->mainCmdCmpFence);
	this->WaitForFence(this->mainCmdCmpFence);
	res = vkResetCommandBuffer(this->mainCmdCmpBuffer, 0);
	n_assert(res == VK_SUCCESS);

	this->delegateBucketIndex++;

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
	VkResult res = vkQueueSubmit(this->renderQueue, 1, &submitInfo, NULL);
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
	res = vkQueuePresentKHR(this->renderQueue, &info);

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
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
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
	this->drawThreads[this->currentDrawThread]->PushCommand(cmd);
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
	This is most likely to cause pipeline creations, because the stack is most usually
	Framebuffer
		Shader
			Vertex layout
*/
void
VkRenderDevice::SetVertexLayoutPipelineInfo(const VkPipelineVertexInputStateCreateInfo& vertexLayout)
{
#define uint_min(a, b) (a < b ? a : b)

	this->currentPipelineBits |= VertexLayoutInfoSet;
	this->vertexInfo = vertexLayout;

	this->currentPipelineBits &= ~InputLayoutInfoSet;
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
VkRenderDevice::CreatePipeline()
{
	VkPipeline pipeline;
	VkResult res = vkCreateGraphicsPipelines(this->dev, this->cache, 1, &this->currentPipelineInfo, NULL, &pipeline);
	n_assert(res == VK_SUCCESS);

	// send pipeline bind command, this is the first step in our procedure, so we use this as a trigger to switch threads
	this->currentDrawThread = (this->currentDrawThread + 1) % NumDrawThreads;
	VkCmdBufferThread::Command cmd;
	cmd.type = VkCmdBufferThread::GraphicsPipeline;
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
VkRenderDevice::BufferUpdate(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, uint32_t* data)
{
#define VK_SUBMIT_MAX_SIZE 65536
#define VK_MIN(a, b) a < b ? a : b;
	// data size must be a multiple of 65536 bytes, so let's do some chunk based updating
	VkDeviceSize submitSize = size;
	VkDeviceSize submitOffset = offset;
	while (submitSize > 0)
	{
		VkDeviceSize numBytesToSubmit = VK_MIN(VK_SUBMIT_MAX_SIZE, submitSize);

		// if we are going to handle the deletes ourself, then we must copy the data
		// push to main buffer
		vkCmdUpdateBuffer(this->mainCmdTransBuffer, buf, submitOffset, numBytesToSubmit, data);
		submitOffset += numBytesToSubmit;
		submitSize = VK_SUBMIT_MAX_SIZE > submitSize ? 0 : submitSize - VK_SUBMIT_MAX_SIZE;
	}
	

	// push delegate which will free the memory when we are done
	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::FreeMemory;
	del.del.memory.data = data;
	del.dev = this->dev;
	del.del.queue = VkDeferredCommand::Transfer;
	this->PushCommand(del);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::ImageUpdate(const VkImage& img, VkBufferImageCopy copy, VkDeviceSize size, uint32_t* data)
{
	// create transfer buffer
	VkBufferCreateInfo info =
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
	vkCreateBuffer(this->dev, &info, NULL, &buf);

	// allocate memory
	VkDeviceMemory bufMem;
	uint32_t bufsize;
	this->AllocateBufferMemory(buf, bufMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, bufsize);
	vkBindBufferMemory(this->dev, buf, bufMem, 0);

	// perform update of buffer, and stage a copy of buffer data to image
	this->BufferUpdate(buf, 0, size, data);
	vkCmdCopyBufferToImage(this->mainCmdTransBuffer, buf, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

	// finally push delegates to dealloc all our staging data
	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::FreeBuffer;
	del.del.buffer.buf = buf;
	del.del.buffer.mem = bufMem;
	del.del.queue = VkDeferredCommand::Transfer;
	del.dev = this->dev;
	this->PushCommand(del);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushBufferUpdate(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, uint32_t* data)
{
	uint32_t* bufCopy = (uint32_t*)n_new_array(uint32_t, VK_DEVICE_SIZE_CONV(size));
	Memory::Copy(data, bufCopy, VK_DEVICE_SIZE_CONV(size));

	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::UpdateBuffer;
	del.del.bufferUpd.buf = buf;
	del.del.bufferUpd.size = size;
	del.del.bufferUpd.offset = offset;
	del.del.bufferUpd.data = bufCopy;
	del.del.queue = VkDeferredCommand::Transfer;
	del.dev = this->dev;
	this->PushCommandStaging(del);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushImageUpdate(const VkImage& img, VkBufferImageCopy copy, VkDeviceSize size, uint32_t* data)
{
	uint32_t* imgCopy = (uint32_t*)n_new_array(uint32_t, VK_DEVICE_SIZE_CONV(size));
	Memory::Copy(data, imgCopy, VK_DEVICE_SIZE_CONV(size));

	VkDeferredCommand del;
	del.del.type = VkDeferredCommand::UpdateImage;
	del.del.imageUpd.img = img;
	del.del.imageUpd.copy = copy;
	del.del.imageUpd.size = size;
	del.del.imageUpd.data = imgCopy;
	del.del.queue = VkDeferredCommand::Transfer;
	del.dev = this->dev;
	this->PushCommandStaging(del);
}

//------------------------------------------------------------------------------
/**
	Begins an immediate command buffer for data transfers, and returns the buffer within which the image data is contained.
*/
void
VkRenderDevice::ReadImage(const VkImage& img, VkBufferImageCopy copy, uint32_t& outMemSize, VkDeviceMemory& outMem, VkBuffer& outBuffer)
{
	VkCommandBuffer cmdBuf = this->BeginImmediateTransfer();

	VkImageSubresource subres;
	subres.arrayLayer = copy.imageSubresource.baseArrayLayer;
	subres.aspectMask = copy.imageSubresource.aspectMask;
	subres.mipLevel = copy.imageSubresource.mipLevel;
	VkSubresourceLayout layout;
	vkGetImageSubresourceLayout(this->dev, img, &subres, &layout);

	// create transfer buffer
	VkBufferCreateInfo info =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		NULL,
		0,
		layout.size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		1,
		&this->transferQueueIdx
	};
	VkBuffer buf;
	vkCreateBuffer(this->dev, &info, NULL, &buf);

	// allocate memory
	VkDeviceMemory bufMem;
	uint32_t bufsize;
	this->AllocateBufferMemory(buf, bufMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, bufsize);
	vkBindBufferMemory(this->dev, buf, bufMem, 0);

	// perform update of buffer, and stage a copy of buffer data to image
	vkCmdCopyImageToBuffer(this->mainCmdCmpBuffer, img, VK_IMAGE_LAYOUT_GENERAL, buf, 1, &copy);

	// end immediate command buffer
	this->EndImmediateTransfer(cmdBuf);

	outBuffer = buf;
	outMem = bufMem;
	outMemSize = VK_DEVICE_SIZE_CONV(layout.size);
}

//------------------------------------------------------------------------------
/**
	
*/
void
VkRenderDevice::WriteImage(const VkBuffer& buf, const VkImage& img, VkBufferImageCopy copy)
{
	VkCommandBuffer cmdBuf = this->BeginImmediateTransfer();
	vkCmdCopyBufferToImage(cmdBuf, buf, img, VK_IMAGE_LAYOUT_GENERAL, 1, &copy);
	this->EndImmediateTransfer(cmdBuf);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushCommand(const VkDeferredCommand& del)
{
	if (del.del.type < VkDeferredCommand::__RunAfterFence)	this->fenceDelegateBuckets[this->delegateBucketIndex].Append(del);
	else													this->nextFrameDelegates.Append(del);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::PushCommandStaging(const VkDeferredCommand& del)
{
	if (del.del.type < VkDeferredCommand::__RunAfterFence)	this->nextFrameFenceDelegates.Append(del);
	else													this->nextFrameDelegates.Append(del);
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
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
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
void
VkRenderDevice::EndSubpassCommands()
{
	// execute on main buffer
	vkCmdExecuteCommands(this->mainCmdGfxBuffer, NumDrawThreads, this->dispatchableDrawCmdBuffers);

	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		// use a delegate to free up the command buffers used by the thread this batch at some later point
		VkDeferredCommand del;
		del.del.type = VkDeferredCommand::FreeCmdBuffers;
		del.del.cmdbufferfree.numBuffers = 1;
		del.del.cmdbufferfree.pool = this->dispatchableCmdDrawBufferPool[i];
		del.del.cmdbufferfree.buffers[0] = this->dispatchableDrawCmdBuffers[i];
		del.del.queue = VkDeferredCommand::Graphics;
		del.dev = this->dev;
		this->PushCommand(del);
	}
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
VkRenderDevice::UpdateDelegates()
{
	IndexT i;
	for (i = 0; i < this->usedFences.Size(); i++)
	{
		VkFence fence = this->deferredDelegateFences[this->usedFences[i]];
		VkResult res = vkGetFenceStatus(this->dev, fence);
		if (res == VK_SUCCESS)
		{
			const Util::Array<VkDeferredCommand>& delegates = this->fenceDelegateBuckets[i];

			IndexT j;
			for (j = 0; j < delegates.Size(); j++)
			{
				delegates[j].RunDelegate();
			}
			vkResetFences(this->dev, 1, &fence);
		}
		
		this->fenceDelegateBuckets[i].Clear();
		this->freeFences.Enqueue(this->usedFences.Dequeue());
		i--;
	}

	for (i = 0; i < this->nextFrameDelegates.Size(); i++)
	{
		this->nextFrameDelegates[i].RunDelegate();
	}
	this->nextFrameDelegates.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::BeginCmdThreads()
{
	// setup begin using pass info
	const VkCommandBufferBeginInfo beginInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		&this->passInfo
	};

	for (IndexT i = 0; i < NumDrawThreads; i++)
	{
		const VkCommandBufferAllocateInfo info =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			NULL,
			this->dispatchableCmdDrawBufferPool[i],
			VK_COMMAND_BUFFER_LEVEL_SECONDARY,
			1
		};
		VkResult res = vkAllocateCommandBuffers(this->dev, &info, &this->dispatchableDrawCmdBuffers[i]);
		n_assert(res == VK_SUCCESS);

		//res = vkBeginCommandBuffer(this->dispatchableDrawCmdBuffers[i], &beginInfo);
		//n_assert(res == VK_SUCCESS);
		VkCmdBufferThread::Command beginCommand;
		beginCommand.type = VkCmdBufferThread::BeginCommand;
		beginCommand.bgCmd.info = beginInfo;
		beginCommand.bgCmd.buf = this->dispatchableDrawCmdBuffers[i];
		this->drawThreads[i]->PushCommand(beginCommand);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::EndCmdThreads()
{
	// wait for our jobs to finish
	IndexT i;
	for (i = 0; i < NumDrawThreads; i++)
	{
		VkCmdBufferThread::Command endCmd;
		endCmd.type = VkCmdBufferThread::EndCommand;
		this->drawThreads[i]->PushCommand(endCmd);

		VkCmdBufferThread::Command cmd;
		cmd.type = VkCmdBufferThread::Sync;
		cmd.syncEvent = &this->drawCompletionEvent[i];
		this->drawThreads[i]->PushCommand(cmd);
		this->drawCompletionEvent[i].Wait();
		this->drawCompletionEvent[i].Reset();

		//VkResult res = vkEndCommandBuffer(this->dispatchableDrawCmdBuffers[i]);
		//n_assert(res == VK_SUCCESS);
	}
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
		this->CreatePipeline();
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
	del.del.type = VkDeferredCommand::ChangeImageLayout;
	del.del.imgBarrier.barrier = barrier;
	del.del.queue = queue;
	del.dev = this->dev;
	this->PushCommandStaging(del);	
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
	case VkDeferredCommand::Graphics: buf = this->mainCmdGfxBuffer; flags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT; break;
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
	this->PushCommandStaging(del);
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
	case VkDeferredCommand::Graphics: buf = this->mainCmdGfxBuffer; break;
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
	this->PushCommandStaging(del);
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
	case VkDeferredCommand::Graphics: buf = this->mainCmdGfxBuffer; break;
	case VkDeferredCommand::Transfer: buf = this->mainCmdTransBuffer; break;
	case VkDeferredCommand::Compute: buf = this->mainCmdCmpBuffer; break;
	}
	vkCmdClearDepthStencilImage(buf, image, layout, &clearValue, 1, &subres);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::WaitForFence(VkFence fence)
{
	VkResult res = vkWaitForFences(this->dev, 1, &fence, true, UINT_MAX);
	n_assert(res == VK_SUCCESS);
	res = vkResetFences(this->dev, 1, &fence);
	n_assert(res == VK_SUCCESS);
}

} // namespace Vulkan
