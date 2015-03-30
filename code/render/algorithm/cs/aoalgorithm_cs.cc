//------------------------------------------------------------------------------
//  aoalgorithm.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "aoalgorithm.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/memorytextureloader.h"
#include "resources/resourceloader.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shadersemantics.h"
#include "graphics/graphicsserver.h"
#include "resources/resourcemanager.h"
#include "coregraphics/base/rendertargetbase.h"

#define MAX_RADIUS_PIXELS 0.5f
#define DivAndRoundUp(a, b) (a % b != 0) ? (a / b + 1) : (a / b)

using namespace Math;
using namespace CoreGraphics;
using namespace Graphics;
namespace Algorithm
{
__ImplementClass(Algorithm::AOAlgorithm, 'ALAO', Algorithm::AlgorithmBase);


//------------------------------------------------------------------------------
/**
*/
AOAlgorithm::AOAlgorithm() :
	downSample(1)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AOAlgorithm::~AOAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Setup()
{
	n_assert(this->inputs.Size() == 1);
	n_assert(this->outputNames.Size() == 1);
	n_assert(this->inputs[0].isvalid());

	AlgorithmBase::Setup();
	
	// setup output render target
	this->output = RenderTarget::Create();
	this->output->SetResolveTextureResourceId(this->outputNames[0]);
    this->output->SetRelativeWidth(1);
    this->output->SetRelativeHeight(1);
	this->output->SetAntiAliasQuality(AntiAliasQuality::None);
	this->output->SetColorBufferFormat(PixelFormat::R8);
	this->output->SetClearColor(float4(0,0,0,0));
	this->output->SetClearFlags(Base::RenderTargetBase::ClearColor);
	this->output->Setup();

    // setup internal targets
    IndexT targetId = 0;
    IndexT numTarget;
    for (numTarget = 0; numTarget < NumInternalRTargets; numTarget++)
    {
        this->internalTargetsR[numTarget] = RenderTarget::Create();
        this->internalTargetsR[numTarget]->SetResolveTextureResourceId("SSAOInternalBuffer" + Util::String::FromInt(targetId++));
        this->internalTargetsR[numTarget]->SetRelativeWidth(1);
        this->internalTargetsR[numTarget]->SetRelativeHeight(1);
        this->internalTargetsR[numTarget]->SetAntiAliasQuality(AntiAliasQuality::None);
        this->internalTargetsR[numTarget]->SetColorBufferFormat(PixelFormat::R16F);
        this->internalTargetsR[numTarget]->Setup();
    }

    for (numTarget = 0; numTarget < NumInternalRGTargets; numTarget++)
    {
        this->internalTargetsRG[numTarget] = RenderTarget::Create();
        this->internalTargetsRG[numTarget]->SetResolveTextureResourceId("SSAOInternalBuffer" + Util::String::FromInt(targetId++));
        this->internalTargetsRG[numTarget]->SetRelativeWidth(1);
        this->internalTargetsRG[numTarget]->SetRelativeHeight(1);
        this->internalTargetsRG[numTarget]->SetAntiAliasQuality(AntiAliasQuality::None);
        this->internalTargetsRG[numTarget]->SetColorBufferFormat(PixelFormat::G16R16F);
        this->internalTargetsRG[numTarget]->Setup();
    }

	// setup shaders
	this->hbao = ShaderServer::Instance()->CreateShaderInstance("shd:hbao_cs");
	this->blur = ShaderServer::Instance()->CreateShaderInstance("shd:hbaoblur_cs");
    this->xDimShaderFeature = ShaderServer::Instance()->FeatureStringToMask("X");
    this->yDimShaderFeature = ShaderServer::Instance()->FeatureStringToMask("Y");

	// setup variables for HBAO
	this->depthTextureVar = this->hbao->GetVariableByName("DepthBuffer");
    this->hbaoOutput0 = this->hbao->GetVariableByName("HBAO0");
    this->hbaoOutput1 = this->hbao->GetVariableByName("HBAO1");

    // setup variables for blur
	this->hbaoTextureVar = this->blur->GetVariableByName("HBAOBuffer");
    this->hbaoBlurredTextureVar = this->blur->GetVariableByName("HBAOBlurred");
    this->hbaoFinalTextureVar = this->blur->GetVariableByName("HBAOFinal");

	this->fullWidth = (float)this->output->GetWidth();
	this->fullHeight = (float)this->output->GetHeight();
	this->radius = 12.0f;

	vars.tanAngleBias = tanf(n_deg2rad(10.0));
	vars.strength = 2.0f;

	this->uvToViewAVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_UVTOVIEWA);
	this->uvToViewBVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_UVTOVIEWB);
	this->r2Var = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_R2);
	this->aoResolutionVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_AORESOLUTION);
	this->invAOResolutionVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_INVAORESOLUTION);
	this->strengthVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_STRENGHT);
	this->tanAngleBiasVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_TANANGLEBIAS);
	this->powerExponentVar = this->blur->GetVariableBySemantic(NEBULA3_SEMANTIC_POWEREXPONENT);
	this->blurFalloff = this->blur->GetVariableBySemantic(NEBULA3_SEMANTIC_FALLOFF);
	this->blurDepthThreshold = this->blur->GetVariableBySemantic(NEBULA3_SEMANTIC_DEPTHTHRESHOLD);
	this->powerExponentVar->SetFloat(1.0f);

    // setup depth texture
    this->depthTextureVar->SetTexture(this->inputs[0]);

	// we need to late setup this algorithm since we need the camera
	this->deferredSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Discard()
{
	AlgorithmBase::Discard();

	// unload shaders
	this->hbao->Discard();
	this->hbao = 0;
	this->blur->Discard();
	this->blur = 0;

	// unload render targets
    IndexT i;
    for (i = 0; i < NumInternalRTargets; i++)
    {
        this->internalTargetsR[i]->Discard();
        this->internalTargetsR[i] = 0;
    }
    for (i = 0; i < NumInternalRGTargets; i++)
    {
        this->internalTargetsRG[i]->Discard();
        this->internalTargetsRG[i] = 0;
    }
	this->output->Discard();
	this->output = 0;

	// free variables
	this->depthTextureVar = 0;
    this->hbaoOutput0 = 0;
    this->hbaoOutput1 = 0;
	this->hbaoTextureVar = 0;
	this->uvToViewAVar = 0;
	this->uvToViewBVar = 0;
	this->r2Var = 0;
	this->aoResolutionVar = 0;
	this->invAOResolutionVar = 0;
	this->strengthVar = 0;
	this->tanAngleBiasVar = 0;
	this->powerExponentVar = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Prepare()
{
	if (this->deferredSetup)
	{
		// calculate variables
		this->Calculate();

		// set variables
		this->uvToViewAVar->SetFloat2(vars.uvToViewA);
		this->uvToViewBVar->SetFloat2(vars.uvToViewB);

		this->r2Var->SetFloat(vars.r2);
		this->aoResolutionVar->SetFloat2(vars.aoResolution);
		this->invAOResolutionVar->SetFloat2(vars.invAOResolution);
		this->strengthVar->SetFloat(vars.strength);
		this->tanAngleBiasVar->SetFloat(vars.tanAngleBias);

		// set blur vars
		this->blurFalloff->SetFloat(vars.blurFalloff);
		this->blurDepthThreshold->SetFloat(vars.blurThreshold);

		// we have performed our deferred setup, so set flag to false to avoid this happening again
		this->deferredSetup = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Execute()
{
	// if enabled, render AO
	if (this->enabled)
	{
		RenderDevice* renderDevice = RenderDevice::Instance();
		ShaderServer* shaderServer = ShaderServer::Instance();

        // this define must match the ROW_TILE_WIDTH and HBAO_TILE_WIDTH in the hbaoblur_cs/hbao_cs shaders
#define TILE_WIDTH 320

        // calculate execution dimensions
        uint numGroupsX1 = DivAndRoundUp(this->output->GetWidth(), TILE_WIDTH);
        uint numGroupsX2 = this->output->GetWidth();
        uint numGroupsY1 = DivAndRoundUp(this->output->GetHeight(), TILE_WIDTH);
        uint numGroupsY2 = this->output->GetHeight();        
		
        // setup targets
        this->hbaoOutput0->SetTexture(this->internalTargetsR[0]->GetResolveTexture());
        this->hbaoOutput1->SetTexture(this->internalTargetsRG[0]->GetResolveTexture());
        this->hbaoBlurredTextureVar->SetTexture(this->internalTargetsRG[1]->GetResolveTexture());
        this->hbaoFinalTextureVar->SetTexture(this->output->GetResolveTexture());

		// first render hbao in X
        this->hbao->SelectActiveVariation(this->xDimShaderFeature);
        this->hbao->Begin();
        this->hbao->BeginPass(0);
        this->hbao->Commit();
        renderDevice->Compute(numGroupsX1, numGroupsY2, 1);
        this->hbao->EndPass();
        this->hbao->End();

        // then in Y
        this->hbao->SelectActiveVariation(this->yDimShaderFeature);
        this->hbao->Begin();
        this->hbao->BeginPass(0);
        this->hbao->Commit();
        renderDevice->Compute(numGroupsY1, numGroupsX2, 1);
        this->hbao->EndPass();
        this->hbao->End();

		// now render first x-blur
        this->blur->SelectActiveVariation(this->xDimShaderFeature);
        this->blur->Begin();
        this->blur->BeginPass(0);
        this->hbaoTextureVar->SetTexture(this->internalTargetsRG[0]->GetResolveTexture());
        this->blur->Commit();
        renderDevice->Compute(numGroupsX1, numGroupsY2, 1);
        this->blur->EndPass();
        this->blur->End();

        this->blur->SelectActiveVariation(this->yDimShaderFeature);
        this->blur->Begin();
        this->blur->BeginPass(0);
        this->hbaoTextureVar->SetTexture(this->internalTargetsRG[1]->GetResolveTexture());
        this->blur->Commit();
        renderDevice->Compute(numGroupsY1, numGroupsX2, 1);
        this->blur->EndPass();
        this->blur->End();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::OnDisplayResize()
{
    // resize output
    this->output->OnDisplayResized();

    // setup internal targets
    IndexT numTarget;
    for (numTarget = 0; numTarget < NumInternalRTargets; numTarget++)
    {
        this->internalTargetsR[numTarget]->OnDisplayResized();
    }

    for (numTarget = 0; numTarget < NumInternalRGTargets; numTarget++)
    {
        this->internalTargetsRG[numTarget]->OnDisplayResized();
    }

	// calculate new dimensions for render target
	this->fullWidth = (float)output->GetWidth();
	this->fullHeight = (float)output->GetHeight();

    // set depth texture since it's been resized
    this->depthTextureVar->SetTexture(this->inputs[0]);

	// trigger a new setup
	this->deferredSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Calculate()
{
	// get camera settings
	const CameraSettings& cameraSettings = Graphics::GraphicsServer::Instance()->GetCurrentView()->GetCameraEntity()->GetCameraSettings();

	this->width = this->fullWidth / downSample;
	this->height = this->fullHeight / downSample;

	this->nearZ = cameraSettings.GetZNear();
	this->farZ = cameraSettings.GetZFar();

	this->sceneScale = n_min(this->nearZ, this->farZ);
	float r = this->radius * 4.0f / 100.0f;
	vars.r2 = r * r;

	vars.aoResolution.x() = this->fullWidth;
	vars.aoResolution.y() = this->fullHeight;
	vars.invAOResolution.x() = 1.0f / this->width;
	vars.invAOResolution.y() = 1.0f / this->height;

	float fov = cameraSettings.GetFov();
	float focalX = 1.0f / tanf(fov * 0.5f) * (this->fullHeight / this->fullWidth);
	float focalY = 1.0f / tanf(fov * 0.5f);

	float2 invFocalLength;
	invFocalLength.x() = 1 / focalX;
	invFocalLength.y() = 1 / focalY;

	vars.uvToViewA.x() = 2.0f * invFocalLength.x();
	vars.uvToViewA.y() = -2.0f * invFocalLength.y();
	vars.uvToViewB.x() = -1.0f * invFocalLength.x();
	vars.uvToViewB.y() = 1.0f * invFocalLength.y();

#ifndef INV_LN2
#define INV_LN2 1.44269504f
#endif

#ifndef SQRT_LN2
#define SQRT_LN2 0.832554611f
#endif

#define BLUR_RADIUS 33
#define BLUR_SHARPNESS 8

	float blurSigma = (BLUR_RADIUS + 1) * 0.5f;
	vars.blurFalloff = INV_LN2 / (2.0f * blurSigma * blurSigma);
	vars.blurThreshold = 2.0f * SQRT_LN2 * (this->sceneScale / BLUR_SHARPNESS);
}

//------------------------------------------------------------------------------
/**
*/
bool 
AOAlgorithm::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	if (msg->CheckId(SetAmbientOcclusionParams::Id))
	{
		Ptr<SetAmbientOcclusionParams> rMsg = msg.downcast<SetAmbientOcclusionParams>();

		// update variables set directly
		this->strengthVar->SetFloat(rMsg->GetStrength());
		this->tanAngleBiasVar->SetFloat(tanf(n_deg2rad(rMsg->GetAngleBias())));
		this->powerExponentVar->SetFloat(rMsg->GetPowerExponent());		
        this->radius = rMsg->GetRadius();		
		
		// radius needs the r2
		this->Calculate();

		// set the newly calculated variables
		this->r2Var->SetFloat(vars.r2);

		return true;
	}
	else if (msg->CheckId(EnableAmbientOcclusion::Id))
	{
		Ptr<EnableAmbientOcclusion> rMsg = msg.downcast<EnableAmbientOcclusion>();
		this->SetEnabled(rMsg->GetEnabled());
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::SetEnabled( bool b )
{
	AlgorithmBase::SetEnabled(b);
	if (b)
	{
		this->Enable();
	}
	else
	{
		this->Disable();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Disable()
{
	RenderDevice* renderDevice = RenderDevice::Instance();
    this->output->Clear(RenderTarget::ClearColor);
}

} // namespace Algorithm