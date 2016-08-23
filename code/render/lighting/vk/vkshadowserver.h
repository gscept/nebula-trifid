#pragma once
//------------------------------------------------------------------------------
/**
    @class Lighting::VkShadowServer
    
    Vulkan shadow server.

	Implements Moment shadow maps for global lights, http://cg.cs.uni-bonn.de/en/publications/paper-details/peters-2015-msm/
	Variance shadow maps for local lights (spot, point), http://developer.download.nvidia.com/SDK/10/direct3d/Source/VarianceShadowMapping/Doc/VarianceShadowMapping.pdf
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
#include "lighting/base/shadowserverbase.h"
#include "frame/frameshader.h"
#include "frame/frameposteffect.h"
#include "coregraphics/rendertarget.h"
#include "lighting/pssmutil.h"
#include "lighting/csmutil.h"
#include "frame/framepass.h"

//------------------------------------------------------------------------------
namespace Lighting
{
class VkShadowServer : public ShadowServerBase
{
	__DeclareClass(VkShadowServer);
public:
	/// constructor
	VkShadowServer();
	/// destructor
	virtual ~VkShadowServer();

	/// open the shadow server
	void Open();
	/// close the shadow server
	void Close();

	/// update shadow buffer
	void UpdateShadowBuffers();

	/// get pointer to shadow buffer for local lights
	const Ptr<CoreGraphics::Texture>& GetSpotLightShadowBufferTexture() const;
	/// get pointer to PSSM shadow buffer for global lights
	const Ptr<CoreGraphics::Texture>& GetGlobalLightShadowBufferTexture() const;
	/// get array of PSSM split distances
	const float* GetSplitDistances() const;
	/// get array of PSSM LightProjTransforms
	const Math::matrix44* GetSplitTransforms() const;  
	/// gets CSM shadow view
	const Math::matrix44* GetShadowView() const;
	/// get array of PSSM frustum far plane corners
	const Math::float4* GetFarPlane() const;
	/// get array of PSSM frustum near plane corners
	const Math::float4* GetNearPlane() const;

private:
	/// update spot light shadow buffers
	void UpdateSpotLightShadowBuffers();
	/// update point light shadow buffers
	void UpdatePointLightShadowBuffers();

	/// prepare updating global buffer
	void PrepareGlobalShadowBuffer();
	/// update global light shadow buffers
	void UpdateGlobalShadowBuffer();

	/// sort local lights by priority
	virtual void SortLights();

	// spot light
	Ptr<CoreGraphics::RenderTarget> spotLightShadowMap1;
	Ptr<CoreGraphics::RenderTarget> spotLightShadowMap2;
	Ptr<CoreGraphics::RenderTarget> spotLightShadowBufferAtlas;
	Ptr<CoreGraphics::ShaderState> satXShader;
	Ptr<CoreGraphics::ShaderState> satYShader;
	Ptr<Frame::FramePass> spotLightPass;
	Ptr<Frame::FrameBatch> spotLightBatch;
	Ptr<Frame::FramePostEffect> spotLightHoriPass;
	Ptr<Frame::FramePostEffect> spotLightVertPass;

	// point light
	Ptr<CoreGraphics::ShaderState> shadowShader;
	Ptr<CoreGraphics::RenderTargetCube> pointLightShadowCubes[MaxNumShadowPointLights];
	Ptr<CoreGraphics::RenderTargetCube> pointLightShadowFilterCube;
    Ptr<Frame::FramePass> pointLightPass;
    Ptr<Frame::FrameBatch> pointLightBatch;
	Ptr<CoreGraphics::ShaderVariable> pointLightPosVar;

	Ptr<CoreGraphics::ShaderState> pointLightBlur;
	CoreGraphics::ShaderFeature::Mask xBlurMask;
	CoreGraphics::ShaderFeature::Mask yBlurMask;
	Ptr<CoreGraphics::ShaderVariable> pointLightBlurReadLinear;
	Ptr<CoreGraphics::ShaderVariable> pointLightBlurReadPoint;
	Ptr<CoreGraphics::ShaderVariable> pointLightBlurWrite;

	// global light
	Ptr<Frame::FramePass> globalLightHotPass;
	Ptr<Frame::FramePostEffect> globalLightBlurPass;
	Ptr<Frame::FrameBatch> globalLightShadowBatch;
	Ptr<CoreGraphics::RenderTarget> globalLightShadowBuffer;
	Ptr<CoreGraphics::RenderTarget> globalLightShadowBufferFinal;

	// generic stuff
	Ptr<CoreGraphics::ShaderState> blurShader;
	Ptr<CoreGraphics::ShaderVariable> shadowCascadeViewVar;
	PSSMUtil pssmUtil;
	CSMUtil csmUtil;

	_declare_timer(globalShadow);
	_declare_timer(pointLightShadow);
	_declare_timer(spotLightShadow);
}; 

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
VkShadowServer::GetSpotLightShadowBufferTexture() const
{
	return this->spotLightShadowBufferAtlas->GetResolveTexture();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>& 
VkShadowServer::GetGlobalLightShadowBufferTexture() const
{
	return this->globalLightShadowBufferFinal->GetResolveTexture();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4* 
VkShadowServer::GetFarPlane() const
{
	return this->pssmUtil.GetFarPlane();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4* 
VkShadowServer::GetNearPlane() const
{
	return this->pssmUtil.GetNearPlane();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44*
VkShadowServer::GetShadowView() const
{
	return &this->csmUtil.GetShadowView();
}

//------------------------------------------------------------------------------
/**
    Get raw pointer to array of PSSM split distances.
*/
inline const float*
VkShadowServer::GetSplitDistances() const
{
    return this->csmUtil.GetCascadeDistances();
}

//------------------------------------------------------------------------------
/**
    Get raw pointer to array of PSSM split LightProjTransform matrices.
*/
inline const Math::matrix44*
VkShadowServer::GetSplitTransforms() const
{
    return this->csmUtil.GetCascadeTransforms();
}

} // namespace Lighting
//------------------------------------------------------------------------------
