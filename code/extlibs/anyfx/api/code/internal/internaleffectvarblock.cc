//------------------------------------------------------------------------------
//  internaleffectvarblock.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "internaleffectvarblock.h"
#include "internaleffectvariable.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
InternalEffectVarblock::InternalEffectVarblock() :
	active(false),
	isShared(false),
	noSync(false),
	isDirty(true),
	isSlave(false),
	manualFlushing(false),
    currentBufferHandle(0),
	masterBlock(NULL)
{
	this->Retain();
}

//------------------------------------------------------------------------------
/**
*/
InternalEffectVarblock::~InternalEffectVarblock()
{
	this->childBlocks.clear();
	this->masterBlock = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarblock::Setup(eastl::vector<InternalEffectProgram*> programs)
{
	// point our master block to ourselves, this way we can always locate the master block of any varblock
	this->masterBlock = this;
    this->currentBufferHandle = new void*;
    *this->currentBufferHandle = NULL;

    for (unsigned i = 0; i < this->variables.size(); i++)
    {
        this->variables[i]->sharedByteOffset = new unsigned;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarblock::SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarblock* master)
{
	assert(!this->isSlave);

	// set master pointer
	this->masterBlock = master;
	this->masterBlock->childBlocks.push_back(this);

	// set slave flag
	this->isSlave = true;

    for (unsigned i = 0; i < this->variables.size(); i++)
    {
        this->variables[i]->sharedByteOffset = new unsigned;
    }

    // make sure slaved varblocks use the same handle
    this->currentBufferHandle = masterBlock->currentBufferHandle;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarblock::SetupDefaultValues()
{
    // initialized variable
    for (unsigned i = 0; i < this->variables.size(); i++)
    {
        InternalEffectVariable* variable = this->variables[i];
        variable->InitializeDefaultValues();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::Apply()
{
	// override me!
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::Commit()
{
	// override me!
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::PreDraw()
{
    // override me!
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::PostDraw()
{
    // override me!
}

//------------------------------------------------------------------------------
/**
	Run this when varblock is properly setup
*/
void 
InternalEffectVarblock::SetupSignature()
{
	// format signature by retrieving all variable signatures and making a string mask
	unsigned i;
	for (i = 0; i < this->variables.size(); i++)
	{
		InternalEffectVariable* variable = this->variables[i];
		this->signature.append(variable->GetSignature());
		this->signature.append(";");
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarblock::SetVariable(InternalEffectVariable* var, void* value)
{
    // override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarblock::SetVariableArray(InternalEffectVariable* var, void* value, size_t size)
{
    // override in subclass
}

//------------------------------------------------------------------------------
/**
    Basically the same as SetVariable, however it assumes the variable is an array, and sets the variable at a certain index.
    This would be equivalent to arr[i] = value;
*/
void
InternalEffectVarblock::SetVariableIndexed(InternalEffectVariable* var, void* value, unsigned i)
{
    // override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarblock::Activate(InternalEffectProgram* program)
{
	// override me!
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarblock::FlushBuffer()
{
	// override me!
}

//------------------------------------------------------------------------------
/**
	Sets the block to flush manually, this applies to ALL blocks
*/
void
InternalEffectVarblock::SetFlushManually(bool b)
{
	this->masterBlock->manualFlushing = b;
	unsigned i;
	for (i = 0; i < this->masterBlock->childBlocks.size(); i++)
	{
		this->masterBlock->childBlocks[i]->manualFlushing = b;
	}
}

} // namespace AnyFX
