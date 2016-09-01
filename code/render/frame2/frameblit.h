#pragma once
//------------------------------------------------------------------------------
/**
	A frame blit performs an image copy.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
namespace Frame2
{
class FrameBlit : public FrameOp
{
	__DeclareClass(FrameBlit);
public:
	/// constructor
	FrameBlit();
	/// destructor
	virtual ~FrameBlit();
private:
};
} // namespace Frame2