#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectSamplerStreamLoader
    
    Loads sampler from binary blob
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
namespace AnyFX
{
class Effect;
class EffectSampler;
class EffectSamplerStreamLoader
{
public:
	/// constructor
	EffectSamplerStreamLoader();
	/// destructor
	virtual ~EffectSamplerStreamLoader();

private:
	friend class EffectStreamLoader;

	EffectSampler* Load(BinReader* reader, Effect* effect);
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------