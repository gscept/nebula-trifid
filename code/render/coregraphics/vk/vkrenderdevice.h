#pragma once
//------------------------------------------------------------------------------
/**
	Vulkan implementation of the render device.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "coregraphics/base/renderdevicebase.h"
#include "vkcmdbufferthread.h"
#include "vkcpugpuinterlockthread.h"
#include "vkdeferredcommand.h"
namespace Vulkan
{
class VkRenderDevice : public Base::RenderDeviceBase
{
	__DeclareClass(VkRenderDevice);
	__DeclareSingleton(VkRenderDevice);
public:

	enum CommandPass
	{
		OnBeginFrame,
		OnEndFrame,
		OnBeginPass,
		OnNextSubpass,
		OnEndPass,
		OnMainTransferSubmitted,
		OnMainDrawSubmitted,
		OnMainComputeSubmitted,
		OnBeginTransferThread,
		OnBeginDrawThread,
		OnBeginComputeThread,
		OnTransferThreadsSubmitted,
		OnDrawThreadsSubmitted,
		OnComputeThreadsSubmitted,

		OnHandleTransferFences,
		OnHandleDrawFences,
		OnHandleComputeFences,
		OnHandleFences,

		OnBindGraphicsPipeline,
		OnBindComputePipeline,

		NumCommandPasses
	};

	/// constructor
	VkRenderDevice();
	/// destructor
	virtual ~VkRenderDevice();

	/// open the device
	bool Open();
	/// close the device
	void Close();

	/// begin complete frame
	bool BeginFrame(IndexT frameIndex);
	/// set the current vertex stream source
	void SetStreamVertexBuffer(IndexT streamIndex, const Ptr<CoreGraphics::VertexBuffer>& vb, IndexT offsetVertexIndex);
	/// set current vertex layout
	void SetVertexLayout(const Ptr<CoreGraphics::VertexLayout>& vl);
	/// set current index buffer
	void SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& ib);
	/// set the primitive group
	void SetPrimitiveGroup(const CoreGraphics::PrimitiveGroup& pg);
	/// perform computation
	void Compute(int dimX, int dimY, int dimZ, uint flag = NoBarrierBit); // use MemoryBarrierFlag
	/// begins pass with single rendertarget
	void BeginPass(const Ptr<CoreGraphics::RenderTarget>& rt);
	/// begins pass with multiple rendertarget
	void BeginPass(const Ptr<CoreGraphics::MultipleRenderTarget>& mrt);
	/// begins pass with rendertarget cube
	void BeginPass(const Ptr<CoreGraphics::RenderTargetCube>& rtc);
	/// begin a rendering pass
	void BeginPass(const Ptr<CoreGraphics::Pass>& pass);
	/// progress to next subpass
	void SetToNextSubpass();
	/// begin rendering a transform feedback with a vertex buffer as target
	void BeginFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb, CoreGraphics::PrimitiveTopology::Code primType);
	/// begin batch
	void BeginBatch(CoreGraphics::FrameBatchType::Code batchType);
	/// bake the current state of the render device (only used on DX12 and Vulkan renderers where pipeline creation is required)
	void BuildRenderPipeline();
	/// draw current primitives
	void Draw();
	/// draw indexed, instanced primitives (see method header for details)
	void DrawIndexedInstanced(SizeT numInstances, IndexT baseInstance);
	/// draw from stream output/transform feedback buffer
	void DrawFeedback(const Ptr<CoreGraphics::FeedbackBuffer>& fb);
	/// draw from stream output/transform feedback buffer, instanced
	void DrawFeedbackInstanced(const Ptr<CoreGraphics::FeedbackBuffer>& fb, SizeT numInstances);
	/// end batch
	void EndBatch();
	/// end current pass
	void EndPass();
	/// end current feedback
	void EndFeedback();
	/// end complete frame
	void EndFrame(IndexT frameIndex);
	/// present the rendered scene
	void Present();
	/// adds a scissor rect
	void SetScissorRect(const Math::rectangle<int>& rect, int index);
	/// sets viewport
	void SetViewport(const Math::rectangle<int>& rect, int index);

	/// copy data between textures
	void Copy(const Ptr<CoreGraphics::Texture>& from, Math::rectangle<SizeT> fromRegion, const Ptr<CoreGraphics::Texture>& to, Math::rectangle<SizeT> toRegion);
	/// blit between render textures
	void Blit(const Ptr<CoreGraphics::RenderTexture>& from, Math::rectangle<SizeT> fromRegion, const Ptr<CoreGraphics::RenderTexture>& to, Math::rectangle<SizeT> toRegion);

	/// save a screenshot to the provided stream
	CoreGraphics::ImageFileFormat::Code SaveScreenshot(CoreGraphics::ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream);
	/// save a region of the screen to the provided stream
	CoreGraphics::ImageFileFormat::Code SaveScreenshot(CoreGraphics::ImageFileFormat::Code fmt, const Ptr<IO::Stream>& outStream, const Math::rectangle<int>& rect, int x, int y);

	/// call when window gets resized
	void DisplayResized(SizeT width, SizeT height);
	/// returns true if we support parallel transfers
	static bool AsyncTransferSupported();
	/// returns true if we support parallel computes
	static bool AsyncComputeSupported();

	static const short MaxNumRenderTargets = 8;
	static const short MaxNumViewports = 16;

private:
	friend class VkTexture;
	friend class VkShapeRenderer;
	friend class VkTextRenderer;
	friend class VkTransformDevice;
	friend class VkDisplayDevice;
	friend class VkVertexBuffer;
	friend class VkIndexBuffer;
	friend class VkCpuSyncFence;
	friend class VkShader;
	friend class VkShaderProgram;
	friend class VkMemoryVertexBufferLoader;
	friend class VkMemoryIndexBufferLoader;
	friend class VkMemoryTextureLoader;
	friend class VkShaderServer;
	friend class VkRenderTarget;
	friend class VkRenderTargetCube;
	friend class VkMultipleRenderTarget;
	friend class VkDepthStencilTarget;
	friend class VkVertexLayout;
	friend class VkUniformBuffer;
	friend class VkShaderStorageBuffer;
	friend class VkStreamTextureLoader;
	friend class VkStreamTextureSaver;
	friend class VkShaderState;
	friend class VkShaderImage;
	friend class VkCmdEvent;
	friend class VkPass;
	friend class VkRenderTexture;
	friend struct VkDeferredCommand;

	friend VKAPI_ATTR void VKAPI_CALL NebulaVkAllocCallback(void* userData, uint32_t size, VkInternalAllocationType type, VkSystemAllocationScope scope);
	friend VKAPI_ATTR void VKAPI_CALL NebulaVkFreeCallback(void* userData, uint32_t size, VkInternalAllocationType type, VkSystemAllocationScope scope);

	enum PipelineInfoBits
	{
		ShaderInfoSet = 1,
		VertexLayoutInfoSet = 2,
		FramebufferLayoutInfoSet = 4,
		InputLayoutInfoSet = 8,

		AllInfoSet = 15,

		PipelineBuilt = 16
	};

	// open Vulkan device context
	bool OpenVulkanContext();
	/// close opengl4 device context
	void CloseVulkanDevice();
	/// setup the requested adapter for the Vulkan device
	void SetupAdapter();
	/// select the requested buffer formats for the Vulkan device
	void SetupBufferFormats();
	/// setup the remaining presentation parameters
	void SetupPresentParams();
	/// set the initial Vulkan device state
	void SetInitialDeviceState();
	/// sync with gpu
	void SyncGPU();

	/// sets the current shader pipeline information
	void SetShaderPipelineInfo(const VkGraphicsPipelineCreateInfo& shader, const Ptr<VkShaderProgram>& program);
	/// sets the current vertex layout information
	void SetVertexLayoutPipelineInfo(const VkPipelineVertexInputStateCreateInfo& vertexLayout);
	/// sets the current framebuffer layout information
	void SetFramebufferLayoutInfo(const VkGraphicsPipelineCreateInfo& framebufferLayout);
	/// sets the current primitive layout information
	void SetInputLayoutInfo(VkPipelineInputAssemblyStateCreateInfo* inputLayout);
	/// create a new pipeline (or fetch from cache) and bind to command queue
	void CreateAndBindGraphicsPipeline();
	/// bind compute pipeline
	void BindComputePipeline(const VkPipeline& pipeline, const VkPipelineLayout& layout);

	/// update descriptors
	void BindDescriptorsGraphics(const VkDescriptorSet* descriptors, const VkPipelineLayout& layout, uint32_t baseSet, uint32_t setCount, const uint32_t* offsets, uint32_t offsetCount);
	/// update descriptors
	void BindDescriptorsCompute(const VkDescriptorSet* descriptors, const VkPipelineLayout& layout, uint32_t baseSet, uint32_t setCount, const uint32_t* offsets, uint32_t offsetCount);
	/// update push ranges
	void UpdatePushRanges(const VkShaderStageFlags& stages, const VkPipelineLayout& layout, uint32_t offset, uint32_t size, void* data);

	/// setup queue from display
	void SetupQueue(uint32_t queueFamily, uint32_t queueIndex);
	/// figure out which memory type fits given memory bits and required properties
	VkResult GetMemoryType(uint32_t bits, VkMemoryPropertyFlags flags, uint32_t& index);

	/// allocate a buffer memory storage, num is a multiplier for how many times the size needs to be duplicated
	void AllocateBufferMemory(const VkBuffer& buf, VkDeviceMemory& bufmem, VkMemoryPropertyFlagBits flags, uint32_t& bufsize);
	/// allocate an image memory storage, num is a multiplier for how many times the size needs to be duplicated
	void AllocateImageMemory(const VkImage& img, VkDeviceMemory& imgmem, VkMemoryPropertyFlagBits flags, uint32_t& imgsize);

	/// update buffer memory from CPU
	void BufferUpdate(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, const void* data);
	/// update buffer memory from CPU
	void BufferUpdate(VkCommandBuffer cmd, const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, const void* data);
	/// update image memory from CPU
	void ImageUpdate(const VkImage& img, const VkImageCreateInfo& info, uint32_t mip, uint32_t face, VkDeviceSize size, uint32_t* data);
	/// setup staging image update for later execution
	void PushImageUpdate(const VkImage& img, const VkImageCreateInfo& info, uint32_t mip, uint32_t face, VkDeviceSize size, uint32_t* data);
	/// perform image read-back, and saves to buffer (SLOW!)
	void ReadImage(const Ptr<VkTexture>& tex, VkImageCopy copy, uint32_t& outMemSize, VkDeviceMemory& outMem, VkImage& outImage);
	/// perform image write-back, transitions data from buffer to image (SLOW!)
	void WriteImage(const VkImage& srcImg, const VkImage& dstImg, VkImageCopy copy);
	/// helper to begin immediate transfer
	VkCommandBuffer BeginImmediateTransfer();
	/// helper to end immediate transfer
	void EndImmediateTransfer(VkCommandBuffer cmdBuf);
	/// helper to begin an interlocked transfer
	VkCommandBuffer BeginInterlockedTransfer();
	/// helper to end interlocked transfer
	void EndInterlockedTransfer(VkCommandBuffer cmdBuf);
	/// push command to specific sync point
	void PushCommandPass(const VkDeferredCommand& del, const CommandPass pass);
	/// execute commands specific to sync point
	void RunCommandPass(const CommandPass pass);

	/// push image layout change
	void PushImageLayoutTransition(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier);
	/// perform image layout transition immediately
	void ImageLayoutTransition(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier);
	/// perform image layout transition immediately
	void ImageLayoutTransition(VkCommandBuffer buf, VkImageMemoryBarrier barrier);
	/// create image memory barrier
	static VkImageMemoryBarrier ImageMemoryBarrier(const VkImage& img, VkImageSubresourceRange subres, VkImageLayout oldLayout, VkImageLayout newLayout);
	/// create image ownership change
	static VkImageMemoryBarrier ImageMemoryBarrier(const VkImage& img, VkImageSubresourceRange subres, VkDeferredCommand::CommandQueueType fromQueue, VkDeferredCommand::CommandQueueType toQueue, VkImageLayout layout);
	/// create buffer memory barrier
	static VkBufferMemoryBarrier BufferMemoryBarrier(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, VkAccessFlags srcAccess, VkAccessFlags dstAccess);
	/// transition image between layouts
	void ChangeImageLayout(const VkImageMemoryBarrier& barrier, const VkDeferredCommand::CommandQueueType& type);
	/// push transition image ownership transition
	void PushImageOwnershipChange(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier);
	/// transition image ownership
	void ImageOwnershipChange(VkDeferredCommand::CommandQueueType queue, VkImageMemoryBarrier barrier);

	/// push image color clear
	void PushImageColorClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearColorValue clearValue, VkImageSubresourceRange subres);
	/// perform image color clear
	void ImageColorClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearColorValue clearValue, VkImageSubresourceRange subres);
	/// push image depth stencil clear
	void PushImageDepthStencilClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearDepthStencilValue clearValue, VkImageSubresourceRange subres);
	/// perform image depth stencil clear
	void ImageDepthStencilClear(const VkImage& image, const VkDeferredCommand::CommandQueueType& queue, VkImageLayout layout, VkClearDepthStencilValue clearValue, VkImageSubresourceRange subres);
	/// perform attachment clear
	void AttachmentClear(const VkClearAttachment* attachments, uint32_t numAttachments, const VkClearRect* regions, uint32_t numRegions);

	/// helper function to submit a command buffer
	void SubmitToQueue(VkQueue queue, VkPipelineStageFlags flags, uint32_t numBuffers, VkCommandBuffer* buffers);
	/// helper function to submit a fence
	void SubmitToQueue(VkQueue queue, VkFence fence);
	/// wait for queue to finish execution using fence, also resets fence
	void WaitForFences(VkFence* fences, uint32_t numFences, bool waitForAll);

	/// wait for deferred delegates to complete
	void UpdateDelegates();
	/// begin using the worker threads to build command buffers
	void BeginGraphicsCmdThreads();
	/// end using the worker threads
	void EndGraphicsCmdThreads();
	/// begin using the worker threads to build command buffers
	void BeginComputeCmdThreads();
	/// end using the worker threads
	void EndComputeCmdThreads();
	/// begin using the worker threads to build command buffers
	void BeginTransferCmdThreads();
	/// end using the worker threads
	void EndTransferCmdThreads();
	/// begin command threads from array of threads and array of command buffers
	void BeginCmdThreads(const Ptr<Vulkan::VkCmdBufferThread>* threads, VkCommandPool* commandBufferPools, IndexT firstThread, SizeT numThreads, VkCommandBuffer* buffers);
	/// end command threads
	void EndCmdThreads(const Ptr<Vulkan::VkCmdBufferThread>* threads, IndexT firstThread, SizeT numThreads, Threading::Event* completionEvents);
	/// completes thread-constructed buffers by executing the comands gathered in the threads on the main buffer, syncPoint must be a pass which handles fences
	void FinishCmdThreads(VkCommandBuffer mainBuffer, IndexT firstThread, SizeT numThreads, VkCommandBuffer* threadBuffers, VkCommandPool* commandBufferPools, CommandPass syncPoint);

	/// start up new draw thread
	void BeginDrawThread();
	/// start up all draw threads
	void BeginDrawThreadCluster();
	/// finish current draw threads
	void EndDrawThreadCluster();
	/// finish current draw threads
	void EndDrawThreads();
	/// continues to next thread
	void NextThread();
	/// add command to thread
	void PushToThread(const VkCmdBufferThread::Command& cmd, const IndexT& index, bool allowStaging = true);
	/// flush remaining staging thread commands
	void FlushToThread(const IndexT& index);

	/// add command to interlock thread
	void PushToInterlockThread(const VkCpuGpuInterlockThread::Command& cmd);
	/// tell interlock thread to wait for previous draw command to finish
	void InterlockWaitCPU(VkPipelineStageFlags stage);
	/// tell interlock thread to block GPU thread waiting for a buffer
	void InterlockWaitGPU(VkPipelineStageFlags waitStage, VkPipelineStageFlags signalStage, VkBufferMemoryBarrier& buffer);
	/// tell interlock thread to block GPU thread waiting for an image
	void InterlockWaitGPU(VkPipelineStageFlags waitStage, VkPipelineStageFlags signalStage, VkImageMemoryBarrier& image);
	/// tell interlock thread to block GPU thread waiting for an image
	void InterlockWaitGPU(VkPipelineStageFlags waitStage, VkPipelineStageFlags signalStage, VkMemoryBarrier& barrier);
	/// give interlock thread a memcpy assignment
	void InterlockMemcpy(uint32_t size, uint32_t offset, const void* data, void* mappedData);

	/// binds common descriptors
	void BindSharedDescriptorSets();

	uint32_t adapter;
	uint32_t frameId;
	VkPhysicalDeviceMemoryProperties memoryProps;

	const uint32_t VkPoolMaxSets = 65536;
	const uint32_t VkPoolSetSize = 65536;

	VkPhysicalDevice devices[64];
	VkExtensionProperties physicalExtensions[64];

	uint32_t usedPhysicalExtensions;
	const char* deviceExtensionStrings[64];

	uint32_t usedExtensions;
	const char* extensions[64];

	uint32_t numQueues;
	VkQueueFamilyProperties queuesProps[64];

	uint32_t drawQueueFamily;
	uint32_t computeQueueFamily;
	uint32_t transferQueueFamily;
	uint32_t drawQueueIdx;
	uint32_t computeQueueIdx;
	uint32_t transferQueueIdx;
	bool putTransferFenceThisFrame;
	bool putDrawFenceThisFrame;
	bool putComputeFenceThisFrame;
	Util::Dictionary<VkFence, Util::Array<VkDeferredCommand>> transferFenceCommands;
	Util::Dictionary<VkFence, Util::Array<VkDeferredCommand>> drawFenceCommands;
	Util::Dictionary<VkFence, Util::Array<VkDeferredCommand>> computeFenceCommands;
	Util::FixedArray<Util::Array<VkDeferredCommand>> commands;

	// stuff used for the swap chain
	VkFormat format;
	VkColorSpaceKHR colorSpace;
	VkSwapchainKHR swapchain;
	VkSemaphore displaySemaphore;

	uint32_t currentBackbuffer;
	Util::FixedArray<VkImage> backbuffers;
	Util::FixedArray<VkDeviceMemory> backbufferMem;
	Util::FixedArray<VkImageView> backbufferViews;
	Util::FixedArray<VkSemaphore> backbufferSemaphores;
	uint32_t numBackbuffers;
	VkRect2D displayRect;

	static VkDevice dev;
	static VkDescriptorPool descPool;
	static VkQueue drawQueue;
	static VkQueue computeQueue;
	static VkQueue transferQueue;
	static VkInstance instance;
	static VkPhysicalDevice physicalDev;
	static VkPipelineCache cache;
	static VkCommandBuffer mainCmdDrawBuffer;
	static VkCommandBuffer mainCmdCmpBuffer;
	static VkCommandBuffer mainCmdTransBuffer;
	VkFence mainCmdDrawFence;
	VkFence mainCmdCmpFence;
	VkFence mainCmdTransFence;

	VkPhysicalDeviceProperties deviceProps;
	VkPhysicalDeviceFeatures deviceFeatures;

	VkCommandBufferInheritanceInfo passInfo;
	VkPipelineInputAssemblyStateCreateInfo inputInfo;
	VkPipelineColorBlendStateCreateInfo blendInfo;

	static const SizeT NumDrawThreads = 8;
	IndexT currentDrawThread;
	VkCommandPool dispatchableCmdDrawBufferPool[NumDrawThreads];
	VkCommandBuffer dispatchableDrawCmdBuffers[NumDrawThreads];
	Ptr<VkCmdBufferThread> drawThreads[NumDrawThreads];
	Threading::Event drawCompletionEvents[NumDrawThreads];

	static const SizeT NumTransferThreads = 1;
	IndexT currentTransThread;
	VkCommandPool dispatchableCmdTransBufferPool[NumTransferThreads];
	VkCommandBuffer dispatchableTransCmdBuffers[NumTransferThreads];
	Ptr<VkCmdBufferThread> transThreads[NumTransferThreads];
	Threading::Event transCompletionEvents[NumTransferThreads];

	static const SizeT NumComputeThreads = 1;
	IndexT currentComputeThread;
	VkCommandPool dispatchableCmdCompBufferPool[NumComputeThreads];
	VkCommandBuffer dispatchableCompCmdBuffers[NumComputeThreads];
	Ptr<VkCmdBufferThread> compThreads[NumComputeThreads];
	Threading::Event compCompletionEvents[NumComputeThreads];

	Util::Array<VkCmdBufferThread::Command> threadCmds[NumDrawThreads];
	SizeT numCallsLastFrame;
	SizeT numActiveThreads;
	SizeT numUsedThreads;

	Ptr<VkCpuGpuInterlockThread> interlockThread;

	VkPipelineVertexInputStateCreateInfo vertexInfo;
	VkViewport* passViewports;
	uint32_t numVsInputs;

	// first pool is for persistent buffers, second is for transient
	static VkCommandPool mainCmdDrawPool;
	static VkCommandPool mainCmdCmpPool;
	static VkCommandPool mainCmdTransPool;

	static VkAllocationCallbacks alloc;
	
	VkCommandPool immediateCmdDrawPool;
	VkCommandPool immediateCmdTransPool;

	VkGraphicsPipelineCreateInfo currentPipelineInfo;
	VkPipelineLayout currentPipelineLayout;
	VkPipeline currentPipeline;
	uint currentPipelineBits;
	uint32_t numViewports;
	VkViewport* viewports;
	uint32_t numScissors;
	VkRect2D* scissors;
	
	Ptr<VkShaderProgram> currentProgram;

	_declare_counter(NumImageBytesAllocated);
	_declare_counter(NumBufferBytesAllocated);
	_declare_counter(NumBytesAllocated);

#if NEBULAT_VULKAN_DEBUG
	VkDebugReportCallbackEXT debugCallbackHandle;
	PFN_vkCreateDebugReportCallbackEXT debugCallbackPtr;
#endif
};

} // namespace Vulkan