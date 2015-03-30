//------------------------------------------------------------------------------
//  bufferlock.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "internalbufferlock.h"
#include <assert.h>

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
InternalBufferLock::InternalBufferLock() :
	currentBuffer(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
InternalBufferLock::~InternalBufferLock()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
InternalBufferLock::Setup(size_t numBuffers)
{
	assert(numBuffers != 0);
	this->numBuffers = numBuffers;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalBufferLock::WaitForRange(size_t start, size_t length)
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
InternalBufferLock::LockRange(size_t start, size_t length)
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
InternalBufferLock::LockAccumulatedRanges()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
InternalBufferLock::NextBuffer()
{
	this->currentBuffer = (this->currentBuffer + 1) % this->numBuffers;
}

//------------------------------------------------------------------------------
/**
*/
void
InternalBufferLock::WaitForRing()
{
	// implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
InternalBufferLock::LockRing()
{
	// implement in subclass
}

} // namespace AnyFX