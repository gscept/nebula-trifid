#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectVariable
    
    An EffectVariable is an interface which we use to set variable values.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifdef AFX_API
#error "afxapi.h included before effectvariable.h"
#endif

#include "EASTL/string.h"
#include "vartypes.h"
#include "annotable.h"
namespace AnyFX
{
class EffectVarblock;
class InternalEffectVariable;
class EffectVariable : public Annotable
{
public:

	/// constructor
	EffectVariable();
	/// destructor
	virtual ~EffectVariable();

	/// discard varblock
	void Discard();

	/// get name of variable
	const eastl::string& GetName() const;
	/// get type of variable
	const VariableType& GetType() const;

	/// returns true if variable has any use whatsoever in the underlying structure
	const bool IsActive() const;
	/// returns true if variable lies within a varblock
	const bool IsInVarblock() const;
    /// returns true if variable is a subroutine
    const bool IsSubroutine() const;

	/// sets variables in the currently activate program
	void Commit();

	/// set float value
	void SetFloat(float f);
	/// set float2 vector
	void SetFloat2(const float* vec);
	/// set float4 vector
	void SetFloat4(const float* vec);
	/// set float array
	void SetFloatArray(const float* f, size_t count);
	/// set float2 array
	void SetFloat2Array(const float* f, size_t count);
	/// set float4 array
	void SetFloat4Array(const float* f, size_t count);
	/// set int value
	void SetInt(int i);
	/// set int2 vector
	void SetInt2(const int* vec);
	/// set int4 vector
	void SetInt4(const int* vec);
	/// set int array
	void SetIntArray(const int* i, size_t count);
	/// set int2 array
	void SetInt2Array(const int* i, size_t count);
	/// set int4 array
	void SetInt4Array(const int* i, size_t count);
	/// set bool
	void SetBool(bool b);
	/// set bool2 vector
	void SetBool2(const bool* vec);
	/// set bool4 vector
	void SetBool4(const bool* vec);
	/// set bool array
	void SetBoolArray(const bool* b, size_t count);
	/// set bool2 array
	void SetBool2Array(const bool* b, size_t count);
	/// set bool4 array
	void SetBool4Array(const bool* b, size_t count);
	/// set matrix 
	void SetMatrix(const float* mat);
	/// set matrix array
	void SetMatrixArray(const float* mat, size_t count);
	/// set texture, textures must be an object of implementation type, i.e. OpenGLTexture
	void SetTexture(void* handle);
	/// set texture handle, using bindless textures, must be an object of implementation type, i.e. OpenGLTextureHandle
	void SetTextureHandle(void* handle);

	/// converts effect variable type to string
	static eastl::string TypeToString(const VariableType& type);
	/// converts effect variable type to byte size
	static unsigned TypeToByteSize(const VariableType& type);

	struct OpenGLTextureBinding
	{
        bool bindless;
        struct BoundTexture
        {
		    int textureType;
		    int handle;
        } bound;

        struct BindlessTexture
        {
            uint64_t handle;
        } notbound;        
	};

	struct DirectXTextureBinding
	{
		void* handle;
	};

private:
    friend class EffectProgramStreamLoader;
	friend class EffectVariableStreamLoader;
	friend class EffectVarblockStreamLoader;
	friend class EffectSamplerStreamLoader;
	friend class InternalEffectProgram;
	friend class EffectStreamLoader;

	InternalEffectVariable* internalVariable;

	char* currentValue;
}; 

} // namespace AnyFX
//------------------------------------------------------------------------------
