#pragma once
//------------------------------------------------------------------------------
/**
	A subpass is a subset of attachments declared by pass, and if depth should be used.
	
	Subpasses can be dependent on each other, and can declare which attachments in the pass
	should be passed between them. 

	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
namespace Frame2
{
class FrameSubpass : public FrameOp
{
	__DeclareClass(FrameSubpass);
public:
	/// constructor
	FrameSubpass();
	/// destructor
	virtual ~FrameSubpass();

	/// add frame operation
	void AddOp(const Ptr<Frame2::FrameOp>& op);

	/// run operation
	void Run(const IndexT frameIndex);
private:
	Util::Array<Ptr<Frame2::FrameOp>> ops;
};
} // namespace Frame2