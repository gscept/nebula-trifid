//------------------------------------------------------------------------------
//  shaderbufferbase.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderbufferbase.h"

namespace Base
{
__ImplementClass(Base::ShaderBufferBase, 'SHBB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ShaderBufferBase::ShaderBufferBase() :
	isSetup(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderBufferBase::~ShaderBufferBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBufferBase::Setup()
{
	n_assert(!this->isSetup);
	n_assert(this->size > 0);
	this->isSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBufferBase::Discard()
{
	n_assert(this->isSetup);
	this->isSetup = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderBufferBase::UpdateBuffer(void* data, SizeT offset, SizeT length)
{
	n_assert(offset < size);
	n_assert(length > 0);
	// implementation specific
}


} // namespace Base