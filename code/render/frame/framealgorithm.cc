//------------------------------------------------------------------------------
//  framealgorithm.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framealgorithm.h"
#include "algorithm/algorithmbase.h"

using namespace Core;
namespace Frame
{
__ImplementClass(Frame::FrameAlgorithm, 'FRAL', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
FrameAlgorithm::FrameAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameAlgorithm::~FrameAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameAlgorithm::Discard()
{
	this->algorithm->Discard();
	this->algorithm = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameAlgorithm::Begin()
{
	this->algorithm->Begin();
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameAlgorithm::Render(IndexT frameIndex)
{
	this->algorithm->Render();
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameAlgorithm::End()
{
	this->algorithm->End();
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameAlgorithm::OnWindowResize(SizeT width, SizeT height)
{
	this->algorithm->OnDisplayResized(width, height);
}

} // namespace Frame