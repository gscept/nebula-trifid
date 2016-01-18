#pragma once
//------------------------------------------------------------------------------
/**
    @class Lighting::SM50ShadowServer
    
    Handles shadowing using SM 5.0
    
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
class SM50ShadowServer : public ShadowServerBase
{
	__DeclareClass(SM50ShadowServer);
public:
	/// constructor
	SM50ShadowServer();
	/// destructor
	virtual ~SM50ShadowServer();

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
	void UpdateHotGlobalShadowBuffer();
	/// update static-only shadow buffer
	void UpdateColdGlobalShadowBuffer();

	/// sort local lights by priority
	virtual void SortLights();

	// spot light
	Ptr<CoreGraphics::RenderTarget> spotLightShadowMap1;
	Ptr<CoreGraphics::RenderTarget> spotLightShadowMap2;
	Ptr<CoreGraphics::RenderTarget> spotLightShadowBufferAtlas;
	Ptr<CoreGraphics::Shader> satXShader;
	Ptr<CoreGraphics::Shader> satYShader;
	Ptr<Frame::FramePass> spotLightPass;
	Ptr<Frame::FrameBatch> spotLightBatch;
	Ptr<Frame::FramePostEffect> spotLightHoriPass;
	Ptr<Frame::FramePostEffect> spotLightVertPass;

	// point light
	Ptr<CoreGraphics::RenderTargetCube> pointLightShadowCubes[MaxNumShadowPointLights];
    Ptr<Frame::FramePass> pointLightPass;
    Ptr<Frame::FrameBatch> pointLightBatch;

	// global light
	Ptr<Frame::FramePass> globalLightHotPass;
	Ptr<Frame::FramePostEffect> globalLightBlurPass;
	Ptr<Frame::FrameBatch> globalLightShadowBatch;
	Ptr<CoreGraphics::RenderTarget> globalLightShadowBuffer;
	Ptr<CoreGraphics::RenderTarget> globalLightShadowBufferFinal;

	// generic stuff
	Ptr<CoreGraphics::Shader> blurShader;
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
SM50ShadowServer::GetSpotLightShadowBufferTexture() const
{
	return this->spotLightShadowBufferAtlas->GetResolveTexture();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>& 
SM50ShadowServer::GetGlobalLightShadowBufferTexture() const
{
	return this->globalLightShadowBufferFinal->GetResolveTexture();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4* 
SM50ShadowServer::GetFarPlane() const
{
	return this->pssmUtil.GetFarPlane();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4* 
SM50ShadowServer::GetNearPlane() const
{
	return this->pssmUtil.GetNearPlane();
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44*
SM50ShadowServer::GetShadowView() const
{
	return &this->csmUtil.GetShadowView();
}

//------------------------------------------------------------------------------
/**
    Get raw pointer to array of PSSM split distances.
*/
inline const float*
SM50ShadowServer::GetSplitDistances() const
{
    return this->csmUtil.GetCascadeDistances();
}

//------------------------------------------------------------------------------
/**
    Get raw pointer to array of PSSM split LightProjTransform matrices.
*/
inline const Math::matrix44*
SM50ShadowServer::GetSplitTransforms() const
{
    return this->csmUtil.GetCascadeTransforms();
}

} // namespace Lighting
//------------------------------------------------------------------------------
