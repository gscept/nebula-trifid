#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectProgramStreamLoader
    
    Decodes an EffectProgram from the input stream.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
namespace AnyFX
{
class Effect;
class EffectProgram;
class EffectProgramStreamLoader
{
public:
	/// constructor
	EffectProgramStreamLoader();
	/// destructor
	virtual ~EffectProgramStreamLoader();

private:
	friend class EffectStreamLoader;

	EffectProgram* Load(BinReader* reader, Effect* effect);
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------