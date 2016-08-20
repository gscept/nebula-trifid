//------------------------------------------------------------------------------
// vkvertexbuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkvertexbuffer.h"
#include "vkrenderdevice.h"

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

	// wait for previous submissions to finish
	this->lock->WaitForRange(offset, length);
	this->updCmd = VkRenderDevice::Instance()->BeginInterlockedTransfer();
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexBuffer::Update(const void* data, SizeT offset, SizeT length)
{
	// send update 
	VkRenderDevice::Instance()->BufferUpdate(this->updCmd, this->buf, VkDeviceSize(offset), VkDeviceSize(length), data);
}

//------------------------------------------------------------------------------
/**
*/
void
VkVertexBuffer::Lock(SizeT offset, SizeT length)
{
	VkRenderDevice::Instance()->EndInterlockedTransfer(this->updCmd);
	this->updCmd = VK_NULL_HANDLE;
	this->lock->LockRange(offset, length);
	BufferBase::Lock(offset, length);
}

} // namespace Vulkan