#pragma once
//------------------------------------------------------------------------------
/**
	@class AnyFX::GLSL4BufferLock
	
	Implements a buffer locking system for use with OpenGL.
	
	(C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "EASTL/vector.h"
#include "internal/internalbufferlock.h"
#include "GL/glew.h"
namespace AnyFX
{

struct BufferRangeSync
{
	BufferRange range;
	GLsync sync;
};
class GLSL4BufferLock : public InternalBufferLock
{
public:
	/// constructor
	GLSL4BufferLock();
	/// destructor
	virtual ~GLSL4BufferLock();

	/// setup buffer
	void Setup(size_t numBuffers);

private:
	friend class GLSL4EffectVarblock;
	friend class GLSL4EffectVarbuffer;

	/// wait for range, implement for each API 
	void WaitForRange(size_t start, size_t length);
	/// lock range, implement for each API
	void LockRange(size_t start, size_t length);

	/// wait for ring
	void WaitForRing();
	/// lock ring
	void LockRing();

	/// lock accumulated ranges
	void LockAccumulatedRanges();

	/// perform waiting
	void Wait(GLsync sync);
	/// cleanup sync object
	void Cleanup(GLsync sync);

	eastl::vector<BufferRangeSync> locks;
	eastl::vector<GLsync> rings;
	//std::unordered_map<BufferRange, GLsync> locks;
};
} // namespace AnyFX