//------------------------------------------------------------------------------
// frameswapbuffers.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frameswapbuffers.h"

namespace Frame2
{

__ImplementClass(Frame2::FrameSwapbuffers, 'FRSW', Frame2::FrameOp);
//------------------------------------------------------------------------------
/**
*/
FrameSwapbuffers::FrameSwapbuffers()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameSwapbuffers::~FrameSwapbuffers()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameSwapbuffers::Run(const IndexT frameIndex)
{
	tex->SwapBuffers();
}

} // namespace Frame2