//------------------------------------------------------------------------------
//  effectsamplerstreamloader.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "effectsamplerstreamloader.h"
#include "internal/internaleffectsampler.h"
#include "internal/glsl4/glsl4effectsampler.h"
#include "effectsampler.h"
#include "effect.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectSamplerStreamLoader::EffectSamplerStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectSamplerStreamLoader::~EffectSamplerStreamLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectSampler* 
EffectSamplerStreamLoader::Load( BinReader* reader, Effect* effect )
{
	InternalEffectSampler* internalSampler = 0;

	// get sampler
	if (effect->GetType() == Effect::GLSL)
	{
		if (effect->GetMajor() == 4) internalSampler = new GLSL4EffectSampler;
	}
	internalSampler->effect = effect;
	EffectSampler* sampler = new EffectSampler;
	
	// get name
    eastl::string name = reader->ReadString().c_str();
	internalSampler->name = name;

	internalSampler->samplerSettings.filterMode = (EffectSampler::FilterMode)reader->ReadInt();
	internalSampler->samplerSettings.addressU = (EffectSampler::AddressMode)reader->ReadInt();
	internalSampler->samplerSettings.addressV = (EffectSampler::AddressMode)reader->ReadInt();
	internalSampler->samplerSettings.addressW = (EffectSampler::AddressMode)reader->ReadInt();
	internalSampler->samplerSettings.comparisonFunc = (EffectSampler::ComparisonFunc)reader->ReadInt();
	internalSampler->samplerSettings.lodBias = reader->ReadFloat();
	internalSampler->samplerSettings.minLod = reader->ReadFloat();
	internalSampler->samplerSettings.maxLod = reader->ReadFloat();
	internalSampler->samplerSettings.maxAnisotropic = reader->ReadFloat();
	internalSampler->samplerSettings.borderColor[0] = reader->ReadFloat();
	internalSampler->samplerSettings.borderColor[1] = reader->ReadFloat();
	internalSampler->samplerSettings.borderColor[2] = reader->ReadFloat();
	internalSampler->samplerSettings.borderColor[3] = reader->ReadFloat();
	internalSampler->samplerSettings.isComparison = reader->ReadBool();
	
	// read textures
	unsigned numTextures = reader->ReadInt();
	eastl::vector<InternalEffectVariable*> textureVars;
	unsigned i;
	for (i = 0; i < numTextures; i++)
	{
        eastl::string texture = reader->ReadString().c_str();
		EffectVariable* var = effect->GetVariableByName(texture);
		textureVars.push_back(var->internalVariable);		
	}

	internalSampler->Setup(textureVars);
	sampler->internalSampler = internalSampler;

	return sampler;
}

} // namespace AnyFX