#pragma once
//------------------------------------------------------------------------------
/**
	@class Grid::GridRTPlugin
	
	The GridRTPlugin injects itself into the main frameshader and renders the grid in the shape-pass.
	
	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "rendermodules/rt/rtplugin.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/shaderstate.h"
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
	void OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch);
	/// called when rendering a frame batch
	void OnRender(const Util::StringAtom& filter);

	/// set if grid should be visible
	void SetVisible(bool b);
	/// set grid tile size
	void SetGridSize(float size);

private:
	bool visible;
	float gridSize;

	// mesh
	CoreGraphics::PrimitiveGroup primitive;
	Ptr<CoreGraphics::VertexBuffer> vbo;
	Ptr<CoreGraphics::IndexBuffer> ibo;
	Ptr<Resources::ManagedTexture> tex;

	// shader
	Ptr<CoreGraphics::ShaderState> shader;
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
GridRTPlugin::SetGridSize(float size)
{
	this->gridSize = size;
}

} // namespace Grid
