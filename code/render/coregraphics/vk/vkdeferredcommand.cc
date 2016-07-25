//------------------------------------------------------------------------------
// vkdeferreddelegate.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkdeferredcommand.h"
#include "vkrenderdevice.h"

namespace Vulkan
{

//------------------------------------------------------------------------------
/**
*/
VkDeferredCommand::VkDeferredCommand() : 
	dev(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkDeferredCommand::~VkDeferredCommand()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkDeferredCommand::RunDelegate()
{
	n_assert(this->dev != 0);
	return;

	// this means the fence is signaled
	switch (this->del.type)
	{
	case FreeCmdBuffers:
		vkFreeCommandBuffers(this->dev, this->del.cmdbufferfree.pool, this->del.cmdbufferfree.numBuffers, this->del.cmdbufferfree.buffers);
		break;
	case FreeMemory:
		free(this->del.memory.data);
		break;
	case FreeBuffer:
		vkFreeMemory(this->dev, this->del.buffer.mem, NULL);
		vkDestroyBuffer(this->dev, this->del.buffer.buf, NULL);
		break;
	case UpdateBuffer:
		VkRenderDevice::Instance()->BufferUpdate(this->del.bufferUpd.buf, this->del.bufferUpd.offset, this->del.bufferUpd.size, this->del.bufferUpd.data);
		break;
	case UpdateImage:
		VkRenderDevice::Instance()->ImageUpdate(this->del.imageUpd.img, this->del.imageUpd.copy, this->del.imageUpd.size, this->del.imageUpd.data);
		break;
	case ChangeImageLayout:
		VkRenderDevice::Instance()->ImageLayoutTransition(this->del.queue, this->del.imgBarrier.barrier);
		break;
	case ClearColorImage:
		VkRenderDevice::Instance()->ImageColorClear(this->del.imgColorClear.img, this->del.queue, this->del.imgColorClear.layout, this->del.imgColorClear.clearValue, this->del.imgColorClear.region);
		break;
	case ClearDepthStencilImage:
		VkRenderDevice::Instance()->ImageDepthStencilClear(this->del.imgDepthStencilClear.img, this->del.queue, this->del.imgDepthStencilClear.layout, this->del.imgDepthStencilClear.clearValue, this->del.imgDepthStencilClear.region);
		break;
	}
}


} // namespace Vulkan