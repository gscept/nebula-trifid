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

	/// open the shape renderer
	void Open();
	/// close the shape renderer
	void Close();

	/// draw attached shapes and clear deferred stack, must be called inside render loop
	void DrawShapes();

	/// maximum amount of vertices to be rendered by drawprimitives and drawindexedprimitives
	static const int MaxNumVertices = 262140;
	/// maximum amount of indices to be rendered by drawprimitives and drawindexedprimitives
	static const int MaxNumIndices = 262140;

	/// maximum size for primitive size (4 floats for position, 4 floats for color)
	static const int MaxVertexWidth = 8 * sizeof(float);
	/// maximum size for an index
	static const int MaxIndexWidth = sizeof(int);
private:
};
} // namespace Vulkan