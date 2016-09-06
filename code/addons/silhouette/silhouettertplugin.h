#pragma once
//------------------------------------------------------------------------------
/**
	@class Silhouette::SilhouetteRTPlugin
	
	The SilhouetteRTPlugin renders silhouettes of provided model entities
	
	(C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "rendermodules/rt/rtplugin.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/shaderstate.h"
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
	void OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch);
	/// called when rendering a frame batch
	void OnRender(const Util::StringAtom& filter);

	/// set if grid should be visible
	void SetVisible(bool b);
	/// set models for a specific group to be rendered with colour. if the group doesnt exist it will be added
	void SetModels(const Util::String& group, const Util::Array<Ptr<Graphics::ModelEntity>>& mdls, const Math::float4& colour);	
	/// clear a group and remove it from rendering
	void ClearModelGroup(const Util::String& group);

private:
	bool visible;

	// model tables
	Ptr<Graphics::ModelEntity> model;
	Util::Dictionary<Util::String, Util::KeyValuePair<Math::float4, Util::Array<Ptr<Graphics::ModelEntity>>>> models;

	// shader
	Ptr<CoreGraphics::ShaderState> shader;
	Ptr<CoreGraphics::ShaderVariable> colorVar;
	CoreGraphics::ShaderFeature::Mask prepassVariation;	
	CoreGraphics::ShaderFeature::Mask outlineVariation;	
};

//------------------------------------------------------------------------------
/**
*/
inline void
SilhouetteRTPlugin::SetVisible(bool b)
{
	this->visible = b;
}

} // namespace Silhouette
