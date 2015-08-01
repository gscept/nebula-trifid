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
#include "graphics/modelentity.h"
#include "renderutil/drawfullscreenquad.h"
namespace Silhouette
{
class SilhouetteRTPlugin : public RenderModules::RTPlugin
{
	__DeclareClass(SilhouetteRTPlugin);
public:
	/// constructor
	SilhouetteRTPlugin();
	/// destructor
	virtual ~SilhouetteRTPlugin();

	/// called when plugin is registered on the render-thread side
	virtual void OnRegister();
	/// called when plugin is unregistered on the render-thread side
	virtual void OnUnregister();

	/// called when rendering a frame batch
	virtual void OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch);

	/// set if grid should be visible
	void SetVisible(bool b);
	/// set models to be rendered
	void SetModels(const Util::Array<Ptr<Graphics::ModelEntity>>& mdls);
	/// set color to be used for rendering silhouette
	void SetColor(const Math::float4& col);

private:
	bool visible;

	// model
	Ptr<Graphics::ModelEntity> model;
	Util::Array<Ptr<Graphics::ModelEntity>> models;

	// shader
	Ptr<CoreGraphics::Shader> shader;
	Ptr<CoreGraphics::ShaderVariable> colorVar;
	CoreGraphics::ShaderFeature::Mask prepassVariation;	
	CoreGraphics::ShaderFeature::Mask outlineVariation;

	// color
	Math::float4 color;
};

//------------------------------------------------------------------------------
/**
*/
inline void
SilhouetteRTPlugin::SetVisible(bool b)
{
	this->visible = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
SilhouetteRTPlugin::SetModels(const Util::Array<Ptr<Graphics::ModelEntity>>& mdls)
{
	this->models = mdls;
}

//------------------------------------------------------------------------------
/**
*/
inline void
SilhouetteRTPlugin::SetColor(const Math::float4& col)
{
	this->color = col;
}

} // namespace Grid