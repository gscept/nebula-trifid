#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectStreamLoader
    
    Loads an Effect by parsing the embedded FourCC codes and 
	applies the appropriate stream loader for each type of data type in the binary FX file.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binreader.h"
#include "effectprogramstreamloader.h"
#include "effectvariablestreamloader.h"
#include "effectvarblockstreamloader.h"
#include "effectrenderstatestreamloader.h"
#include "effectsamplerstreamloader.h"
#include "effectshaderstreamloader.h"
#include "effectvarbufferstreamloader.h"
#include "effectsubroutinestreamloader.h"
namespace AnyFX
{
class Effect;
class EffectStreamLoader
{
public:
	/// constructor
	EffectStreamLoader();
	/// destructor
	virtual ~EffectStreamLoader();

	/// set binary reader, this should already be opened when the stream loader is run
	void SetReader(BinReader* reader);
	/// get binary reader
	BinReader* GetReader() const;

private:
	friend class EffectFactory;

	/// loads effect
	Effect* Load();

	EffectShaderStreamLoader shaderLoader;
	EffectVariableStreamLoader variableLoader;
	EffectVarblockStreamLoader varblockLoader;
	EffectRenderStateStreamLoader renderStateLoader;
	EffectProgramStreamLoader programLoader;
	EffectSamplerStreamLoader samplerLoader;
    EffectVarbufferStreamLoader varbufferLoader;
    EffectSubroutineStreamLoader subroutineLoader;
	BinReader* reader;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
EffectStreamLoader::SetReader( BinReader* reader )
{
	this->reader = reader;
}

//------------------------------------------------------------------------------
/**
*/
inline BinReader* 
EffectStreamLoader::GetReader() const
{
	return this->reader;
}

} // namespace AnyFX
//------------------------------------------------------------------------------