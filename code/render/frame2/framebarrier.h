#pragma once
//------------------------------------------------------------------------------
/**
	Implements a barrier between frame operations.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
namespace Frame2
{
class FrameBarrier : public FrameOp
{
	__DeclareClass(FrameBarrier);
public:
	/// constructor
	FrameBarrier();
	/// destructor
	virtual ~FrameBarrier();
private:
};
} // namespace Frame2