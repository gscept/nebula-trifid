//------------------------------------------------------------------------------
//  effectfactory.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "effectfactory.h"
#include <assert.h>
#include "binreader.h"
#include <fstream>

namespace AnyFX
{

EffectFactory* EffectFactory::instance = 0;
//------------------------------------------------------------------------------
/**
*/
EffectFactory::EffectFactory()
{
	assert(instance == 0);
	instance = this;
}

//------------------------------------------------------------------------------
/**
*/
EffectFactory::~EffectFactory()
{
	assert(instance != 0);
	instance = 0;
}

//------------------------------------------------------------------------------
/**
*/
EffectFactory* 
EffectFactory::Instance()
{
	assert(instance != 0);
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
void
EffectFactory::SetSharedVarblock(const eastl::string& signature, InternalEffectVarblock* varblock)
{
	assert(this->sharedVarblocks.find(signature) == this->sharedVarblocks.end());
	this->sharedVarblocks[signature] = varblock;
}

//------------------------------------------------------------------------------
/**
*/
Effect* 
EffectFactory::CreateEffectFromFile(const eastl::string& file)
{
	BinReader reader;
	reader.SetPath(file.c_str());
	reader.Open();
	this->loader.SetReader(&reader);
	Effect* retval = this->loader.Load();
	this->loader.SetReader(0);
	reader.Close();
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
Effect* 
EffectFactory::CreateEffectFromMemory( void* data, size_t size )
{
	BinReader* reader = new BinReader;
	reader->Open((const char*)data, size);
	this->loader.SetReader(reader);
	Effect* retval = this->loader.Load();
	this->loader.SetReader(0);
	reader->Close();	
	delete reader;
	return retval;
}

} // namespace AnyFX