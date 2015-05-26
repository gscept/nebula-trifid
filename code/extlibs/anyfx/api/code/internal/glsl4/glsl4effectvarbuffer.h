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

	GLuint currentLocation;
	eastl::hash_map<GLSL4EffectProgram*, GLuint> activeMap;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------