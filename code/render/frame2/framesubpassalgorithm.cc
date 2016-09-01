//------------------------------------------------------------------------------
// framesubpassalgorithm.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framesubpassalgorithm.h"

namespace Frame2
{

__ImplementClass(Frame2::FrameSubpassAlgorithm, 'FSUA', Frame2::FrameOp);
//------------------------------------------------------------------------------
/**
*/
FrameSubpassAlgorithm::FrameSubpassAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameSubpassAlgorithm::~FrameSubpassAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameSubpassAlgorithm::Run(const IndexT frameIndex)
{
	this->alg->Execute(this->func, frameIndex);
}

} // namespace Frame2