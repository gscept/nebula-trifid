//------------------------------------------------------------------------------
// vkvertexbuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkvertexbuffer.h"
#include "vkrenderdevice.h"
#include "vkcmdbufferthread.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkVertexBuffer, 'VKVB', Base::VertexBufferBase);
//------------------------------------------------------------------------------
/**
*/
VkVertexBuffer::VkVertexBuffer() :
	mapcount(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkVertexBuffer::~VkVertexBuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexBuffer::Unload()
{
	n_assert(this->mapcount == 0);
	vkFreeMemory(VkRenderDevice::dev, this->mem, NULL);
	vkDestroyBuffer(VkRenderDevice::dev, this->buf, NULL);
}

//------------------------------------------------------------------------------
/**
*/
void*
VkVertexBuffer::Map(MapType mapType)
{
	this->mapcount++;
	void* buf;
	VkResult res = vkMapMemory(VkRenderDevice::dev, this->mem, 0, this->vertexLayout->GetVertexByteSize() * this->numVertices, 0, &buf);
	n_assert(res == VK_SUCCESS);
	return buf;
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexBuffer::Unmap()
{
	vkUnmapMemory(VkRenderDevice::dev, this->mem);
	this->mapcount--;
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexBuffer::Unlock(SizeT offset, SizeT length)
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
		this->barrier = VkRenderDevice::Instance()->BufferMemoryBarrier(this->buf, offset, length, VK_ACCESS_HOST_READ_BIT, VK_ACCESS_HOST_WRITE_BIT);
		this->memBarrier.pNext = NULL;
		this->memBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		this->memBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		this->memBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		VkRenderDevice::Instance()->InterlockWaitGPU(VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, this->memBarrier);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexBuffer::Update(const void* data, SizeT offset, SizeT length, void* mappedData)
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
VkVertexBuffer::Lock(SizeT offset, SizeT length)
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
		VkRenderDevice::Instance()->InterlockWaitCPU(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
	}
	BufferBase::Lock(offset, length);
}

} // namespace Vulkan