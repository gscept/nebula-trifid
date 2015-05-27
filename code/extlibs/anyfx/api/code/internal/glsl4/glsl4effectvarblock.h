#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::GLSL4EffectVarblock
    
    GLSL4 backend for EffectVarblock, implements InternalEffectVarblock.

    It implements a double buffered backend for each varblock inside the AnyFX shader.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "EASTL/hash_map.h"
#include "GL/glew.h"
#include "internal/internaleffectvarblock.h"
#include "glsl4bufferlock.h"
namespace AnyFX
{
class GLSL4EffectProgram;
class GLSL4EffectVarblock : public InternalEffectVarblock
{
public:
	/// constructor
	GLSL4EffectVarblock();
	/// destructor
	virtual ~GLSL4EffectVarblock();

protected:

	/// sets up variable block from program
	void Setup(eastl::vector<InternalEffectProgram*> programs);
	/// sets up varblock from programs using a pre-existing variable block
	void SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarblock* master);

    /// set variable
    void SetVariable(InternalEffectVariable* var, void* value);
    /// set variable array
    void SetVariableArray(InternalEffectVariable* var, void* value, size_t size);
    /// set variable indexed
    void SetVariableIndexed(InternalEffectVariable* var, void* value, unsigned i);

    /// locks buffer
    void LockBuffer();
    /// unlock buffer
    void UnlockBuffer();

	/// sets if the buffer should flush manually, reallocates the buffer storage with different flags
	void SetFlushManually(bool b);
	/// flushes buffer
	void FlushBuffer();

	/// binds varblock
	void Apply();
	/// updates variable block
	void Commit();
    /// puts a sync fence in the GL queue
    void PostDraw();
	/// activates varblock uniform location
	void Activate(InternalEffectProgram* program);

	GLSL4BufferLock* bufferLock;
	GLuint activeProgram;
	GLuint uniformBlockBinding;
	GLuint uniformBlockLocation;
	static const unsigned NumAuxBuffers = 6;
	GLuint buffer;
	GLuint* auxBuffers;
	GLuint* elementIndex;
    GLchar* glBuffer;
	GLchar* glBackingBuffer;
    GLuint* glBufferOffset;
	GLsizei bufferSize;
    GLuint alignedSize;

	GLint currentLocation;
	eastl::hash_map<GLSL4EffectProgram*, GLint> activeMap;
	eastl::vector<GLboolean>* ringLocks;
}; 

struct GLSL4VarblockRangeState
{
	GLuint buffer;
	GLuint offset;
	GLsizei length;
} static GLSL4VarblockRangeStates[16384];

struct GLSL4VarblockBaseState
{
	GLuint buffer;
} static GLSL4VarblockBaseStates[16384];

//------------------------------------------------------------------------------
/**
*/
inline bool
operator!=(const GLSL4VarblockRangeState& lhs, const GLSL4VarblockRangeState& rhs)
{
	return lhs.buffer != rhs.buffer || lhs.offset != rhs.offset || lhs.length != rhs.length;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
operator!=(const GLSL4VarblockBaseState& lhs, const GLSL4VarblockBaseState& rhs)
{
	return lhs.buffer != rhs.buffer;
}

} // namespace AnyFX
//------------------------------------------------------------------------------