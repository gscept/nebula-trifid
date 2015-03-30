//------------------------------------------------------------------------------
//  glsl4bufferlock.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "glsl4bufferlock.h"
#include <assert.h>

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
GLSL4BufferLock::GLSL4BufferLock()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GLSL4BufferLock::~GLSL4BufferLock()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::Setup(size_t numBuffers)
{
	InternalBufferLock::Setup(numBuffers);
	this->rings.resize(numBuffers);
	for (unsigned i = 0; i < this->rings.size(); i++)
	{
		this->rings[i] = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::WaitForRange(size_t start, size_t length)
{
	BufferRange range = { start, length };
	this->accumulatedRanges.push_back(range);
	eastl::vector<BufferRangeSync>::iterator it = this->locks.begin();
	//std::unordered_map<BufferRange, GLsync>::iterator it = this->locks.begin();
	while (it != this->locks.end())
	{
		if (range.Overlaps(it->range))
		{
			this->Wait(it->sync);
			this->Cleanup(it->sync);
			it = this->locks.erase(it);
		}
		else
		{
			it++;
		}
	}

	// swap locks, this removes the ranges we wait for and have already cleaned up
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::LockRange(size_t start, size_t length)
{
	BufferRange range = { start, length };
	GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	this->locks.push_back({ range, sync });
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::LockAccumulatedRanges()
{
	// iterate through and lock ranges
	for (eastl::vector<BufferRange>::iterator it = this->accumulatedRanges.begin(); it != this->accumulatedRanges.end(); it++)
	{
		//this->locks[*it] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		this->locks.push_back({ *it, glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0) });
	}
	this->accumulatedRanges.clear();
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::Wait(GLsync sync)
{
	GLbitfield waitFlags = 0;
	GLuint64 waitDuration = 0;
	while (true)
	{
		GLenum waitRet = glClientWaitSync(sync, waitFlags, waitDuration);
		if (waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED) return;
		if (waitRet == GL_WAIT_FAILED)
		{
			return;
		}

		// After the first time, need to start flushing, and wait for a long time.
		waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
		waitDuration = 1000000000;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::Cleanup(GLsync sync)
{
	glDeleteSync(sync);
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::WaitForRing()
{
	if (this->currentBuffer == 0)
	{
		const GLsync& sync = this->rings[0];
		if (sync != 0)
		{
			this->Wait(sync);
			this->Cleanup(sync);
			this->rings[0] = 0;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4BufferLock::LockRing()
{
	if (this->currentBuffer == 0)
	{
		this->rings[0] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	/*
	if (this->rings[this->currentBuffer] == 0)
	{
		this->rings[this->currentBuffer] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}
	*/
}

} // namespace AnyFX