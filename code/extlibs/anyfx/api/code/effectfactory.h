#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectFactory
    
    Creates Effects by using the stream loaders.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <map>
#include <string>
#include "loaders/effectstreamloader.h"
namespace AnyFX
{
class Effect;
class InternalEffectVarblock;
class EffectFactory
{
public:
	/// constructor
	EffectFactory();
	/// destructor
	virtual ~EffectFactory();

	/// returns instance to factory
	static EffectFactory* Instance();

	/// creates an effect from file
	Effect* CreateEffectFromFile(const std::string& file);
	/// creates an effect from memory
	Effect* CreateEffectFromMemory(void* data, size_t size);

private:


	/// sets shared varblock for signature, this way, we can reuse the varblock for several shaders
	void SetSharedVarblock(const std::string& signature, InternalEffectVarblock* varblock);
	
	static EffectFactory* instance;

	std::map<std::string, InternalEffectVarblock*> sharedVarblocks;
	EffectStreamLoader loader;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------
