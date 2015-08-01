#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::InternalEffectVariable
    
    EffectVariable backend, inherit this class to provide an implementation
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "effectvariable.h"
#include "types.h"
#include "autoref.h"
#include <string>
#include "EASTL/vector.h"
#include "EASTL/hash_map.h"
namespace AnyFX
{

class Effect;
class InternalEffectProgram;
class InternalEffectVarblock;
class InternalEffectSampler;
class InternalEffectVariable : public AutoRef
{
public:

	enum ImageFormat
	{
		RGBA32F,
		RGBA16F,
		RG32F,
		RG16F,
		R11G11B10F,
		R32F,
		R16F,
		RGBA16,
		RGB10A2,
		RGBA8,
		RG16,
		RG8,
		R16,
		R8,
		RGBA16SNORM,
		RGBA8SNORM,
		RG16SNORM,
		RG8SNORM,
		R16SNORM,
		R8SNORM,
		RGBA32I,
		RGBA16I,
		RGBA8I,
		RG32I,
		RG16I,
		RG8I,
		R32I,
		R16I,
		R8I,
		RGBA32UI,
		RGBA16UI,
		RGB10A2UI,
		RGBA8UI,
		RG32UI,
		RG16UI,
		RG8UI,
		R32UI,
		R16UI,
		R8UI,
		NoFormat,

		NumImageFormats
	};

	enum AccessMode
	{
		Read,
		Write,
		ReadWrite,
		NoAccess,

		NumImageAccessModes
	};

	/// constructor
	InternalEffectVariable();
	/// destructor
	virtual ~InternalEffectVariable();

    /// returns name of variable
    const eastl::string& GetName() const;
    /// returns signature
    const eastl::string& GetSignature() const;
    /// returns type of variable
    const VariableType& GetType() const;

protected:

    friend class GLSL4EffectVarblock;
	friend class EffectVariable;
    friend class EffectVarblock;
	friend class InternalEffectVarblock;
	friend class InternalEffectProgram;
	friend class EffectStreamLoader;
	friend class EffectVariableStreamLoader;
	friend class EffectVarblockStreamLoader;	

	/// sets up variable from program, override in subclass
    virtual void Setup(eastl::vector<InternalEffectProgram*> program, const eastl::string& defaultValue);
    /// sets up variable from another variable
    virtual void SetupSlave(eastl::vector<InternalEffectProgram*> program, InternalEffectVariable* master);
	/// sets up texture-specific stuff
	virtual void MakeTexture();
	/// sets up default value
    void SetupDefaultValue(const eastl::string& string);
	/// initializes value if variable lies in varblock
	void InitializeDefaultValues();

	/// activates variable, this makes the uniform location be the one found in the given program
	virtual void Activate(InternalEffectProgram* program);
	/// deactivates variable
	virtual void Deactivate();
	/// applies variable
	virtual void Apply();
	/// commits changes of variable, override if required
	virtual void Commit();
	
	/// returns true if variable lies within a varblock
	const bool IsInVarblock() const;
    /// returns true if variable is a subroutine variable
    const bool IsSubroutine() const;

	/// set float value, override in back-end implementation
	void SetFloat(float f);
	/// set float2 vector, override in back-end implementation
	void SetFloat2(const float* vec);
	/// set float3 vector, override in back-end implementation
	void SetFloat3(const float* vec);
	/// set float4 vector, override in back-end implementation
	void SetFloat4(const float* vec);
	/// set float value, override in back-end implementation
	void SetFloatArray(const float* f, size_t count);
	/// set float2 vector, override in back-end implementation
	void SetFloat2Array(const float* vec, size_t count);
	/// set float3 vector, override in back-end implementation
	void SetFloat3Array(const float* vec, size_t count);
	/// set float4 vector, override in back-end implementation
	void SetFloat4Array(const float* vec, size_t count);
	/// set int value, override in back-end implementation
	void SetInt(int i);
	/// set int2 vector, override in back-end implementation
	void SetInt2(const int* vec);
	/// set int3 vector, override in back-end implementation
	void SetInt3(const int* vec);
	/// set int4 vector, override in back-end implementation
	void SetInt4(const int* vec);
	/// set int value, override in back-end implementation
	void SetIntArray(const int* i, size_t count);
	/// set int2 vector, override in back-end implementation
	void SetInt2Array(const int* vec, size_t count);
	/// set int3 vector, override in back-end implementation
	void SetInt3Array(const int* vec, size_t count);
	/// set int4 vector, override in back-end implementation
	void SetInt4Array(const int* vec, size_t count);
	/// set bool, override in back-end implementation
	void SetBool(bool b);
	/// set bool2 vector, override in back-end implementation
	void SetBool2(const bool* vec);
	/// set bool3 vector, override in back-end implementation
	void SetBool3(const bool* vec);
	/// set bool4 vector, override in back-end implementation
	void SetBool4(const bool* vec);
	/// set bool, override in back-end implementation
	void SetBoolArray(const bool* b, size_t count);
	/// set bool2 vector, override in back-end implementation
	void SetBool2Array(const bool* vec, size_t count);
	/// set bool3 vector, override in back-end implementation
	void SetBool3Array(const bool* vec, size_t count);
	/// set bool4 vector, override in back-end implementation
	void SetBool4Array(const bool* vec, size_t count);
	/// set matrix, override in back-end implementation
	void SetMatrix(const float* mat);
	/// set matrix array, override in back-end implementation
	void SetMatrixArray(const float* mat, size_t count);
	/// set texture, override in back-end implementation
	void SetTexture(void* handle);

    /// set float value indexed, override in back-end implementation
    void SetFloatIndexed(float f, unsigned index);
    /// set float2 vector indexed, override in back-end implementation
    void SetFloat2Indexed(const float* vec, unsigned index);
    /// set float3 vector indexed, override in back-end implementation
    void SetFloat3Indexed(const float* vec, unsigned index);
    /// set float4 vector indexed, override in back-end implementation
    void SetFloat4Indexed(const float* vec, unsigned index);
    /// set int value indexed, override in back-end implementation
    void SetIntIndexed(int i, unsigned index);
    /// set int2 vector indexed, override in back-end implementation
    void SetInt2Indexed(const int* vec, unsigned index);
    /// set int3 vector indexed, override in back-end implementation
    void SetInt3Indexed(const int* vec, unsigned index);
    /// set int4 vector indexed, override in back-end implementation
    void SetInt4Indexed(const int* vec, unsigned index);
    /// set bool indexed, override in back-end implementation
    void SetBoolIndexed(bool b, unsigned index);
    /// set bool2 vector indexed, override in back-end implementation
    void SetBool2Indexed(const bool* vec, unsigned index);
    /// set bool3 vector indexed, override in back-end implementation
    void SetBool3Indexed(const bool* vec, unsigned index);
    /// set bool4 vector indexed, override in back-end implementation
    void SetBool4Indexed(const bool* vec, unsigned index);
    /// set matrix indexed, override in back-end implementation
    void SetMatrixIndexed(const float* mat, unsigned index);
    /// set texture indexed, override in back-end implementation
    void SetTextureIndexed(void* handle, unsigned index);

	InternalEffectVarblock* parentBlock;
	bool isInVarblock;
	eastl::hash_map<unsigned, unsigned> blockOffsets;	// byte offset for the parent block indexed as program/offset
    
    unsigned byteSize;										// byte size
	unsigned byteOffset;									// byte offset from beginning of parent block
    unsigned* sharedByteOffset;

	VariableType type;
	ImageFormat format;
	AccessMode access;
	bool active;

	bool isDirty;
	bool isArray;
    bool isSubroutine;
	int arraySize;
	int commitSize;
	
    eastl::string name;
    eastl::string signature;

	static unsigned globalTextureCounter;
    bool bindless;
	bool hasDefaultValue;
	char* currentValue;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectVariable::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const eastl::string&
InternalEffectVariable::GetSignature() const
{
	return this->signature;
}

//------------------------------------------------------------------------------
/**
*/
inline const VariableType& 
InternalEffectVariable::GetType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
InternalEffectVariable::IsInVarblock() const
{
	return this->isInVarblock;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
InternalEffectVariable::IsSubroutine() const
{
    return this->isSubroutine;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
