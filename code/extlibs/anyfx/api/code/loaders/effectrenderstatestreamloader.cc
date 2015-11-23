//------------------------------------------------------------------------------
//  effectrenderstatestreamloader.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "effectrenderstatestreamloader.h"
#include "effectrenderstate.h"
#include "internal/internaleffectrenderstate.h"
#include "internal/glsl4/glsl4effectrenderstate.h"
#include "effect.h"
#include "effectannotationstreamloader.h"
#include <string.h>

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectRenderStateStreamLoader::EffectRenderStateStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectRenderStateStreamLoader::~EffectRenderStateStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectRenderState* 
EffectRenderStateStreamLoader::Load( BinReader* reader, Effect* effect )
{
	InternalEffectRenderState* internalRenderState = 0;

	// we should create our implementation back-end first
	if (effect->GetType() == Effect::GLSL)
	{
		if (effect->GetMajor() == 4) internalRenderState = new GLSL4EffectRenderState;
	}
	internalRenderState->effect = effect;
	EffectRenderState* renderState = new EffectRenderState;

	// get name
    eastl::string name = reader->ReadString().c_str();
	internalRenderState->name = name;

	bool hasAnnotation = reader->ReadBool();
	if (hasAnnotation)
	{
		EffectAnnotationStreamLoader loader;
		loader.Load(reader, renderState);
	}

	unsigned i;
	for (i = 0; i < InternalEffectRenderState::MaxNumRenderTargets; i++)
	{
		bool blendEnabled = reader->ReadBool();
		int sourceBlend = reader->ReadInt();
		int destinationBlend = reader->ReadInt();
		int blendOp = reader->ReadInt();
		int sourceBlendAlpha = reader->ReadInt();
		int destinationBlendAlpha = reader->ReadInt();
		int blendOpAlpha = reader->ReadInt();

		internalRenderState->renderSettings.blendEnabled[i]		= blendEnabled;
		internalRenderState->renderSettings.srcBlends[i]		= (EffectRenderState::BlendMode)sourceBlend;
		internalRenderState->renderSettings.dstBlends[i]		= (EffectRenderState::BlendMode)destinationBlend;
		internalRenderState->renderSettings.blendOps[i]			= (EffectRenderState::BlendOperation)blendOp;
		internalRenderState->renderSettings.alphaSrcBlends[i]	= (EffectRenderState::BlendMode)sourceBlendAlpha;
		internalRenderState->renderSettings.alphaDstBlends[i]	= (EffectRenderState::BlendMode)destinationBlendAlpha;
		internalRenderState->renderSettings.alphaBlendOps[i]	= (EffectRenderState::BlendOperation)blendOpAlpha;
	}

	bool depthEnabled = reader->ReadBool();
	bool depthWrite = reader->ReadBool();
	bool depthClamp = reader->ReadBool();
	bool separateBlend = reader->ReadBool();
	bool scissorEnabled = reader->ReadBool();
	bool stencilEnabled = reader->ReadBool();
	bool alphaToCoverageEnabled = reader->ReadBool();
	bool multisampleEnabled = reader->ReadBool();
	bool polygonOffsetEnabled = reader->ReadBool();

	internalRenderState->renderSettings.depthEnabled			= depthEnabled;
	internalRenderState->renderSettings.depthWriteEnabled		= depthWrite;
	internalRenderState->renderSettings.depthClampEnabled		= depthClamp;
	internalRenderState->renderSettings.separateBlendEnabled	= separateBlend;
	internalRenderState->renderSettings.scissorEnabled			= scissorEnabled;
	internalRenderState->renderSettings.stencilEnabled			= stencilEnabled;
	internalRenderState->renderSettings.alphaToCoverageEnabled	= alphaToCoverageEnabled;
	internalRenderState->renderSettings.multisampleEnabled		= multisampleEnabled;
	internalRenderState->renderSettings.polygonOffsetEnabled	= polygonOffsetEnabled;

	unsigned depthFunc = reader->ReadUInt();
	unsigned cullMode = reader->ReadUInt();
	unsigned rasterizerMode = reader->ReadUInt();
	unsigned stencilFrontFail = reader->ReadUInt();
	unsigned stencilBackFail = reader->ReadUInt();
	unsigned stencilFrontPass = reader->ReadUInt();
	unsigned stencilBackPass = reader->ReadUInt();
	unsigned stencilFrontDepthFail = reader->ReadUInt();
	unsigned stencilBackDepthFail = reader->ReadUInt();
	unsigned stencilFrontFunc = reader->ReadUInt();
	unsigned stencilBackFunc = reader->ReadUInt();

	internalRenderState->renderSettings.depthFunction		= (EffectRenderState::ComparisonFunc)depthFunc;
	internalRenderState->renderSettings.cullMode			= (EffectRenderState::DrawCullMode)cullMode;
	internalRenderState->renderSettings.fillMode			= (EffectRenderState::DrawFillMode)rasterizerMode;
	internalRenderState->renderSettings.frontFailOp			= (EffectRenderState::StencilOperation)stencilFrontFail;
	internalRenderState->renderSettings.backFailOp			= (EffectRenderState::StencilOperation)stencilBackFail;
	internalRenderState->renderSettings.frontPassOp			= (EffectRenderState::StencilOperation)stencilFrontPass;
	internalRenderState->renderSettings.backPassOp			= (EffectRenderState::StencilOperation)stencilBackPass;
	internalRenderState->renderSettings.frontDepthFailOp	= (EffectRenderState::StencilOperation)stencilFrontDepthFail;
	internalRenderState->renderSettings.backDepthFailOp		= (EffectRenderState::StencilOperation)stencilBackDepthFail;
	internalRenderState->renderSettings.frontFaceFunc		= (EffectRenderState::ComparisonFunc)stencilFrontFunc;
	internalRenderState->renderSettings.backFaceFunc		= (EffectRenderState::ComparisonFunc)stencilBackFunc;

	int stencilFrontRef = reader->ReadInt();
	int stencilBackRef = reader->ReadInt();
	unsigned stencilReadMask = reader->ReadUInt();
	unsigned stencilWriteMask = reader->ReadUInt();

	internalRenderState->renderSettings.frontRef			= stencilFrontRef;
	internalRenderState->renderSettings.backRef				= stencilBackRef;
	internalRenderState->renderSettings.stencilReadMask		= stencilReadMask;
	internalRenderState->renderSettings.stencilWriteMask	= stencilWriteMask;

	float polygonOffsetFactor = reader->ReadFloat();
	float polygonOffsetUnits = reader->ReadFloat();

	internalRenderState->renderSettings.polygonOffsetFactor = polygonOffsetFactor;
	internalRenderState->renderSettings.polygonOffsetUnits = polygonOffsetUnits;

    // memcpy default values loaded from file to interface
    memcpy(&internalRenderState->defaultRenderSettings, &internalRenderState->renderSettings, sizeof(InternalEffectRenderState::RenderStateSettings));

	renderState->internalRenderState = internalRenderState;
	return renderState;
}
} // namespace AnyFX
