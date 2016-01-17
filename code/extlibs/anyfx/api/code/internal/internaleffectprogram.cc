//------------------------------------------------------------------------------
//  internaleffectprogram.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "internaleffectprogram.h"
#include "effect.h"
#include "effectvariable.h"
#include "effectvarblock.h"
#include "effectvarbuffer.h"
#include "internaleffectvariable.h"
#include "internaleffectsampler.h"
#include "internaleffectvarblock.h"
#include "internaleffectsubroutine.h"
#include "internaleffectvarbuffer.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
InternalEffectProgram::InternalEffectProgram() :
	renderState(NULL),
	linkState(NotLinked),
	supportsTessellation(false),
	patchSize(0)
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
InternalEffectProgram::~InternalEffectProgram()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectProgram::Apply()
{
	// signal our variables and varblocks that this program is active so that they may select their correct internal handle
	unsigned i;
	size_t num = this->effect->numVariables;
	for (i = 0; i < num; ++i)
	{
		this->effect->variablesByIndex[i]->internalVariable->Activate(this);
		this->effect->variablesByIndex[i]->internalVariable->Apply();
	}

	num = this->effect->numVarblocks;
	for (i = 0; i < num; ++i)
	{
		this->effect->varblocksByIndex[i]->internalVarblock->Activate(this);
		this->effect->varblocksByIndex[i]->internalVarblock->Apply();
	}

	num = this->effect->numVarbuffers;
	for (i = 0; i < num; ++i)
	{
		this->effect->varbuffersByIndex[i]->internalVarbuffer->Activate(this);
		this->effect->varbuffersByIndex[i]->internalVarbuffer->Apply();
	}

	num = this->effect->numSamplers;
	for (i = 0; i < num; ++i)
	{
		this->effect->samplersByIndex[i]->internalSampler->Apply();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectProgram::Commit()
{
	// signal our variables and varblocks to apply their variables
	unsigned i;
	size_t num = this->effect->numVariables;
	for (i = 0; i < num; ++i)
	{
		this->effect->variablesByIndex[i]->internalVariable->Commit();
	}

	num = this->effect->numVarbuffers;
	for (i = 0; i < num; ++i)
	{
		this->effect->varbuffersByIndex[i]->internalVarbuffer->Commit();
	}

	num = this->effect->numVarblocks;
	for (i = 0; i < num; ++i)
	{
		this->effect->varblocksByIndex[i]->internalVarblock->Commit();
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
InternalEffectProgram::Link()
{
	// override me if needed!
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectProgram::SetupSlave(InternalEffectProgram* other)
{
	// override me!
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectProgram::SetupSubroutines()
{
    // override me!
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectProgram::LoadingDone()
{
	// override me!
}

} // namespace AnyFX