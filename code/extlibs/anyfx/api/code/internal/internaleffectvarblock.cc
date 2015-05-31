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
	if (this->masterBlock == this) delete[] this->dataBlock->data;
	this->masterBlock = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::Setup( eastl::vector<InternalEffectProgram*> programs )
{
	// create new datablock if and only if we are the master varblock
	this->dataBlock = new InternalVarblockData;

	// point our master block to ourselves, this way we can always locate the master block of any varblock
	this->masterBlock = this;

	// set size to 0
	this->dataBlock->size = 0;

	// setup variable offsets and sizes
	unsigned i;
	for (i = 0; i < this->variables.size(); i++)
	{
		InternalEffectVariable* variable = this->variables[i];
		variable->byteOffset = this->dataBlock->size;
		this->dataBlock->size += variable->byteSize;
	}

	// create cpu-side buffer
	this->dataBlock->data = new char[this->dataBlock->size];

	// initialized variable
	for (i = 0; i < this->variables.size(); i++)
	{
		InternalEffectVariable* variable = this->variables[i];
		variable->InitializeDefaultValues();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::SetupSlave( eastl::vector<InternalEffectProgram*> programs, InternalEffectVarblock* master )
{
	assert(!this->isSlave);

	// copy pointer to data buffer
	this->dataBlock = master->dataBlock;

	// set master pointer
	this->masterBlock = master;
	this->masterBlock->childBlocks.push_back(this);

	// set slave flag
	this->isSlave = true;

	// calculate size again
	unsigned size = 0;

	// initialized variable
	unsigned i;
	for (i = 0; i < this->variables.size(); i++)
	{
		InternalEffectVariable* variable = this->variables[i];
		variable->byteOffset = size;
		size += variable->byteSize;
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
InternalEffectVarblock::SetVariable( InternalEffectVariable* var, void* value )
{
    char* data = this->dataBlock->data + var->byteOffset;
	memcpy(data, value, var->byteSize);
	this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::SetVariableArray( InternalEffectVariable* var, void* value, size_t size )
{
    char* data = this->dataBlock->data + var->byteOffset;
	memcpy(data, value, size);
	this->isDirty = true;	
}

//------------------------------------------------------------------------------
/**
    Basically the same as SetVariable, however it assumes the variable is an array, and sets the variable at a certain index.
    This would be equivalent to arr[i] = value;
*/
void 
InternalEffectVarblock::SetVariableIndexed( InternalEffectVariable* var, void* value, unsigned i )
{
    char* data = this->dataBlock->data + var->byteOffset + i * var->byteSize;
    memcpy(data, value, var->byteSize);
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarblock::Activate( InternalEffectProgram* program )
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
