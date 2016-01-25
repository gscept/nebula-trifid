#pragma once
//------------------------------------------------------------------------------
/**
    @class Algorithm::VolumetricLightAlgorithm
    
    Renders the light sources which should be rendered volumetrically.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "algorithmbase.h"
#include "coregraphics/shaderfeature.h"
#include "graphics/globallightentity.h"
#include "graphics/pointlightentity.h"
#include "graphics/spotlightentity.h"
#include "resources/managedmesh.h"
#include "renderutil/drawfullscreenquad.h"
#include "debug/debugtimer.h"
namespace Algorithm
{
class VolumetricLightAlgorithm : public AlgorithmBase
{
	__DeclareClass(VolumetricLightAlgorithm);
public:
	/// constructor
	VolumetricLightAlgorithm();
	/// destructor
	virtual ~VolumetricLightAlgorithm();

	/// setup godrays
	void Setup();
	/// discard godrays
	void Discard();

	/// renders godrays
	void Render();	

	/// resize rendertargets
	void OnDisplayResized(SizeT width, SizeT height);

	/// handles messages
	bool HandleMessage(const Ptr<Messaging::Message>& msg);

	static const int VolumeLightsPerRow = 4;
	static const int VolumeLightsPerColumn = 4;
	static const int MaxNumVolumetricLights = 16;

private:

	/// calculate screen-space center of transform
	Math::float2 CalculateScreenSpaceCenter(const Math::float4& worldPos, const Math::matrix44& viewProj);
	/// render light scattering to secondary  buffer
	void Scatter(const Math::float2& lightPos);

	/// renders global light
	void RenderGlobalLight();

	RenderUtil::DrawFullScreenQuad quad;
	Ptr<CoreGraphics::Shader> volumeLightShader;
	Ptr<CoreGraphics::Shader> lightScatterShader;
	Ptr<CoreGraphics::Shader> verticalBloom;
	Ptr<CoreGraphics::Shader> horizontalBloom;

	Ptr<Graphics::GlobalLightEntity> globalLight;
	Util::Array<Ptr<Graphics::PointLightEntity> > pointLights;
	Util::Array<Ptr<Graphics::SpotLightEntity> > spotLights;

	CoreGraphics::ShaderFeature::Mask pointLightFeatureBits;
	CoreGraphics::ShaderFeature::Mask spotLightFeatureBits;
	CoreGraphics::ShaderFeature::Mask globalLightFeatureBits;
	CoreGraphics::ShaderFeature::Mask globalLightMeshFeatureBits;

	CoreGraphics::ShaderFeature::Mask globalScatterFeatureBits;

	Ptr<CoreGraphics::ShaderVariable> lightScatterLightPosVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterDensityVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterDecayVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterWeightVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterExposureVar;

	Ptr<CoreGraphics::ShaderVariable> volumeUnshadedTextureVar;
	Ptr<CoreGraphics::ShaderVariable> lightScatterUnshadedTextureVar;
	Ptr<CoreGraphics::ShaderVariable> godrayTexVar;
	Ptr<CoreGraphics::ShaderVariable> bloomedTexVar;

	Ptr<CoreGraphics::RenderTarget> albedoRenderTarget;
	Ptr<CoreGraphics::RenderTarget> volumeLightBuffer1;
	Ptr<CoreGraphics::RenderTarget> volumeLightBuffer2;
	Ptr<CoreGraphics::RenderTarget> output;

	Ptr<Resources::ManagedTexture> whiteMap;
	Ptr<Resources::ManagedTexture> currentTexture;

	Ptr<CoreGraphics::ShaderVariable> lightScaleVar;
	Ptr<CoreGraphics::ShaderVariable> lightIntensityVar;
	Ptr<CoreGraphics::ShaderVariable> lightProjMapVar;
	Ptr<CoreGraphics::ShaderVariable> lightProjCubeVar;
	Ptr<CoreGraphics::ShaderVariable> lightColorVar;
    Ptr<CoreGraphics::ShaderVariable> lightTransformVar;
	Ptr<CoreGraphics::ShaderVariable> lightCenterPosVar;

	Ptr<Resources::ManagedMesh> globalLightMesh;
	Ptr<Resources::ManagedMesh> pointLightMesh;         // point light mesh
	Ptr<Resources::ManagedMesh> spotLightMesh;          // spot light mesh

	_declare_timer(VolumetricLightTimer);

}; 

__RegisterClass(VolumetricLightAlgorithm);
} // namespace Algorithm
//------------------------------------------------------------------------------