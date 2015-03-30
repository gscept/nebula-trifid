#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectVarBufferStreamLoader
    
    Loads variable buffers from stream.
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "binreader.h"
#include "internal/internaleffectvarbuffer.h"
#include "EASTL/vector.h"
#include <map>
namespace AnyFX
{
class Effect;
class EffectVarbuffer;
class EffectVariable;
class EffectVarbufferStreamLoader
{
public:
	/// constructor
	EffectVarbufferStreamLoader();
	/// destructor
	virtual ~EffectVarbufferStreamLoader();

private:
    friend class EffectStreamLoader;

    // load variable buffer into object
	EffectVarbuffer* Load(BinReader* reader, Effect* effect);
	static std::map<std::string, InternalEffectVarbuffer*> sharedBuffers;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------