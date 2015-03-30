//------------------------------------------------------------------------------
//  d3d9transformdevice.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"


#include "coregraphics/win360/d3d9transformdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/shadersemantics.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9TransformDevice, 'D9TD', Base::TransformDeviceBase);
__ImplementSingleton(Direct3D9::D3D9TransformDevice);

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
D3D9TransformDevice::D3D9TransformDevice() :
    mvpSemantic(NEBULA3_SEMANTIC_MODELVIEWPROJECTION),
    modelSemantic(NEBULA3_SEMANTIC_MODEL),
    viewSemantic(NEBULA3_SEMANTIC_VIEW),
    modelViewSemantic(NEBULA3_SEMANTIC_MODELVIEW),
    invModelViewSemantic(NEBULA3_SEMANTIC_INVMODELVIEW),
    invViewSemantic(NEBULA3_SEMANTIC_INVVIEW),
    viewProjSemantic(NEBULA3_SEMANTIC_VIEWPROJECTION),
    eyePosSemantic(NEBULA3_SEMANTIC_EYEPOS),
    projectionSemantic(NEBULA3_SEMANTIC_PROJECTION),
    invProjectionSemantic(NEBULA3_SEMANTIC_INVPROJECTION)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9TransformDevice::~D3D9TransformDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
D3D9TransformDevice::Open()
{
    ShaderServer* shdServer = ShaderServer::Instance();
    this->shdEyePos = shdServer->GetSharedVariableBySemantic(this->eyePosSemantic);
    this->viewMatrix = shdServer->GetSharedVariableBySemantic(this->viewSemantic);
    this->invViewMatrix = shdServer->GetSharedVariableBySemantic(this->invViewSemantic);
    this->projectionMatrix = shdServer->GetSharedVariableBySemantic(this->projectionSemantic);
    this->invProjectionMatrix = shdServer->GetSharedVariableBySemantic(this->invProjectionSemantic);
    this->focalLengthShaderVar = shdServer->GetSharedVariableBySemantic(NEBULA3_SEMANTIC_FOCALLENGTH);
    return TransformDeviceBase::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9TransformDevice::Close()
{
    this->shdEyePos = 0;
    this->viewMatrix = 0;
    this->invViewMatrix = 0;
    this->invProjectionMatrix = 0;
    TransformDeviceBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9TransformDevice::ApplyViewSettings()
{
    // update shared shader variable eye pos from current view matrix
    this->shdEyePos->SetFloat4(this->GetInvViewTransform().getrow3());
    
    // apply view matrix to shader
    this->viewMatrix->SetMatrix(this->GetViewTransform());   
    this->invViewMatrix->SetMatrix(this->GetInvViewTransform());

    // apply inv projection
    this->invProjectionMatrix->SetMatrix(this->GetInvProjTransform());  

    // apply projection matrix to shader
    this->projectionMatrix->SetMatrix(this->GetProjTransform());

    // apply focal length
    this->focalLengthShaderVar->SetFloat4(Math::float4(this->GetFocalLength().x(), this->GetFocalLength().y(), 0, 0));
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9TransformDevice::ApplyModelTransforms(const Ptr<ShaderInstance>& shdInst)
{    
    // apply ModelViewProjection 
    n_assert(shdInst->HasVariableBySemantic(this->mvpSemantic));
    const Ptr<ShaderVariable>& modelViewProjMatrix = shdInst->GetVariableBySemantic(mvpSemantic);       
    modelViewProjMatrix->SetMatrix(this->GetModelViewProjTransform());

    // apply optional ViewProjection
    if (shdInst->HasVariableBySemantic(this->viewProjSemantic))
    {
        const Ptr<ShaderVariable>& viewProj = shdInst->GetVariableBySemantic(this->viewProjSemantic);
        viewProj->SetMatrix(this->GetViewProjTransform());
    }

    // apply optional Model matrix to shader
    if (shdInst->HasVariableBySemantic(this->modelSemantic))
    {
        const Ptr<ShaderVariable>& model = shdInst->GetVariableBySemantic(this->modelSemantic);
        model->SetMatrix(this->GetModelTransform());
    }      

    // apply optional ModelView matrix to shader
    if (shdInst->HasVariableBySemantic(this->modelViewSemantic))
    {
        const Ptr<ShaderVariable>& modelView = shdInst->GetVariableBySemantic(this->modelViewSemantic);
        modelView->SetMatrix(this->GetModelViewTransform());
    }

    // apply optional InvModelView matrix to shader
    if (shdInst->HasVariableBySemantic(this->invModelViewSemantic))
    {
        const Ptr<ShaderVariable>& invModelView = shdInst->GetVariableBySemantic(this->invModelViewSemantic);
        invModelView->SetMatrix(this->GetInvModelViewTransform());
    } 

    // apply optional inverse projection matrix to shader
    if (shdInst->HasVariableBySemantic(this->invProjectionSemantic))
    {
        const Ptr<ShaderVariable>& invProj = shdInst->GetVariableBySemantic(this->invProjectionSemantic);
        invProj->SetMatrix(this->GetInvProjTransform());
    }
}

} // namespace Direct3D9

