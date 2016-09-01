#pragma once
//------------------------------------------------------------------------------
/**
	A subpass batch performs batch rendering of geometry.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
#include "frame/batchgroup.h"
namespace Frame2
{
class FrameSubpassBatch : public FrameOp
{
	__DeclareClass(FrameSubpassBatch);
public:
	/// constructor
	FrameSubpassBatch();
	/// destructor
	virtual ~FrameSubpassBatch();

	/// set batch
	void SetBatchCode(const Frame::BatchGroup::Code& code);

	/// run operation
	void Run(const IndexT frameIndex);

private:
	Frame::BatchGroup::Code batch;
};

//------------------------------------------------------------------------------
/**
*/
inline void
FrameSubpassBatch::SetBatchCode(const Frame::BatchGroup::Code& code)
{
	this->batch = code;
}

} // namespace Frame2