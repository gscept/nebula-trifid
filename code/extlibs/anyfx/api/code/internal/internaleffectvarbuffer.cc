//------------------------------------------------------------------------------
//  internaleffectvarbuffer.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "internaleffectvarbuffer.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
InternalEffectVarbuffer::InternalEffectVarbuffer() :
	bufferHandle(0),
	isSlave(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
InternalEffectVarbuffer::~InternalEffectVarbuffer()
{
	this->childBuffers.clear();
	this->masterBuffer = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarbuffer::Setup(eastl::vector<InternalEffectProgram*> programs)
{
	this->masterBuffer = this;
	this->bufferHandle = new void*;
	*this->bufferHandle = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalEffectVarbuffer::SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarbuffer* master)
{
	assert(!this->isSlave);

	// set master pointer
	this->masterBuffer = master;
	this->masterBuffer->childBuffers.push_back(this);

	// set slave flag
	this->isSlave = true;

	// make sure slaved varblocks use the same handle
	this->bufferHandle = masterBuffer->bufferHandle;
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarbuffer::Apply()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarbuffer::Commit()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectVarbuffer::Activate( InternalEffectProgram* program )
{
	// override in subclass
}

} // namespace AnyFX