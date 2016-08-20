//------------------------------------------------------------------------------
// vkindexbuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkindexbuffer.h"
#include "vkrenderdevice.h"
#include "coregraphics/indextype.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkIndexBuffer, 'VKIB', Base::IndexBufferBase);
//------------------------------------------------------------------------------
/**
*/
VkIndexBuffer::VkIndexBuffer() :
	mapcount(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkIndexBuffer::~VkIndexBuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkIndexBuffer::Unload()
{
	n_assert(this->mapcount == 0);
	vkFreeMemory(VkRenderDevice::dev, this->mem, NULL);
	vkDestroyBuffer(VkRenderDevice::dev, this->buf, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void*
VkIndexBuffer::Map(MapType mapType)
{
	this->mapcount++;
	void* buf;
	VkResult res = vkMapMemory(VkRenderDevice::dev, this->mem, 0, this->numIndices * CoreGraphics::IndexType::SizeOf(this->indexType), 0, &buf);
	n_assert(res == VK_SUCCESS);
	return buf;
}

//------------------------------------------------------------------------------
/**
*/
void
VkIndexBuffer::Unmap()
{
	vkUnmapMemory(VkRenderDevice::dev, this->mem);
	this->mapcount--;
}

//------------------------------------------------------------------------------
/**
*/
void
VkIndexBuffer::Unlock(SizeT offset, SizeT length)
{
	BufferBase::Unlock(offset, length);
	
	// unlock buffer by giving the lock the command buffer used to wait
	this->updCmd = VkRenderDevice::Instance()->BeginImmediateTransfer();
	this->lock->SetWaitCommandBuffer(this->updCmd, VK_PIPELINE_STAGE_TRANSFER_BIT);
	this->lock->WaitForRange(offset, length);
}

//------------------------------------------------------------------------------
/**
*/
void
VkIndexBuffer::Update(const void* data, SizeT offset, SizeT length)
{
	VkRenderDevice::Instance()->BufferUpdate(this->updCmd, this->buf, VkDeviceSize(offset), VkDeviceSize(length), data);
}

//------------------------------------------------------------------------------
/**
*/
void
VkIndexBuffer::Lock(SizeT offset, SizeT length)
{
	// lock buffer by telling the main draw buffer to wait for vertex inputs
	this->lock->SetSignalCommandBuffer(VkRenderDevice::mainCmdDrawBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
	this->lock->LockRange(offset, length);

	VkRenderDevice::Instance()->EndImmediateTransfer(this->updCmd);
	this->updCmd = VK_NULL_HANDLE; 
	BufferBase::Lock(offset, length);
}

} // namespace Vulkan