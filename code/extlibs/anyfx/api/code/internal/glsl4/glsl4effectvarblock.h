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

	/// binds varblock
	void Apply();
	/// updates variable block
	void Commit();
	/// activates varblock uniform location
	void Activate(InternalEffectProgram* program);

    /// sets up uniform block offsets
    void SetupUniformOffsets(GLSL4EffectProgram* program, GLuint blockIndex);

	GLSL4BufferLock* bufferLock;
	GLuint activeProgram;
	GLuint uniformBlockBinding;
	GLuint uniformBlockLocation;

	GLuint* elementIndex;
    GLchar* glBuffer;
	GLchar* glBackingBuffer;
    GLuint* glBufferOffset;
	GLsizei bufferSize;

    unsigned* uniformOffsets;
	GLint currentLocation;
	eastl::hash_map<GLSL4EffectProgram*, GLint> activeMap;
	eastl::vector<GLboolean>* ringLocks;
}; 

struct GLSL4VarblockRangeState
{
	GLuint buffer;
	GLuint offset;
	GLsizei length;

	GLSL4VarblockRangeState()
	{
		buffer = 0;
		offset = 0;
		length = 0;
	}

} static GLSL4VarblockRangeStates[512];

struct GLSL4VarblockBaseState
{
	GLuint buffer;

	GLSL4VarblockBaseState()
	{
		buffer = 0;
	}

} static GLSL4VarblockBaseStates[512];

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