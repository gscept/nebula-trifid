#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectRenderStateStreamLoader
    
    Decodes an EffectRenderState from stream.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
namespace AnyFX
{
class Effect;
class EffectRenderState;
class EffectRenderStateStreamLoader
{
public:
	/// constructor
	EffectRenderStateStreamLoader();
	/// destructor
	virtual ~EffectRenderStateStreamLoader();

private:
	friend class EffectStreamLoader;

	EffectRenderState* Load(BinReader* reader, Effect* effect);
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------