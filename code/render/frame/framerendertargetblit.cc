//------------------------------------------------------------------------------
//  framerendertargetblit.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framerendertargetblit.h"

namespace Frame
{
__ImplementClass(Frame::FrameRenderTargetBlit, 'FRCP', Frame::FramePassBase);

//------------------------------------------------------------------------------
/**
*/
FrameRenderTargetBlit::FrameRenderTargetBlit()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameRenderTargetBlit::~FrameRenderTargetBlit()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameRenderTargetBlit::Render(IndexT frameIndex)
{
    n_assert(this->from.isvalid());
    n_assert(this->to.isvalid());

    // perform copy
    this->from->Copy(this->to);
}

} // namespace Frame