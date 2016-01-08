#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::GLSL4EffectVarbuffer
    
    Implements a shader storage block.
    
    (C) 2014 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "EASTL/hash_map.h"
#include "internal/internaleffectvarbuffer.h"
#include "GL/glew.h"
namespace AnyFX
{
class GLSL4EffectProgram;
class GLSL4EffectVarbuffer : public InternalEffectVarbuffer
{
public:
	/// constructor
	GLSL4EffectVarbuffer();
	/// destructor
	virtual ~GLSL4EffectVarbuffer();

	/// sets up variable block from program
	void Setup(eastl::vector<InternalEffectProgram*> programs);
	/// sets up varblock from programs using a pre-existing variable block
	void SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarbuffer* master);

protected:

	/// updates variable block
	void Commit();
	/// activates varblock uniform location
	void Activate(InternalEffectProgram* program);

	GLuint activeProgram;
	GLuint shaderStorageBlockBinding;
	GLuint shaderStorageBlockLocation;
	GLint offsetAlignment;

	GLuint currentLocation;
	eastl::hash_map<GLSL4EffectProgram*, GLuint> activeMap;
}; 

struct GLSL4VarbufferRangeState
{
	GLuint buffer;
	GLuint offset;
	GLsizei length;

	GLSL4VarbufferRangeState()
	{
		buffer = 0;
		offset = 0;
		length = 0;
	}

} static GLSL4VarbufferRangeStates[512];

struct GLSL4VarbufferBaseState
{
	GLuint buffer;

	GLSL4VarbufferBaseState()
	{
		buffer = 0;
	}

} static GLSL4VarbufferBaseStates[512];


//------------------------------------------------------------------------------
/**
*/
inline bool
operator!=(const GLSL4VarbufferRangeState& lhs, const GLSL4VarbufferRangeState& rhs)
{
	return lhs.buffer != rhs.buffer || lhs.offset != rhs.offset || lhs.length != rhs.length;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
operator!=(const GLSL4VarbufferBaseState& lhs, const GLSL4VarbufferBaseState& rhs)
{
	return lhs.buffer != rhs.buffer;
}

} // namespace AnyFX
//------------------------------------------------------------------------------