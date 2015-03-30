#pragma once
//------------------------------------------------------------------------------
/**
    @class Lighting::SM50LightServer
    
    A newer version of the light prepass server, used for deferred lighting
    
    (C) 2011-2013 Individual contributors, see AUTHORS file
*/
#include "lighting/base/lightserverbase.h"
#include "graphics/pointlightentity.h"
#include "graphics/spotlightentity.h"
#include "coregraphics/shaderinstance.h"
#include "resources/managedmesh.h"
#include "resources/managedtexture.h"
#include "renderutil/drawfullscreenquad.h"
#include "coregraphics/rendertarget.h"
#include "frame/frameserver.h"
#include "renderutil/drawquad.h"
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace Lighting
{
class SM50LightServer : public LightServerBase
{
	__DeclareClass(SM50LightServer);
public:
	/// constructor
	SM50LightServer();
	/// destructor
	virtual ~SM50LightServer();

	/// open the light server
	void Open();
	/// close the light server
	void Close();
	/// returns if the server needs model-light linking
	bool NeedsLightModelLinking() const;

	/// attach a visible light source
	void AttachVisibleLight(const Ptr<Graphics::AbstractLightEntity>& lightEntity);
	/// end lighting frame
	void EndFrame();
	/// render lights
	void RenderLights();
	/// render light probes
	void RenderLightProbes();
	
protected:

	/// render the global light
	void RenderGlobalLight();
	/// render all point lights
	void RenderPointLights();
	/// render all spot lights
	void RenderSpotLights();
	/// assign render buffers to shaders
	void AssignRenderBufferTextures();

	enum ShadowFlag
	{
		CastShadows = 0,
		NoShadows,

		NumShadowFlags
	};


	Util::Array<Ptr<Graphics::PointLightEntity> > pointLights[NumShadowFlags];
	Util::Array<Ptr<Graphics::SpotLightEntity> > spotLights[NumShadowFlags];

	CoreGraphics::ShaderFeature::Mask pointLightFeatureBits[NumShadowFlags];
	CoreGraphics::ShaderFeature::Mask spotLightFeatureBits[NumShadowFlags];
	CoreGraphics::ShaderFeature::Mask globalLightFeatureBits[NumShadowFlags];
	CoreGraphics::ShaderFeature::Mask lightProbeFeatureBits[Graphics::LightProbeEntity::NumProbeShapeTypes + 2];
	RenderUtil::DrawFullScreenQuad fullScreenQuadRenderer;          // fs quad renderer

	Ptr<Frame::FrameShader> godRayFrameShader;

	Ptr<CoreGraphics::ShaderInstance> lightShader;
	Ptr<CoreGraphics::ShaderInstance> lightProbeShader;
	Ptr<Resources::ManagedTexture> lightProjMap; 

	/// global light variables
	Ptr<CoreGraphics::ShaderVariable> globalLightFocalLength;
	Ptr<CoreGraphics::ShaderVariable> globalLightDir;
	Ptr<CoreGraphics::ShaderVariable> globalLightColor;
	Ptr<CoreGraphics::ShaderVariable> globalBackLightColor;
	Ptr<CoreGraphics::ShaderVariable> globalAmbientLightColor;
	Ptr<CoreGraphics::ShaderVariable> globalBackLightOffset;

	Ptr<CoreGraphics::ShaderVariable> globalLightShadow;
	Ptr<CoreGraphics::ShaderVariable> globalLightCascadeOffset;
	Ptr<CoreGraphics::ShaderVariable> globalLightCascadeScale;
	Ptr<CoreGraphics::ShaderVariable> globalLightMinBorderPadding;
	Ptr<CoreGraphics::ShaderVariable> globalLightMaxBorderPadding;
	Ptr<CoreGraphics::ShaderVariable> globalLightShadowBias;
	Ptr<CoreGraphics::ShaderVariable> globalLightPartitionSize;

	/// global light textures
	Ptr<CoreGraphics::ShaderVariable> normalBufferVar;
	Ptr<CoreGraphics::ShaderVariable> depthBufferVar;
	Ptr<CoreGraphics::ShaderVariable> lightBufferVar;       
	Ptr<CoreGraphics::ShaderVariable> lightProjMapVar;
	Ptr<CoreGraphics::ShaderVariable> lightProjCubeVar;
	Ptr<CoreGraphics::ShaderVariable> shadowProjMapVar;     
    Ptr<CoreGraphics::ShaderVariable> shadowProjCubeVar;

	/// light variables
	Ptr<CoreGraphics::ShaderVariable> lightPosRange;
	Ptr<CoreGraphics::ShaderVariable> lightColor;
	Ptr<CoreGraphics::ShaderVariable> lightProjTransform;
	Ptr<CoreGraphics::ShaderVariable> lightShadowBias;

	/// shadow variables
	Ptr<CoreGraphics::ShaderVariable> shadowConstants;
	Ptr<CoreGraphics::ShaderVariable> shadowIntensityVar;
	Ptr<CoreGraphics::ShaderVariable> shadowProjTransform;
	Ptr<CoreGraphics::ShaderVariable> shadowOffsetScaleVar;

	// light probe variables
	Ptr<CoreGraphics::ShaderVariable> lightProbeReflectionVar;
	Ptr<CoreGraphics::ShaderVariable> lightProbeIrradianceVar;
	Ptr<CoreGraphics::ShaderVariable> lightProbeFalloffVar;
	Ptr<CoreGraphics::ShaderVariable> lightProbePowerVar;
	Ptr<CoreGraphics::ShaderVariable> lightProbeReflectionNumMipsVar;
	Ptr<CoreGraphics::ShaderVariable> lightProbeBboxMinVar;
	Ptr<CoreGraphics::ShaderVariable> lightProbeBboxMaxVar;
	Ptr<CoreGraphics::ShaderVariable> lightProbeBboxCenterVar;

	Ptr<Resources::ManagedMesh> pointLightMesh;         // point light mesh
	Ptr<Resources::ManagedMesh> spotLightMesh;          // spot light mesh
	Ptr<Resources::ManagedMesh> lightProbeMesh;
	bool renderBuffersAssigned;     

}; 
} // namespace Lighting
//------------------------------------------------------------------------------