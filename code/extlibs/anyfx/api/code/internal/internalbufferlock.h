#pragma once
//------------------------------------------------------------------------------
/**
	@class AnyFX::BufferLock
	
	A buffer lock is used by the varbuffer and varblock classes to lock a range of memory from being written to by the CPU.
	
	(C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "EASTL/vector.h"

namespace AnyFX
{
struct BufferRange
{
	size_t start;
	size_t length;

	bool Overlaps(const BufferRange& rhs) const
	{
		return this->start < (rhs.start + rhs.length) && rhs.start < (this->start + this->length);
	}
};

}

namespace AnyFX
{
class InternalBufferLock
{
public:
	/// constructor
	InternalBufferLock();
	/// destructor
	virtual ~InternalBufferLock();

	/// setup buffer lock
	virtual void Setup(size_t numBuffers);

protected:

	/// progess to next buffer in ring
	void NextBuffer();

	/// wait for range, implement for each API 
	virtual void WaitForRange(size_t start, size_t length);
	/// lock range, implement for each API
	virtual void LockRange(size_t start, size_t length);

	/// wait for the current ring buffer
	virtual void WaitForRing();
	/// lock the current ring
	virtual void LockRing();

	/// lock accumulated ranges
	void LockAccumulatedRanges();

	eastl::vector<BufferRange> accumulatedRanges;
	unsigned currentBuffer;
	size_t numBuffers;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
operator==(const BufferRange& lhs, const BufferRange& rhs)
{
	return lhs.start == rhs.start && lhs.length == rhs.length;
}


//------------------------------------------------------------------------------
/**
*/
inline bool
operator!=(const BufferRange& lhs, const BufferRange& rhs)
{
	return lhs.start != rhs.start || lhs.length != rhs.length;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
operator>(const BufferRange& lhs, const BufferRange& rhs)
{
	return lhs.start > rhs.start;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
operator<(const BufferRange& lhs, const BufferRange& rhs)
{
	return lhs.start < rhs.start;
}
} // namespace AnyFX