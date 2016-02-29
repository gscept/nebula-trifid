//------------------------------------------------------------------------------
// vkdepthstenciltarget.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkdepthstenciltarget.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkDepthStencilTarget, 'VKDS', Base::DepthStencilTargetBase);
//------------------------------------------------------------------------------
/**
*/
VkDepthStencilTarget::VkDepthStencilTarget()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkDepthStencilTarget::~VkDepthStencilTarget()
{
	// empty
}

} // namespace Vulkan