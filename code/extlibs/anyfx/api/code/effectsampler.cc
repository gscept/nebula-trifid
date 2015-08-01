//------------------------------------------------------------------------------
//  effectsampler.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "./effectsampler.h"
#include "internal/internaleffectsampler.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectSampler::EffectSampler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectSampler::~EffectSampler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectSampler::Discard()
{
	delete this->internalSampler;
	this->internalSampler = 0;
}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
EffectSampler::GetName() const
{
	return this->internalSampler->GetName();
}

} // namespace AnyFX