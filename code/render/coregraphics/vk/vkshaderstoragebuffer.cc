//------------------------------------------------------------------------------
// vkshaderstoragebuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderstoragebuffer.h"
#include "vkrenderdevice.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderStorageBuffer, 'VKSB', Base::ShaderReadWriteBufferBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderStorageBuffer::VkShaderStorageBuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderStorageBuffer::~VkShaderStorageBuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderStorageBuffer::Setup(const SizeT numBackingBuffers)
{
	ShaderReadWriteBufferBase::Setup(numBackingBuffers);

	uint32_t queues[] = { VkRenderDevice::Instance()->renderQueueIdx, VkRenderDevice::Instance()->computeQueueIdx, VkRenderDevice::Instance()->transferQueueIdx };
	VkBufferCreateInfo info =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		NULL,
		0,
		this->size,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_SHARING_MODE_CONCURRENT,
		sizeof(queues) / sizeof(uint32_t),
		queues
	};
	VkResult res = vkCreateBuffer(VkRenderDevice::dev, &info, NULL, &this->buf);
	n_assert(res == VK_SUCCESS);

	uint32_t alignedSize;
	VkRenderDevice::Instance()->AllocateBufferMemory(this->buf, this->mem, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), alignedSize);

	// bind to buffer
	vkBindBufferMemory(VkRenderDevice::dev, this->buf, this->mem, 0);

	// map memory so we can use it later
	res = vkMapMemory(VkRenderDevice::dev, this->mem, 0, alignedSize, 0, &this->buffer);
	n_assert(res == VK_SUCCESS);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderStorageBuffer::Discard()
{
	vkUnmapMemory(VkRenderDevice::dev, this->mem);
	vkFreeMemory(VkRenderDevice::dev, this->mem, NULL);
	vkDestroyBuffer(VkRenderDevice::dev, this->buf, NULL);
}

} // namespace Vulkan