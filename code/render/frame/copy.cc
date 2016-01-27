//------------------------------------------------------------------------------
//  copy.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "copy.h"

namespace Frame
{
__ImplementClass(Frame::Copy, 'FRCP', Frame::FramePassBase);

//------------------------------------------------------------------------------
/**
*/
Copy::Copy()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Copy::~Copy()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Copy::Render(IndexT frameIndex)
{
    n_assert(this->from.isvalid());
    n_assert(this->to.isvalid());

    // perform copy
    this->from->Copy(this->to);
}

} // namespace Frame