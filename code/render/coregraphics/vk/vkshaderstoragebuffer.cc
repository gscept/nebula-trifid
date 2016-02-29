//------------------------------------------------------------------------------
// vkshaderstoragebuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderstoragebuffer.h"

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

} // namespace Vulkan