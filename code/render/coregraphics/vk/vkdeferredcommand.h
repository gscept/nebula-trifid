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
		__RunAfterFence,		// don't use this flag, but all delegates prior to this flag requires a frame to be complete before it can occur

		UpdateBuffer,
		UpdateImage,

		ClearColorImage,
		ClearDepthStencilImage,

		ChangeImageLayout
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
				VkBufferImageCopy copy;
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
		};
	} del;

	VkDevice dev;
	bool runUponCompletion;

	/// run delegate action
	void RunDelegate();
};


} // namespace Vulkan