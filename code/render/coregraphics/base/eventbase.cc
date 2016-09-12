//------------------------------------------------------------------------------
// eventbase.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "eventbase.h"

namespace Base
{

__ImplementClass(Base::EventBase, 'EVBA', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
EventBase::EventBase() :
	createSignaled(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EventBase::~EventBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
EventBase::Signal()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
EventBase::Wait()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
EventBase::Reset()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
EventBase::Setup()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
EventBase::Discard()
{
	// implement in subclass
}

} // namespace Base