#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan index buffer.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/indexbufferbase.h"

namespace Vulkan
{
class VkIndexBuffer : public Base::IndexBufferBase
{
	__DeclareClass(VkIndexBuffer);
public:
	/// constructor
	VkIndexBuffer();
	/// destructor
	virtual ~VkIndexBuffer();

	/// unload the resource, or cancel the pending load
	virtual void Unload();
	/// map the vertices for CPU access
	void* Map(MapType mapType);
	/// unmap the resource
	void Unmap();

	/// unlock range within index buffer
	void Unlock(SizeT offset, SizeT length);
	/// handle updating the vertex buffer
	void Update(const void* data, SizeT offset, SizeT length);
	/// lock range within index buffer
	void Lock(SizeT offset, SizeT length);

	/// get vulkan buffer
	const VkBuffer& GetVkBuffer() const;
	/// set vulkan buffer and device memory
	void SetVkBuffer(const VkBuffer& buf, const VkDeviceMemory& mem);
private:
	VkDeviceMemory mem;
	VkBuffer buf;
	uint32_t mapcount;

	VkCommandBuffer updCmd;
};


//------------------------------------------------------------------------------
/**
*/
inline const VkBuffer&
VkIndexBuffer::GetVkBuffer() const
{
	return this->buf;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VkIndexBuffer::SetVkBuffer(const VkBuffer& buf, const VkDeviceMemory& mem)
{
	this->buf = buf;
	this->mem = mem;
}

} // namespace Vulkan