//------------------------------------------------------------------------------
//  vrview.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "openvr.h"
#include "vrview.h"
#include "particles/particlerenderer.h"
#include "coregraphics/renderdevice.h"
#include "lighting/lightserver.h"
#include "lighting/shadowserver.h"
#include "models/modelserver.h"
#include "vrmanager.h"
#include "vivemote.h"
#include "coregraphics/transformdevice.h"

using namespace CoreGraphics;
using namespace Lighting;
using namespace Models;
namespace VR
{
__ImplementClass(VR::VrView, 'VRVW', Graphics::View);

//------------------------------------------------------------------------------
/**
*/
VrView::VrView()
{

}

//------------------------------------------------------------------------------
/**
*/
VrView::~VrView()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VrView::Render(IndexT frameIndex)
{
    n_assert(this->frameShader.isvalid());
    n_assert(this->camera.isvalid());
   
    this->camera->SetCameraSettings(this->camSettings[VrView::LeftCamera]);
    this->ApplyCameraSettings();

    // reset model node instance index for a new render frame (view)
    ModelServer::Instance()->ResetModelNodeInstanceIndex();
    RenderDevice* renderDev = RenderDevice::Instance();
    LightServer* lightServer = LightServer::Instance();
    ShadowServer* shadowServer = ShadowServer::Instance();

    lightServer->BeginFrame(this->camera);
    shadowServer->BeginFrame(this->camera);
    this->frameShader->Begin();

    Particles::ParticleRenderer::Instance()->BeginAttach();

    _start_timer(resolveVisibleShadowCasters);
    // resolve visible light source
    this->ResolveVisibleLights(frameIndex);

    // resolve visible shadow casters for global light
    //this->ResolveVisibleShadowCasters(frameIndex);
    _stop_timer(resolveVisibleShadowCasters);

    _start_timer(updateShadowBuffers);
    // update local shadows
    shadowServer->UpdateShadowBuffers();
    _stop_timer(updateShadowBuffers);

    _start_timer(resolveVisibleModelNodeInstances);
    // resolve visible ModelNodeInstances
    this->ResolveVisibleModelNodeInstances(frameIndex);
    _stop_timer(resolveVisibleModelNodeInstances);

    Particles::ParticleRenderer::Instance()->EndAttach();

    // if we have a resolve rect, we set the resolve rectangle for the default render target
    // this will cause any frame shaders aimed at the default render target (screen) to be rendered to a subregion of the screen
    if (this->resolveRectValid)	renderDev->GetDefaultRenderTarget()->SetResolveRect(this->resolveRect);
    else						renderDev->GetDefaultRenderTarget()->ResetResolveRects();

    // get last pass
    this->lastPass = this->frameShader->GetAllFramePassBases().Back();

        
    // render the world...
    _start_timer(render);
#ifdef CAMSET
    Math::matrix44 trans = this->camera->GetTransform();
    Math::matrix44 leftTrans = Math::matrix44::multiply(this->leftEyeDisplace, trans);    
    this->camera->SetTransform(leftTrans);
    this->camera->SetCameraSettings(this->camSettings[LeftCamera]);
    this->ApplyCameraSettings();

#else
    // render left eye
    TransformDevice* transformDevice = TransformDevice::Instance();
    transformDevice->SetProjTransform(this->leftEyeProj);
    transformDevice->SetViewTransform(Math::matrix44::multiply(this->camera->GetViewTransform(), this->leftEyeDisplace));

    transformDevice->SetFocalLength(this->camera->GetCameraSettings().GetFocalLength());
    transformDevice->ApplyViewSettings();
#endif
    this->lastPass->SetRenderTarget(this->leftEyeTarget);
    this->frameShader->Render(frameIndex);
    glFinish();
#ifdef CAMSET
    Math::matrix44 rightTrans = Math::matrix44::multiply(this->rightEyeDisplace, trans);
    //this->camera->SetTransform(rightTrans);
    //this->camera->SetCameraSettings(this->camSettings[RightCamera]);
    //this->ApplyCameraSettings();
#else
    // render right eye
    transformDevice->SetProjTransform(this->rightEyeProj);
    transformDevice->SetViewTransform(Math::matrix44::multiply(this->camera->GetViewTransform(), this->rightEyeDisplace));    
    
    transformDevice->SetFocalLength(this->camera->GetCameraSettings().GetFocalLength());
    transformDevice->ApplyViewSettings();
#endif

	this->lastPass->SetRenderTarget(this->rightEyeTarget);
	this->frameShader->Render(frameIndex);
    glFinish();
	_stop_timer(render);

	// render picking
	_start_timer(picking);
	_stop_timer(picking);

	// tell main frame shader, light and shadow servers that rendering is finished
	this->frameShader->End();
	shadowServer->EndFrame();
	lightServer->EndFrame();

    glFinish();

	vr::Texture_t leftEye = { (void*)this->leftEyeTarget->GetTexture(), vr::API_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEye);
    glFinish();
	vr::Texture_t rightEye = { (void*)this->rightEyeTarget->GetTexture(), vr::API_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEye);
    glFinish();


}

//------------------------------------------------------------------------------
/**
*/
void
VrView::OnAttachToServer()
{
	View::OnAttachToServer();

	vr::IVRSystem* hmd = VRManager::Instance()->GetHMD();
	hmd->GetRecommendedRenderTargetSize(&this->HMDWidth, &this->HMDHeight);

	vr::HmdMatrix44_t m1 = hmd->GetProjectionMatrix(vr::Eye_Left, 0.01f, 2500.0f, vr::API_OpenGL);
	this->leftEyeProj.loadu(&m1.m[0][0]);
	this->leftEyeProj = Math::matrix44::transpose(this->leftEyeProj);    
	vr::HmdMatrix44_t m2 = hmd->GetProjectionMatrix(vr::Eye_Right, 0.01f, 2500.0f, vr::API_OpenGL);
	this->rightEyeProj.loadu(&m2.m[0][0]);
	this->rightEyeProj = Math::matrix44::transpose(this->rightEyeProj);
	vr::HmdMatrix34_t m3 = hmd->GetEyeToHeadTransform(vr::Eye_Left);
	for (int j = 0; j < 3; j++)
	{
		this->leftEyeDisplace.row(j).loadu(m3.m[j]);
	}
	this->leftEyeDisplace = Math::matrix44::transpose(this->leftEyeDisplace);
    this->leftEyeDisplace = Math::matrix44::inverse(this->leftEyeDisplace);

	vr::HmdMatrix34_t m4 = hmd->GetEyeToHeadTransform(vr::Eye_Right);
	for (int j = 0; j < 3; j++)
	{
		this->rightEyeDisplace.row(j).loadu(m4.m[j]);
	}
	this->rightEyeDisplace = Math::matrix44::transpose(this->rightEyeDisplace);
    this->rightEyeDisplace = Math::matrix44::inverse(this->rightEyeDisplace);

	this->leftEyeTarget = CoreGraphics::RenderTarget::Create();
	this->leftEyeTarget->SetColorBufferFormat(PixelFormat::SRGBA8);
	this->leftEyeTarget->SetWidth(this->HMDWidth);
	this->leftEyeTarget->SetHeight(this->HMDHeight);
	this->leftEyeTarget->SetAntiAliasQuality(AntiAliasQuality::None);
	this->leftEyeTarget->SetResolveTextureResourceId("VRLeftEye");
	// setting this will display the left eye on the normal window.
	// causes issues with some drivers and openvr, better to use the 
	// mirror view in openvr	
    //this->leftEyeTarget->SetDefaultRenderTarget(true);
	this->leftEyeTarget->Setup();

	this->rightEyeTarget = CoreGraphics::RenderTarget::Create();
	this->rightEyeTarget->SetColorBufferFormat(PixelFormat::SRGBA8);
	this->rightEyeTarget->SetWidth(this->HMDWidth);
	this->rightEyeTarget->SetHeight(this->HMDHeight);
	this->rightEyeTarget->SetAntiAliasQuality(AntiAliasQuality::None);
	this->rightEyeTarget->SetResolveTextureResourceId("VRRightEye");
	this->rightEyeTarget->Setup();


}

//------------------------------------------------------------------------------
/**
*/
void
VrView::OnRemoveFromServer()
{
	this->leftEyeTarget->Discard();
	this->leftEyeTarget = 0;
	this->rightEyeTarget->Discard();
	this->rightEyeTarget = 0;

	View::OnRemoveFromServer();
}

//------------------------------------------------------------------------------
/**
*/
void
VrView::SetFrameShader(const Ptr<Frame::FrameShader>& frameShader)
{
	View::SetFrameShader(frameShader);
    this->frameShader->OnDisplayResized(this->HMDWidth, this->HMDHeight);    
}

} // namespace VR