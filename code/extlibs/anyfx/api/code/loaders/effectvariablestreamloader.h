#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectVariableStreamLoader
    
    Decodes a EffectVariable from stream.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
#include <map>
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
    static std::map<std::string, InternalEffectVariable*> sharedVariables;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------