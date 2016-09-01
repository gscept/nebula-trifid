#pragma once
//------------------------------------------------------------------------------
/**
	A subpass sorted batch renders the same geometry as the ordinary batch, however
	it prioritizes Z-order instead shader, making it potentially detrimental for performance.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "frameop.h"
#include "frame/batchgroup.h"
namespace Frame2
{
class FrameSubpassOrderedBatch : public FrameOp
{
	__DeclareClass(FrameSubpassOrderedBatch);
public:
	/// constructor
	FrameSubpassOrderedBatch();
	/// destructor
	virtual ~FrameSubpassOrderedBatch();

	/// set batch
	void SetBatchCode(const Frame::BatchGroup::Code& code);

	/// setup operation
	void Setup();
	/// run operation
	void Run(const IndexT frameIndex);

private:
	Frame::BatchGroup::Code batch;
};

//------------------------------------------------------------------------------
/**
*/
inline void
FrameSubpassOrderedBatch::SetBatchCode(const Frame::BatchGroup::Code& code)
{
	this->batch = code;
}
} // namespace Frame2