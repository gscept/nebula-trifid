//------------------------------------------------------------------------------
// frameop.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frameop.h"

namespace Frame2
{

__ImplementClass(Frame2::FrameOp, 'FROP', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
FrameOp::FrameOp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameOp::~FrameOp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameOp::Run(const IndexT frameIndex)
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
FrameOp::DisplayResized(const SizeT width, const SizeT height)
{
	// implement in subclass
}

} // namespace Frame2