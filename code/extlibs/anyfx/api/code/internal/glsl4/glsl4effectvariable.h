#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::GLSL4EffectVariable
    
    GLSL4 backend for EffectVariable, implements InternalEffectVariable.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "internal/internaleffectvariable.h"
#include "GL/glew.h"
#include <unordered_map>
#include "EASTL/hash_map.h"
namespace AnyFX
{
class InternalEffectProgram;
class GLSL4EffectVariable : public InternalEffectVariable
{
public:
	/// constructor
	GLSL4EffectVariable();
	/// destructor
	virtual ~GLSL4EffectVariable();

private:
	friend class GLSL4EffectSampler;
	friend class GLSL4EffectProgram;

	/// sets up variable from program, override in subclass
    void Setup(eastl::vector<InternalEffectProgram*> programs, const eastl::string& defaultValue);
    /// sets up variable from program as slave, this basically just adds program-uniform pairs
    void SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVariable* master);
	/// sets up texture-specific stuff
	void MakeTexture();
	/// activates variable, this makes the uniform location be the one found in the given program
	void Activate(InternalEffectProgram* program);

	/// apply variable, only applies to textures
	void Apply();
	/// commits variable to glsl shader
	void Commit();

	eastl::hash_map<GLint, GLint> uniformProgramMap;
	GLSL4EffectProgram* activeProgram;
	GLuint activeProgramHandle;
	GLint uniformLocation;
	GLint textureUnit;
	GLenum textureType;
	GLenum glAccessMode;
	GLenum glImageFormat;

	
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------