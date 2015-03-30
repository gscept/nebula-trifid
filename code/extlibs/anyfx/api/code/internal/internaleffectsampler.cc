//------------------------------------------------------------------------------
//  internaleffectsampler.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "internaleffectsampler.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
InternalEffectSampler::InternalEffectSampler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
InternalEffectSampler::~InternalEffectSampler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectSampler::Setup(const eastl::vector<InternalEffectVariable*>& textures)
{
	this->textureVariables = textures;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectSampler::Apply()
{
	// override in subclass
}

} // namespace AnyFX