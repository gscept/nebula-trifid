#pragma once
//------------------------------------------------------------------------------
/**
	Implements a Vulkan immediate shape and primitive renderer (debug meshes, random primitives, wireframes etc)
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaperendererbase.h"
namespace Vulkan
{
class VkShapeRenderer : public Base::ShapeRendererBase
{
	__DeclareClass(VkShapeRenderer);
public:
	/// constructor
	VkShapeRenderer();
	/// destructor
	virtual ~VkShapeRenderer();
private:
};
} // namespace Vulkan