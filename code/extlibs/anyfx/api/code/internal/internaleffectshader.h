#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectShader
    
    EffectShader backend, inherit this class to provide an implementation
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "effectshader.h"
#include <string>
#include <string.h>
namespace AnyFX
{
class Effect;
class InternalEffectShader
{
public:

	enum InternalShaderType
	{
		VertexShader,
		PixelShader,
		GeometryShader,
		HullShader,
		DomainShader,		
		ComputeShader,

		NumInternalShaderTypes
	};

	/// constructor
	InternalEffectShader();
	/// destructor
	virtual ~InternalEffectShader();

protected:
	friend class EffectShader;
	friend class EffectShaderStreamLoader;
	friend class EffectProgramStreamLoader;

	/// compiles shader with given code
	virtual bool Compile();

	/// returns name
    const eastl::string& GetName() const;
	/// returns error string
    const eastl::string& GetError() const;
	/// returns warning string
    const eastl::string& GetWarning() const;
	/// returns code
    const eastl::string& GetCode() const;

    /// returns list of local sizes used for compute shaders
    const unsigned* GetLocalSizes() const;

	Effect* effect;
	InternalShaderType type;
	eastl::string sourceCode;
	eastl::string name;
	eastl::string error;
	eastl::string warning;
    unsigned localSizes[3];
}; 


//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectShader::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectShader::GetError() const
{
	return this->error;
}

//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectShader::GetWarning() const
{
	return this->warning;
}

//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectShader::GetCode() const
{
	return this->sourceCode;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned* 
InternalEffectShader::GetLocalSizes() const
{
    return this->localSizes;
}

} // namespace AnyFX
//------------------------------------------------------------------------------