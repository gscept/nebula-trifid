//------------------------------------------------------------------------------
// vkdeferreddelegate.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkdeferreddelegate.h"

namespace Vulkan
{

//------------------------------------------------------------------------------
/**
*/
VkDeferredDelegate::VkDeferredDelegate() :
	currentStatus(VK_NOT_READY)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkDeferredDelegate::~VkDeferredDelegate()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkDeferredDelegate::operator=(const VkDeferredDelegate& rhs)
{
	this->del = rhs.del;
}

//------------------------------------------------------------------------------
/**
*/
void
VkDeferredDelegate::RunDelegate()
{
	// this means the fence is signaled
	switch (this->del.type)
	{
	case FreeCmdBuffers:
		vkFreeCommandBuffers(this->dev, this->del.cmdbufferfree.pool, this->del.cmdbufferfree.numBuffers, this->del.cmdbufferfree.buffers);
		break;
	case FreeMemory:
		free(this->del.memory.data);
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
VkDeferredDelegate::CheckAsync()
{
	n_assert(this->currentStatus != VK_SUCCESS);
	this->currentStatus = vkGetFenceStatus(this->dev, this->fence);
	if (this->currentStatus == VK_SUCCESS)
	{
		this->RunDelegate();
		this->Reset();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
VkDeferredDelegate::CheckSync()
{
	n_assert(this->currentStatus != VK_SUCCESS);
	vkWaitForFences(this->dev, 1, &this->fence, VK_TRUE, UINT64_MAX);
	this->RunDelegate();
	this->Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
VkDeferredDelegate::Reset()
{
	this->currentStatus = VK_NOT_READY;
	vkResetFences(this->dev, 1, &this->fence);
}

} // namespace Vulkan