#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectShaderStreamLoader
    
    Decodes an EffectShader from stream.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
namespace AnyFX
{
class Effect;
class EffectShader;
class EffectShaderStreamLoader
{
public:
	/// constructor
	EffectShaderStreamLoader();
	/// destructor
	virtual ~EffectShaderStreamLoader();

private:
	friend class EffectStreamLoader;

	EffectShader* Load(BinReader* reader, Effect* effect);
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------