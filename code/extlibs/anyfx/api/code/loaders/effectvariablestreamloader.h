#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectVariableStreamLoader
    
    Decodes a EffectVariable from stream.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
#include "EASTL/map.h"
#include "EASTL/string.h"
namespace AnyFX
{
class Effect;
class EffectVariable;
class InternalEffectVarblock;
class InternalEffectVariable;
class EffectVariableStreamLoader
{
public:
	/// constructor
	EffectVariableStreamLoader();
	/// destructor
	virtual ~EffectVariableStreamLoader();

private:
	friend class EffectStreamLoader;
	friend class EffectVarblockStreamLoader;

	EffectVariable* Load(BinReader* reader, Effect* effect, InternalEffectVarblock* varblock = 0);
    static eastl::map<eastl::string, InternalEffectVariable*> sharedVariables;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------