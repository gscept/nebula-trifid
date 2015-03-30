#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectSampler
    
    Internal representation of a sampler
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "internal/internaleffectvariable.h"
#include "effectsampler.h"
#include <string>
namespace AnyFX
{
class Effect;
class InternalEffectSampler
{
public:
	/// constructor
	InternalEffectSampler();
	/// destructor
	virtual ~InternalEffectSampler();

		/// setup using texture variable
	virtual void Setup(const eastl::vector<InternalEffectVariable*>& texture);

	/// get name of variable
	const std::string& GetName() const;

    static const unsigned MaxNumSamplerBinds = 256;

protected:
	friend class EffectSampler;
	friend class EffectSamplerStreamLoader;
	friend class InternalEffectProgram;

	// applies sampler
	virtual void Apply();

	Effect* effect;

	struct SamplerSettings
	{
		EffectSampler::FilterMode filterMode;
		EffectSampler::AddressMode addressU;
		EffectSampler::AddressMode addressV;
		EffectSampler::AddressMode addressW;
		EffectSampler::ComparisonFunc comparisonFunc;

		bool isComparison;
		float minLod;
		float maxLod;
		float lodBias;
		float maxAnisotropic;
		float borderColor[4];

	} samplerSettings;

	eastl::vector<InternalEffectVariable*> textureVariables;
	InternalEffectVariable* textureVariable;

	std::string name;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
InternalEffectSampler::GetName() const
{
	return this->name;
}

} // namespace AnyFX
//------------------------------------------------------------------------------