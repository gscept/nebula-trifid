#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectSubroutineStreamLoader
    
    Loads a subroutine from stream.
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "binreader.h"
namespace AnyFX
{
class Effect;
class EffectSubroutine;
class EffectSubroutineStreamLoader
{
public:
	/// constructor
	EffectSubroutineStreamLoader();
	/// destructor
	virtual ~EffectSubroutineStreamLoader();

private:
    friend class EffectStreamLoader;

    // load subroutine into object
    EffectSubroutine* Load(BinReader* reader, Effect* effect);
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------