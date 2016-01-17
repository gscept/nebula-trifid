//------------------------------------------------------------------------------
//  effectprogram.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "./effectprogram.h"
#include "./effectrenderstate.h"
#include "./effectshader.h"
#include "internal/internaleffectprogram.h"
#include <assert.h>


namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectProgram::EffectProgram() :
	internalProgram(NULL)
{
	this->shaderBlock.vs = NULL;
	this->shaderBlock.ps = NULL;
	this->shaderBlock.hs = NULL;
	this->shaderBlock.ds = NULL;
	this->shaderBlock.gs = NULL;
	this->shaderBlock.cs = NULL;
}

//------------------------------------------------------------------------------
/**
*/
EffectProgram::~EffectProgram()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProgram::Discard()
{
	delete this->internalProgram;
	this->internalProgram = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProgram::Apply()
{
	assert(0 != this->renderState);
	assert(0 != this->internalProgram);
	this->internalProgram->Apply();	
	this->renderState->Apply();	
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProgram::Commit()
{
	assert(0 != this->internalProgram);
	this->internalProgram->Commit();
}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
EffectProgram::GetName() const
{
	assert(0 != this->internalProgram);
	return this->internalProgram->GetName();
}

//------------------------------------------------------------------------------
/**
*/
bool 
EffectProgram::IsValid()
{
	return this->internalProgram->linkState == InternalEffectProgram::LinkedOk;
}

//------------------------------------------------------------------------------
/**
*/
const eastl::string&
EffectProgram::GetError() const
{
	return this->internalProgram->error;
}

//------------------------------------------------------------------------------
/**
*/
void
EffectProgram::LoadingDone()
{
	this->internalProgram->LoadingDone();
}

//------------------------------------------------------------------------------
/**
*/
const bool 
EffectProgram::SupportsTessellation() const
{
	return this->internalProgram->SupportsTessellation();
}

//------------------------------------------------------------------------------
/**
*/
const unsigned* 
EffectProgram::GetLocalSizes() const
{
    assert(0 != this->shaderBlock.cs);
    return this->shaderBlock.cs->GetLocalSizes();
}

//------------------------------------------------------------------------------
/**
*/
eastl::vector<EffectShader*> 
EffectProgram::GetShaders() const
{
    eastl::vector<EffectShader*> shaderList;
    shaderList.resize(6);
    shaderList[0] = this->shaderBlock.vs;
    shaderList[1] = this->shaderBlock.ps;
    shaderList[2] = this->shaderBlock.hs;
    shaderList[3] = this->shaderBlock.ds;
    shaderList[4] = this->shaderBlock.gs;
    shaderList[5] = this->shaderBlock.cs;
    return shaderList;
}

//------------------------------------------------------------------------------
/**
*/
EffectRenderState* 
EffectProgram::GetRenderState() const
{
    return this->renderState;
}
} // namespace AnyFX
