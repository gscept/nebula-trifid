#pragma once
//------------------------------------------------------------------------------
/**
	Implements a uniform buffer used for shader uniforms in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/constantbufferbase.h"
namespace Vulkan
{
class VkUniformBuffer : public Base::ConstantBufferBase
{
	__DeclareClass(VkUniformBuffer);
public:
	/// constructor
	VkUniformBuffer();
	/// destructor
	virtual ~VkUniformBuffer();

	/// setup buffer
	void Setup(const SizeT numBackingBuffers = DefaultNumBackingBuffers);
	/// bind variables in a block with a name in a shader to this buffer (only do this on system managed blocks)
	void SetupFromBlockInShader(const Ptr<CoreGraphics::ShaderState>& shader, const Util::String& blockName, const SizeT numBackingBuffers = DefaultNumBackingBuffers);
	/// discard buffer
	void Discard();

	/// allocates instance memory, and returns offset into buffer at new instance
	SizeT AllocateInstance(SizeT numInstances = 1);
	/// deallocates instance memory
	void FreeInstance(SizeT offset);

	/// get buffer
	const VkBuffer& GetVkBuffer() const;
	/// get memory
	const VkDeviceMemory& GetVkMemory() const;
private:

	/// update buffer asynchronously, depending on implementation, this might overwrite data before used
	virtual void UpdateAsync(void* data, uint offset, uint size);
	/// update segment of buffer as array, depending on implementation, this might overwrite data before used
	virtual void UpdateArrayAsync(void* data, uint offset, uint size, uint count);

	/// grow uniform buffer, returns new aligned size
	uint32_t Grow(SizeT oldCapacity, SizeT growBy);

	VkBufferCreateInfo createInfo;
	uint32_t stride;
	VkDeviceMemory mem;
	VkBuffer buf;
};

//------------------------------------------------------------------------------
/**
*/
inline const VkBuffer&
VkUniformBuffer::GetVkBuffer() const
{
	return this->buf;
}

//------------------------------------------------------------------------------
/**
*/
inline const VkDeviceMemory&
VkUniformBuffer::GetVkMemory() const
{
	return this->mem;
}

} // namespace Vulkan