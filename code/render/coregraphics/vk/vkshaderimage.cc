//------------------------------------------------------------------------------
// vkshaderimage.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaderimage.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderImage, 'VKSI', Base::ShaderReadWriteTextureBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderImage::VkShaderImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderImage::~VkShaderImage()
{
	// empty
}

} // namespace Vulkan