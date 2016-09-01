//------------------------------------------------------------------------------
// framesubpass.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framesubpass.h"

namespace Frame2
{

__ImplementClass(Frame2::FrameSubpass, 'FRSU', Frame2::FrameOp);
//------------------------------------------------------------------------------
/**
*/
FrameSubpass::FrameSubpass()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameSubpass::~FrameSubpass()
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/
void
FrameSubpass::AddOp(const Ptr<Frame2::FrameOp>& op)
{
	this->ops.Append(op);
}

//------------------------------------------------------------------------------
/**
*/
void
FrameSubpass::Run(const IndexT frameIndex)
{
	IndexT i;
	for (i = 0; i < this->ops.Size(); i++)
	{
		this->ops[i]->Run(frameIndex);
	}
}

} // namespace Frame2