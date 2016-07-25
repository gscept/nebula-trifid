//------------------------------------------------------------------------------
// vkrendertargetcube.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkrendertargetcube.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkRenderTargetCube, 'VKRC', Base::RenderTargetCubeBase);
//------------------------------------------------------------------------------
/**
*/
VkRenderTargetCube::VkRenderTargetCube()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkRenderTargetCube::~VkRenderTargetCube()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTargetCube::Setup()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTargetCube::Discard()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VkRenderTargetCube::EndPass()
{
	RenderTargetCubeBase::EndPass();
}

} // namespace Vulkan