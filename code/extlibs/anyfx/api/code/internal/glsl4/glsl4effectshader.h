#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::GLSL4EffectShader
    
    GLSL4 backend for EffectShader, implements InternalEffectShader.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "internal/internaleffectshader.h"
#include "GL/glew.h"
namespace AnyFX
{
class GLSL4EffectShader : public InternalEffectShader
{
public:
	/// constructor
	GLSL4EffectShader();
	/// destructor
	virtual ~GLSL4EffectShader();

	/// returns shader handle
	GLuint GetShaderHandle() const;
private:

	/// compiles shader
	bool Compile();

	GLuint shaderHandle;
}; 

//------------------------------------------------------------------------------
/**
*/
inline GLuint 
GLSL4EffectShader::GetShaderHandle() const
{
	return this->shaderHandle;
}

} // namespace AnyFX
//------------------------------------------------------------------------------