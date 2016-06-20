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
#include "coregraphics/shaderinstance.h"
#include "coregraphics/texture.h"
#include "resources/managedtexture.h"
namespace Clouds
{
class CloudsRTPlugin : public RenderModules::RTPlugin
{
	__DeclareClass(CloudsRTPlugin);
public:

	struct CloudSettings
	{
		float size;
		float thickness;
		Resources::ResourceId tex;
	};

	/// constructor
	CloudsRTPlugin();
	/// destructor
	virtual ~CloudsRTPlugin();

	/// called when plugin is registered on the render-thread side
	virtual void OnRegister();
	/// called when plugin is unregistered on the render-thread side
	virtual void OnUnregister();

	/// called when rendering a frame batch
	virtual void OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch);

	/// set if clouds should be visible
	void SetVisible(bool b);
	/// set cloud settings at index, which is valid between 0-2
	void SetCloudSettings(const CloudSettings& settings, IndexT index);

	static const SizeT NumCloudLayers = 3;
private:
	bool visible;
	float cloudSize;
	float cloudThickness;
	Math::float4 cloudThickColor;

	CloudSettings cloudSettings[NumCloudLayers];

	// mesh
	CoreGraphics::PrimitiveGroup primitive;
	Ptr<CoreGraphics::VertexBuffer> vbo;
	Ptr<CoreGraphics::IndexBuffer> ibo;
	Ptr<Resources::ManagedTexture> textures[NumCloudLayers];
	Frame::BatchGroup::Code group;

	// shader
	Ptr<CoreGraphics::Shader> shader;
	Ptr<CoreGraphics::ShaderVariable> cloudsSizesVar;
	Ptr<CoreGraphics::ShaderVariable> cloudsLayer1TexVar;
	Ptr<CoreGraphics::ShaderVariable> cloudsLayer2TexVar;
	Ptr<CoreGraphics::ShaderVariable> cloudsLayer3TexVar;
	Ptr<CoreGraphics::ShaderVariable> cloudThicknessesVar;
	Ptr<CoreGraphics::ShaderVariable> cloudThickColorVar;
};

//------------------------------------------------------------------------------
/**
*/
inline void
CloudsRTPlugin::SetVisible(bool b)
{
	this->visible = b;
}

} // namespace Grid