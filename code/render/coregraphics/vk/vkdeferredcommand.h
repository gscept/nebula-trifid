#pragma once
//------------------------------------------------------------------------------
/**
	Implements a deferred delegate, which is used to perform an action whenever a fence object is done.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "util/fixedarray.h"
#include <vulkan/vulkan.h>
namespace Vulkan
{
struct VkDeferredCommand
{
	/// constructor
	VkDeferredCommand();
	/// destructor
	~VkDeferredCommand();

	enum DelegateType
	{
		FreeCmdBuffers,
		FreeMemory,
		FreeBuffer,
		FreeImage,
		__RunAfterFence,		// don't use this flag, but all delegates prior to this flag requires a frame to be complete before it can occur

		BindDescriptorSets,		// this can actually be done outside of a frame (views, custom code, etc)

		UpdateBuffer,
		UpdateImage,

		ClearColorImage,
		ClearDepthStencilImage,
		ImageOwnershipChange,
		ImageLayoutTransition
	};

	enum CommandQueueType
	{
		Graphics,
		Compute,
		Transfer
	};

	struct Delegate
	{
		DelegateType type;
		CommandQueueType queue;
		VkFence fence;
		union
		{
			struct CmdBufferFree
			{
				VkCommandBuffer buffers[64];
				VkCommandPool pool;
				uint32_t numBuffers;
			} cmdbufferfree;

			struct MemoryFree
			{
				void* data;
			} memory;

			struct BufferFree
			{
				VkBuffer buf;
				VkDeviceMemory mem;
			} buffer;

			struct ImageFree
			{
				VkImage img;
				VkDeviceMemory mem;
			} image;

			struct BufferUpdate
			{
				VkBuffer buf;
				VkDeviceSize offset;
				VkDeviceSize size;
				uint32_t* data;
			} bufferUpd;

			struct ImageUpdate
			{
				VkImage img;
				VkImageCreateInfo info;
				uint32_t mip;
				uint32_t face;
				VkDeviceSize size;
				uint32_t* data;
			} imageUpd;

			struct ImageLayoutChange
			{
				VkImageMemoryBarrier barrier;
			} imgBarrier;

			struct ImageColorClear
			{
				VkImage img;
				VkImageLayout layout;
				VkClearColorValue clearValue;
				VkImageSubresourceRange region;
			} imgColorClear;

			struct ImageDepthStencilClear
			{
				VkImage img;
				VkImageLayout layout;
				VkClearDepthStencilValue clearValue;
				VkImageSubresourceRange region;
			} imgDepthStencilClear;

			struct ImageOwnershipTransition
			{
				VkImageMemoryBarrier barrier;
			} imgOwnerChange;

			struct DescriptorSetBind
			{
				VkPipelineBindPoint type;
				VkPipelineLayout layout;
				uint32_t baseSet;
				uint32_t numSets;
				const VkDescriptorSet* sets;
				uint32_t numOffsets;
				const uint32_t* offsets;
			} descSetBind;
		};
	} del;

	VkDevice dev;

	/// run delegate action
	void RunDelegate();
};


} // namespace Vulkan