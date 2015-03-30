 #pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectVarblockStreamLoader
    
    Decodes a EffectVarblock from stream.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
#include "internal/internaleffectvarblock.h"
#include "EASTL/vector.h"
#include <map>
namespace AnyFX
{
class Effect;
class EffectVarblock;
class EffectVariable;
class EffectVarblockStreamLoader
{
public:
	/// constructor
	EffectVarblockStreamLoader();
	/// destructor
	virtual ~EffectVarblockStreamLoader();

private:
	friend class EffectStreamLoader;

	EffectVarblock* Load(BinReader* reader, Effect* effect, eastl::vector<EffectVariable*>& vars);
	static std::map<std::string, InternalEffectVarblock*> sharedBlocks;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------