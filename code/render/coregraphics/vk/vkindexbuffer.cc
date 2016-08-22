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
	
	if (VkRenderDevice::AsyncTransferSupported())
	{
		this->updCmd = VkRenderDevice::Instance()->BeginImmediateTransfer();
		this->lock->SetWaitCommandBuffer(this->updCmd, VK_PIPELINE_STAGE_TRANSFER_BIT);
		this->lock->WaitForRange(offset, length);
	}
	else
	{
		// let CPU signal GPU to continue
		VkRenderDevice::Instance()->InterlockCPUSignal(VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkIndexBuffer::Update(const void* data, SizeT offset, SizeT length, void* mappedData)
{
	if (VkRenderDevice::AsyncTransferSupported())
	{
		// send update 
		VkRenderDevice::Instance()->BufferUpdate(VkRenderDevice::mainCmdDrawBuffer, this->buf, VkDeviceSize(offset), VkDeviceSize(length), data);
	}
	else
	{
		// perform interlocked memcpy
		VkRenderDevice::Instance()->InterlockMemcpy(length, offset, data, mappedData);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkIndexBuffer::Lock(SizeT offset, SizeT length)
{
	// lock buffer by telling the main draw buffer to wait for vertex inputs
	if (VkRenderDevice::AsyncTransferSupported())
	{
		this->lock->SetSignalCommandBuffer(VkRenderDevice::mainCmdDrawBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
		this->lock->LockRange(offset, length);
		VkRenderDevice::Instance()->EndImmediateTransfer(this->updCmd);
		this->updCmd = VK_NULL_HANDLE;
	}
	else
	{
		// let GPU signal CPU to continue
		VkRenderDevice::Instance()->InterlockGPUSignal(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
	}
	BufferBase::Lock(offset, length);
}

} // namespace Vulkan