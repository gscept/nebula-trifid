#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectShader
    
    An EffectShader bears no actual use except for getting raw byte code.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifdef AFX_API
#error "afxapi.h included before effectshader.h"
#endif

#include "EASTL/string.h"
namespace AnyFX
{
class InternalEffectShader;
class EffectShader
{
public:

	/// constructor
	EffectShader();
	/// destructor
	virtual ~EffectShader();

	/// discard program
	void Discard();

	/// return name of shader
    const eastl::string& GetName() const;
	/// return raw shader code
    const eastl::string& GetCode() const;
	/// return error, only possibly viable after compilation
    const eastl::string& GetError() const;

    /// return list of local sizes (only viable for compute shaders)
    const unsigned* GetLocalSizes() const;

private:
	friend class EffectShaderStreamLoader;
	friend class EffectProgramStreamLoader;
	
	InternalEffectShader* internalShader;
}; 


} // namespace AnyFX
//------------------------------------------------------------------------------