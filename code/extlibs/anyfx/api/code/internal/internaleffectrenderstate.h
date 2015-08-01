#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectRenderState
    
    EffectRenderState backend, inherit this class to provide an implementation
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include "effectrenderstate.h"
namespace AnyFX
{
class Effect;
class InternalEffectRenderState
{
public:
	/// constructor
	InternalEffectRenderState();
	/// destructor
	virtual ~InternalEffectRenderState();

	static const unsigned MaxNumRenderTargets = 8;

protected:
	friend class EffectRenderState;
	friend class EffectRenderStateStreamLoader;

	/// applies render state, override in implementation
	virtual void Apply();
    /// resets render state
    void Reset();

	/// gets name of render state
    const eastl::string& GetName() const;
	
	Effect* effect;

	struct RenderStateSettings
	{
		bool blendEnabled[MaxNumRenderTargets];
		EffectRenderState::BlendMode srcBlends[MaxNumRenderTargets];
		EffectRenderState::BlendMode dstBlends[MaxNumRenderTargets];
		EffectRenderState::BlendOperation blendOps[MaxNumRenderTargets];
		EffectRenderState::BlendMode alphaSrcBlends[MaxNumRenderTargets];
		EffectRenderState::BlendMode alphaDstBlends[MaxNumRenderTargets];
		EffectRenderState::BlendOperation alphaBlendOps[MaxNumRenderTargets];

		EffectRenderState::StencilOperation frontFailOp;
		EffectRenderState::StencilOperation backFailOp;
		EffectRenderState::StencilOperation frontPassOp;
		EffectRenderState::StencilOperation backPassOp;
		EffectRenderState::StencilOperation frontDepthFailOp;
		EffectRenderState::StencilOperation backDepthFailOp;
		EffectRenderState::ComparisonFunc frontFaceFunc;
		EffectRenderState::ComparisonFunc backFaceFunc;
		int frontRef;
		int backRef;

		unsigned stencilReadMask;
		unsigned stencilWriteMask;
		EffectRenderState::ComparisonFunc depthFunction;
		EffectRenderState::DrawCullMode cullMode;
		EffectRenderState::DrawFillMode fillMode;

		bool depthEnabled;
		bool depthWriteEnabled;
		bool depthClampEnabled;
		bool separateBlendEnabled;
		bool scissorEnabled;
		bool stencilEnabled;
		bool alphaToCoverageEnabled;
		bool multisampleEnabled;
		bool polygonOffsetEnabled;
		float polygonOffsetFactor;
		float polygonOffsetUnits;
	} renderSettings, defaultRenderSettings;

    eastl::string name;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectRenderState::GetName() const
{
	return this->name;
}

} // namespace AnyFX
//------------------------------------------------------------------------------