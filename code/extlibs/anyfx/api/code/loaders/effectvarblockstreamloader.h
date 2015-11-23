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
#include "EASTL/map.h"
#include "EASTL/string.h"
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
    static eastl::map<eastl::string, InternalEffectVarblock*> sharedBlocks;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------