#pragma once
//------------------------------------------------------------------------------
/**
	@class Grid::GridRTPlugin
	
	The GridRTPlugin injects itself into the main frameshader and renders the grid in the shape-pass.
	
	(C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "rendermodules/rt/rtplugin.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/texture.h"
#include "resources/managedtexture.h"
namespace Grid
{
class GridRTPlugin : public RenderModules::RTPlugin
{
	__DeclareClass(GridRTPlugin);
public:
	/// constructor
	GridRTPlugin();
	/// destructor
	virtual ~GridRTPlugin();

	/// called when plugin is registered on the render-thread side
	virtual void OnRegister();
	/// called when plugin is unregistered on the render-thread side
	virtual void OnUnregister();

	/// called when rendering a frame batch
	virtual void OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch);

	/// set if grid should be visible
	void SetVisible(bool b);
	/// set grid tile size
	void SetGridSize(int size);
	/// set grid cell size
	void SetCellSize(float size);

private:
	bool visible;
	int gridSize;
	float cellSize;

	// mesh
	CoreGraphics::PrimitiveGroup primitive;
	Ptr<CoreGraphics::VertexBuffer> vbo;
	Ptr<CoreGraphics::IndexBuffer> ibo;
	Ptr<Resources::ManagedTexture> tex;

	// shader
	Ptr<CoreGraphics::ShaderInstance> shader;
	Ptr<CoreGraphics::ShaderVariable> gridSizeVar;
	Ptr<CoreGraphics::ShaderVariable> gridTexVar;
};

//------------------------------------------------------------------------------
/**
*/
inline void
GridRTPlugin::SetVisible(bool b)
{
	this->visible = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GridRTPlugin::SetGridSize(int size)
{
	this->gridSize = size;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GridRTPlugin::SetCellSize(float size)
{
	this->cellSize = size;
}

} // namespace Grid