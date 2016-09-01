//------------------------------------------------------------------------------
// framebufferbase.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "passbase.h"

namespace Base
{

__ImplementClass(Base::PassBase, 'FRBS', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
PassBase::PassBase() :
	depthStencilAttachment(NULL),
	depthStencilFlags(NoFlags),
	clearDepth(1),
	clearStencil(0),
	inBegin(false),
	currentSubpass(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PassBase::~PassBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
PassBase::Setup()
{
	// override in implementation
}

//------------------------------------------------------------------------------
/**
*/
void
PassBase::Discard()
{
	// override in implementation
}

//------------------------------------------------------------------------------
/**
*/
void
PassBase::Begin()
{
	n_assert(!this->inBegin);
	this->inBegin = true;
	this->currentSubpass = 0;
	// override in implementation
}

//------------------------------------------------------------------------------
/**
*/
void
PassBase::NextSubpass()
{
	n_assert(this->inBegin);
	this->currentSubpass++;
}

//------------------------------------------------------------------------------
/**
*/
void
PassBase::End()
{
	n_assert(this->inBegin);
	this->inBegin = false;
	// override in implementation
}

} // namespace Base