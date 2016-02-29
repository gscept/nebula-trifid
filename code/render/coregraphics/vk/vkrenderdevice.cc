//------------------------------------------------------------------------------
// vkrenderdevice.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
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
VkQueue VkRenderDevice::displayQueue;
VkInstance VkRenderDevice::instance;
VkPhysicalDevice VkRenderDevice::physicalDev;
VkPipelineCache VkRenderDevice::cache;
VkDescriptorPool VkRenderDevice::descPool;

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
	currentThread(0)
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
	
	const char* layers[] = { "VK_LAYER_LUNARG_mem_tracker" };
#if NEBULAT_VULKAN_DEBUG
	this->extensions[this->usedExtensions++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
	const int numLayers = 1;
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
#endif

	// get number of queues
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDev, &this->numQueues, NULL);
	n_assert(this->numQueues > 0);

	// now get queues from device
	this->queuesProps = n_new_array(VkQueueFamilyProperties, this->numQueues);
	vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDev, &this->numQueues, this->queuesProps);
	vkGetPhysicalDeviceMemoryProperties(this->physicalDev, &this->memoryProps);

	// setup swap chain in display
	VkDisplayDevice::Instance()->SetupSwapchain();

	this->surfaceSupport = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(this->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	this->surfaceFormats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(this->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	n_assert(surfaceSupport != NULL);
	n_assert(surfaceFormats != NULL);
	VkBool32* canPresent = n_new_array(VkBool32, numQueues);
	for (i = 0; i < numQueues; i++)
	{
		this->surfaceSupport(VkRenderDevice::Instance()->physicalDev, i, VkDisplayDevice::Instance()->surface, &canPresent[i]);
	}

	uint32_t gfxIdx = UINT32_MAX;
	uint32_t queueIdx = UINT32_MAX;

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
					gfxIdx = i;
					queueIdx = j;
					goto esc;
				}
			}
		}

		// also setup compute and transfer queues
		if (this->queuesProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			this->computeQueueIdx = i;
		}
		if (this->queuesProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			this->transferQueueIdx = i;
		}
	}

esc:
	if (queueIdx == UINT32_MAX || gfxIdx == UINT32_MAX) n_error("VkDisplayDevice: Could not find a queue that supported screen present and graphics.\n");

	// delete array of present flags
	n_delete_array(canPresent);

	// create device
	float prios[] = {0.0f};
	VkDeviceQueueCreateInfo queueInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		NULL,
		0,
		gfxIdx,
		1,
		prios
	};

	// get physical device features
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(this->physicalDev, &features);

	VkDeviceCreateInfo deviceInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		NULL,
		0,
		1,
		&queueInfo,
		numLayers,
		layers,
		this->usedPhysicalExtensions,
		this->physicalExtensions,
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
	n_assert(this->swapChainCtor != NULL);
	n_assert(this->swapChainDtor != NULL);
	n_assert(this->swapChainNextImage != NULL);
	n_assert(this->swapChainPresent != NULL);

	// setup display queue in render device
	vkGetDeviceQueue(VkRenderDevice::dev, gfxIdx, queueIdx, &VkRenderDevice::displayQueue);

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

	VkPipelineCacheCreateInfo cacheInfo =
	{
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		NULL,
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
	VkCommandBufferAllocateInfo cmdAllocInfo =
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
	VkCommandBufferAllocateInfo cmdAllocInfo =
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
	IndexT i;
	for (i = 0; i < NumThreads; i++)
	{
		threadName.Format("RenderThread%d", i);
		this->threads[i] = VkCmdBufferThread::Create();
		this->threads[i]->SetPriority(Threading::Thread::High);
		this->threads[i]->SetCoreId(System::Cpu::RenderThreadFirstCore + i);
		this->threads[i]->SetName(threadName);
		this->threads[i]->Start();
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
		VkPhysicalDevice* devices = new VkPhysicalDevice[gpuCount];
		res = vkEnumeratePhysicalDevices(this->instance, &gpuCount, devices);
		n_assert(res == VK_SUCCESS);

		// hmm, this is ugly, perhaps implement a way to get a proper device
		physicalDev = devices[0];
		delete[] devices;
	}
	else
	{
		n_error("VkRenderDevice::SetupAdapter(): No GPU available.\n");
	}

	uint32_t numExtensions;
	res = vkEnumerateDeviceExtensionProperties(this->physicalDev, NULL, &numExtensions, NULL);
	n_assert(res == VK_SUCCESS);

	if (numExtensions > 0)
	{
		VkExtensionProperties* exts = n_new_array(VkExtensionProperties, numExtensions);
		res = vkEnumerateDeviceExtensionProperties(this->physicalDev, NULL, &numExtensions, exts);

		this->usedPhysicalExtensions = 0;
		uint32_t i;
		for (i = 0; i < numExtensions; i++)
		{
			this->physicalExtensions[this->usedPhysicalExtensions++] = exts[i].extensionName;
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
	VkCommandBufferBeginInfo info =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		VK_NULL_HANDLE
	};
	vkBeginCommandBuffer(this->mainCmdGfxBuffer, &info);
	vkBeginCommandBuffer(this->mainCmdCmpBuffer, &info);
	vkBeginCommandBuffer(this->mainCmdTransBuffer, &info);

	// reset current thread
	this->currentThread = 0;
	return RenderDeviceBase::BeginFrame(frameIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderDevice::SetStreamSource(IndexT streamIndex, const Ptr<CoreGraphics::VertexBuffer>& vb, IndexT offsetVertexIndex)
{
	RenderDeviceBase::SetStreamSource(streamIndex, vb, offsetVertexIndex);
	VkCmdBufferThread::Command cmd;
	cmd.vbo.buffer = vb->GetVkBuffer();
	cmd.vbo.index = streamIndex;
	cmd.vbo.offset = offsetVertexIndex;
	this->threads[this->currentThread]->PushCommand(cmd);
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
	this->threads[this->currentThread]->PushCommand(cmd);
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
	VkCommandBufferAllocateInfo info =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		this->cmdGfxPool[Transient],
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		NumThreads
	};
	VkResult res = vkAllocateCommandBuffers(this->dev, &info, this->dispatchableCmdBuffers);
	n_assert(res == VK_SUCCESS);

	// begin buffers
	VkCommandBufferBeginInfo beginInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		VK_NULL_HANDLE
	};

	for (IndexT i = 0; i < NumThreads; i++)
	{
		vkBeginCommandBuffer(this->dispatchableCmdBuffers[i], &beginInfo);
		this->threads[i]->SetCommandBuffer(this->dispatchableCmdBuffers[i]);
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
	this->threads[this->currentThread]->PushCommand(cmd);
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
	this->threads[this->currentThread]->PushCommand(cmd);
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
	for (i = 0; i < NumThreads; i++)
	{ 
		this->threads[i]->Stop();
	}
	
	// stop recording command buffers
	for (IndexT i = 0; i < NumThreads; i++)
	{
		vkEndCommandBuffer(this->dispatchableCmdBuffers[i]);
	}

	VkPipelineStageFlags flags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	VkSubmitInfo info =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		NULL,
		0,
		NULL,
		&flags,
		NumThreads,
		this->dispatchableCmdBuffers,
		0,
		NULL
	};

	// submit to queue
	VkResult res = vkQueueSubmit(this->displayQueue, 1, &info, VK_NULL_HANDLE);
	n_assert(res == VK_SUCCESS);

	// free up our command buffers
	vkFreeCommandBuffers(this->dev, this->cmdGfxPool[Transient], NumThreads, this->dispatchableCmdBuffers);
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
	const VkPresentInfoKHR info =
	{
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		NULL,
		0,
		NULL,

	};
	this->swapChainPresent(this->displayQueue, &info);
	this->swapChainNextImage(this->dev, )
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
	vp.width = rect.width();
	vp.height = rect.height();
	vp.x = rect.left;
	vp.y = rect.top;
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
	for (uint32_t i = 0; i < 32; i++)
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
	VkResult err = VkRenderDevice::Instance()->GetMemoryType(req.memoryTypeBits, flags, memtype);
	n_assert(err == VK_SUCCESS);
	VkMemoryAllocateInfo meminfo =
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		NULL,
		req.size,
		memtype
	};

	// now allocate memory
	VkDeviceMemory mem;
	err = vkAllocateMemory(VkRenderDevice::dev, &meminfo, NULL, &mem);
	if (err == VK_ERROR_OUT_OF_DEVICE_MEMORY || err == VK_ERROR_OUT_OF_HOST_MEMORY)
	{
		n_error("VkRenderDevice::AllocateBufferMemory(): Could not allocate '%d' bytes, out of memory\n.", req.size);
	}
	n_assert(err == VK_SUCCESS);
	bufmem = mem;
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
	vkGetImageMemoryRequirements(VkRenderDevice::dev, imgmem, &req);

	uint32_t memtype;
	VkResult err = VkRenderDevice::Instance()->GetMemoryType(req.memoryTypeBits, flags, memtype);
	n_assert(err == VK_SUCCESS);
	VkMemoryAllocateInfo meminfo =
	{
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		NULL,
		req.size,
		memtype
	};

	// now allocate memory
	VkDeviceMemory mem;
	err = vkAllocateMemory(VkRenderDevice::dev, &meminfo, NULL, &mem);
	if (err == VK_ERROR_OUT_OF_DEVICE_MEMORY || err == VK_ERROR_OUT_OF_HOST_MEMORY)
	{
		n_error("VkRenderDevice::AllocateImageMemory(): Could not allocate '%d' bytes, out of memory\n.", req.size);
	}
	n_assert(err == VK_SUCCESS);
	imgmem = mem;
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
	this->currentThread = (this->currentThread + 1) % NumThreads;
	VkCmdBufferThread::Command cmd;
	cmd.pipeline = pipeline;
	this->threads[this->currentThread]->PushCommand(cmd);
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
	this->threads[this->currentThread]->PushCommand(cmd);
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
	this->threads[this->currentThread]->PushCommand(cmd);
}

} // namespace Vulkan