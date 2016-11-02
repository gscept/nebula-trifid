//------------------------------------------------------------------------------
// framecomputealgorithm.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framecomputealgorithm.h"

namespace Frame2
{

__ImplementClass(Frame2::FrameComputeAlgorithm, 'FRCA', Frame2::FrameOp);
//------------------------------------------------------------------------------
/**
*/
FrameComputeAlgorithm::FrameComputeAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameComputeAlgorithm::~FrameComputeAlgorithm()
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/
void
FrameComputeAlgorithm::Discard()
{
	FrameOp::Discard();
	this->alg->Discard();
	this->alg = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
FrameComputeAlgorithm::Run(const IndexT frameIndex)
{
	this->alg->Execute(this->func, frameIndex);
}

} // namespace Frame2