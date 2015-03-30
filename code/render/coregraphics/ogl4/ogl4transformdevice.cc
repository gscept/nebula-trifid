//------------------------------------------------------------------------------
//  ogl4transformdevice.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/ogl4/ogl4transformdevice.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/shadersemantics.h"
#include "coregraphics/ogl4/ogl4renderdevice.h"
#include "framesync/framesynctimer.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4TransformDevice, 'D1TD', Base::TransformDeviceBase);
__ImplementSingleton(OpenGL4::OGL4TransformDevice);

using namespace Util;
using namespace CoreGraphics;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
OGL4TransformDevice::OGL4TransformDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4TransformDevice::~OGL4TransformDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
OGL4TransformDevice::Open()
{
    ShaderServer* shdServer = ShaderServer::Instance();
    const Ptr<ShaderInstance>& shdInst = shdServer->GetSharedShader();
    this->viewVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_VIEW);
    this->invViewVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_INVVIEW);
    this->viewProjVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_VIEWPROJECTION);
    this->invViewProjVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_INVVIEWPROJECTION);
    this->projVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_PROJECTION);
    this->invProjVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_INVPROJECTION);
    this->eyePosVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_EYEPOS);
    this->focalLengthVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_FOCALLENGTH);
	this->viewMatricesVar = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_VIEWMATRIXARRAY);
	this->timeAndRandomVariable = shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_TIMEANDRANDOM);

	// get per frame block
	this->perFrameBlock = shdInst->GetAnyFXEffect()->GetVarblockByName("PerFrame");
	this->perFrameBlock->SetFlushManually(true);
	this->perShadowFrameBlock = shdInst->GetAnyFXEffect()->GetVarblockByName("PerShadowFrame");
	this->perShadowFrameBlock->SetFlushManually(true);
    return TransformDeviceBase::Open();
}

//------------------------------------------------------------------------------
/**
/*
void
OGL4TransformDevice::Close()
{
    this->viewVar = 0;
    this->invViewVar = 0;
    this->viewProjVar = 0;
    this->invViewProjVar = 0;
    this->projVar = 0;
    this->invProjVar = 0;
    this->eyePosVar = 0;
    this->focalLengthVar = 0;
    TransformDeviceBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4TransformDevice::ApplyViewSettings()
{
    this->viewProjVar->SetMatrix(this->GetViewProjTransform());    
    this->invViewProjVar->SetMatrix(this->GetInvViewTransform());
    this->viewVar->SetMatrix(this->GetViewTransform());
    this->invViewVar->SetMatrix(this->GetInvViewTransform());
    this->projVar->SetMatrix(this->GetProjTransform());
    this->invProjVar->SetMatrix(this->GetInvProjTransform());
    this->eyePosVar->SetFloat4(this->GetInvViewTransform().getrow3());
    this->focalLengthVar->SetFloat4(float4(this->GetFocalLength().x(), this->GetFocalLength().y(), 0, 0));	

	// set time and random, this isn't really related to the transform device, but the variable is in the per-frame block
	this->timeAndRandomVariable->SetFloat4(Math::float4(
		(float)FrameSync::FrameSyncTimer::Instance()->GetTime(),
		Math::n_rand(0, 1),
		0, 0));
	this->perFrameBlock->FlushBuffer();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4TransformDevice::ApplyViewMatrixArray(const Math::matrix44* matrices, SizeT num)
{
	this->viewMatricesVar->SetMatrixArray(matrices, num);
	this->perShadowFrameBlock->FlushBuffer();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4TransformDevice::ApplyModelTransforms(const Ptr<ShaderInstance>& shdInst)
{    
    if (shdInst->HasVariableBySemantic(NEBULA3_SEMANTIC_MODEL))
    {
        shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_MODEL)->SetMatrix(this->GetModelTransform());
    }
    if (shdInst->HasVariableBySemantic(NEBULA3_SEMANTIC_INVMODEL))
    {
        shdInst->GetVariableBySemantic(NEBULA3_SEMANTIC_INVMODEL)->SetMatrix(this->GetInvModelTransform());
    }
}

} // namespace OpenGL4
