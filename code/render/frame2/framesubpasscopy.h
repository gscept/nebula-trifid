#pragma once
//------------------------------------------------------------------------------
/**
	Performs a copy operation within a subpass, using a fragment shader instead
	of any 'native' copy method.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
namespace Frame2
{
class FrameSubpassCopy : public FrameOp
{
	__DeclareClass(FrameSubpassCopy);
public:
	/// constructor
	FrameSubpassCopy();
	/// destructor
	virtual ~FrameSubpassCopy();
private:
};
} // namespace Frame2