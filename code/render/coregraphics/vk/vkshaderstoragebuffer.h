#pragma once
//------------------------------------------------------------------------------
/**
	Implements a read/write buffer used within shaders, in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaderreadwritebufferbase.h"
namespace Vulkan
{
class VkShaderStorageBuffer : public Base::ShaderReadWriteBufferBase
{
	__DeclareClass(VkShaderStorageBuffer);
public:
	/// constructor
	VkShaderStorageBuffer();
	/// destructor
	virtual ~VkShaderStorageBuffer();

	/// setup buffer
	void Setup(const SizeT numBackingBuffers = DefaultNumBackingBuffers);
	/// discard buffer
	void Discard();
private:

	void* buffer;
	VkBuffer buf;
	VkDeviceMemory mem;
};
} // namespace Vulkan