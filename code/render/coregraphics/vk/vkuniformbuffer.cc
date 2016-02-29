//------------------------------------------------------------------------------
// vkuniformbuffer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkuniformbuffer.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkUniformBuffer, 'VKUB', Base::ConstantBufferBase);
//------------------------------------------------------------------------------
/**
*/
VkUniformBuffer::VkUniformBuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkUniformBuffer::~VkUniformBuffer()
{
	// empty
}

} // namespace Vulkan