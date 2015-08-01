//------------------------------------------------------------------------------
//  effectvariable.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "./effectrenderstate.h"
#include "internal/internaleffectrenderstate.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectRenderState::EffectRenderState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectRenderState::~EffectRenderState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::Discard()
{
	delete this->internalRenderState;
	this->internalRenderState = 0;
}

//------------------------------------------------------------------------------
/**
    This function resets the state to be like it was when the file was loaded.
*/
void 
EffectRenderState::Reset()
{
    this->internalRenderState->Reset();
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::Apply()
{
	this->internalRenderState->Apply();
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetBlendModeFlag( unsigned renderTarget, BlendModeFlag flag, BlendMode mode )
{
	switch (flag)
	{
	case SrcBlend:
		this->internalRenderState->renderSettings.srcBlends[renderTarget] = mode;
		break;
	case DstBlend:
		this->internalRenderState->renderSettings.dstBlends[renderTarget] = mode;
		break;
	case SrcBlendAlpha:
		this->internalRenderState->renderSettings.alphaSrcBlends[renderTarget] = mode;
		break;
	case DstBlendAlpha:
		this->internalRenderState->renderSettings.alphaDstBlends[renderTarget] = mode;
		break;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetBlendFuncFlag( unsigned renderTarget, BlendFuncFlag flag, BlendOperation op )
{
	switch (flag)
	{
	case BlendOp:
		this->internalRenderState->renderSettings.blendOps[renderTarget] = op;
		break;
	case BlendOpAlpha:
		this->internalRenderState->renderSettings.alphaBlendOps[renderTarget] = op;
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetBlendBoolFlag( unsigned renderTarget, BlendBoolFlag flag, bool value )
{
	switch (flag)
	{
	case DepthEnabled:
		this->internalRenderState->renderSettings.depthEnabled = value;
		break;
	case DepthWrite:
		this->internalRenderState->renderSettings.depthWriteEnabled = value;
		break;
	case SeparateBlend:
		this->internalRenderState->renderSettings.separateBlendEnabled = value;
		break;
	case StencilEnabled:
		this->internalRenderState->renderSettings.stencilEnabled = value;
		break;
	case AlphaToCoverageEnabled:
		this->internalRenderState->renderSettings.alphaToCoverageEnabled = value;
		break;
	case MultisampleEnabled:
		this->internalRenderState->renderSettings.multisampleEnabled = value;
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetStencilOpFlag( StencilOpFlag flag, StencilOperation op )
{
	switch (flag)
	{
	case StencilFrontFailOp:
		this->internalRenderState->renderSettings.frontFailOp = op;
		break;
	case StencilBackFailOp:
		this->internalRenderState->renderSettings.backFailOp = op;
		break;
	case StencilFrontPassOp:
		this->internalRenderState->renderSettings.frontPassOp = op;
		break;
	case StencilBackPassOp:
		this->internalRenderState->renderSettings.backPassOp = op;
		break;
	case StencilFrontDepthFailOp:
		this->internalRenderState->renderSettings.frontDepthFailOp = op;
		break;
	case StencilBackDepthFailOp:
		this->internalRenderState->renderSettings.backDepthFailOp = op;
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetStencilFuncFlag( StencilFuncFlag flag, ComparisonFunc func )
{
	switch (flag)
	{
	case StencilFrontFunc:
		this->internalRenderState->renderSettings.frontFaceFunc = func;
		break;
	case StencilBackFunc:
		this->internalRenderState->renderSettings.backFaceFunc = func;
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetDepthFunc( ComparisonFunc func )
{
	this->internalRenderState->renderSettings.depthFunction = func;
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetFillMode( DrawFillMode fillMode )
{
	this->internalRenderState->renderSettings.fillMode = fillMode;
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetCullMode( DrawCullMode cullMode )
{
	this->internalRenderState->renderSettings.cullMode = cullMode;
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectRenderState::SetStencilReadWrite( unsigned read, unsigned write )
{
	this->internalRenderState->renderSettings.stencilReadMask = read;
	this->internalRenderState->renderSettings.stencilWriteMask = write;
}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
EffectRenderState::GetName() const
{
	return this->internalRenderState->GetName();
}

} // namespace AnyFX