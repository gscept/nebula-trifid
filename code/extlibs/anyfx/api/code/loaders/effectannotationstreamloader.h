#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectAnnotationStreamLoader
    
    Loads annotations attached to effect objects.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
namespace AnyFX
{
class Annotable;
class EffectAnnotationStreamLoader
{
public:
	/// constructor
	EffectAnnotationStreamLoader();
	/// destructor
	virtual ~EffectAnnotationStreamLoader();

private:
	friend class EffectProgramStreamLoader;
	friend class EffectRenderStateStreamLoader;
	friend class EffectVarblockStreamLoader;
	friend class EffectVarbufferStreamLoader;
	friend class EffectVariableStreamLoader;
	

	// load annotation and save into object
	void Load(BinReader* reader, Annotable* object);
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------