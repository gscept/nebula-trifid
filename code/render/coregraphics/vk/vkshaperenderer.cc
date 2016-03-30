//------------------------------------------------------------------------------
// vkshaperenderer.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshaperenderer.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkShapeRenderer, 'VKSR', Base::ShapeRendererBase);
//------------------------------------------------------------------------------
/**
*/
VkShapeRenderer::VkShapeRenderer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShapeRenderer::~VkShapeRenderer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShapeRenderer::Open()
{
	ShapeRendererBase::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
VkShapeRenderer::Close()
{
	ShapeRendererBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
VkShapeRenderer::DrawShapes()
{

}

} // namespace Vulkan