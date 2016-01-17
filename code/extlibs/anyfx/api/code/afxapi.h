#pragma once
//------------------------------------------------------------------------------
/**
    @file afxapi.h
    
    Include file for AnyFX API
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifndef AFX_API
#define AFX_API
#endif

#include <string>
#include "vartypes.h"
#include "effectfactory.h"
namespace AnyFX
{

class AutoRef;
typedef void Handle;
struct OpenGLBufferBinding
{
    int handle;
    unsigned offset;
    unsigned size;
    bool bindRange;

	OpenGLBufferBinding() : bindRange(false), offset(0), size(0), handle(0) {};
};

struct DirectXBufferBinding
{
    void* handle;
};

struct VarblockVariableBinding
{
    eastl::string name;
    unsigned offset;
    unsigned size;
    unsigned arraySize;
    char* value;
};

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

class Annotable
{
public:
    /// returns true if annotation exists
    bool HasAnnotation(const eastl::string& name);

	/// get int value
    int GetAnnotationInt(const eastl::string& name);
	/// get bool value
    bool GetAnnotationBool(const eastl::string& name);
	/// get double value
    double GetAnnotationDouble(const eastl::string& name);
	/// get float value
    float GetAnnotationFloat(const eastl::string& name);
	/// get string value
    const eastl::string& GetAnnotationString(const eastl::string& name);
};

class EffectRenderState : public Annotable
{
public:
	enum BlendMode
	{
		ZeroBlend,
		OneBlend,
		SourceColor,
		OneMinusSourceColor,
		DestinationColor,
		OneMinusDestinationColor,
		SourceAlpha,
		OneMinusSourceAlpha,
		DestinationAlpha,
		OneMinusDestinationAlpha,
		SourceAlphaSaturate,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,

		NumBlendModes
	};

	enum BlendOperation
	{
		Add,
		Sub,
		InvSub,
		Min,
		Max,

		NumBlendOperations
	};

	enum BlendBoolFlag
	{
		BlendEnabled,

		NumBlendBoolFlags
	};

	enum BlendModeFlag
	{
		SrcBlend,
		DstBlend,
		SrcBlendAlpha,
		DstBlendAlpha,

		NumBlendModeFlags
	};

	enum BlendFuncFlag
	{
		BlendOp,
		BlendOpAlpha,

		NumBlendFlags
	};

	enum StencilOperation
	{
		Keep,
		Zero,
		Replace,
		Increase,
		IncreaseWrap,
		Decrease,
		DecreaseWrap,
		Invert,

		NumStencilOperations
	};

	enum StencilOpFlag
	{
		StencilFrontFailOp,
		StencilBackFailOp,
		StencilFrontPassOp,
		StencilBackPassOp,
		StencilFrontDepthFailOp,
		StencilBackDepthFailOp,

		NumStencilOpFlags
	};

	enum StencilFuncFlag
	{
		StencilFrontFunc,
		StencilBackFunc,

		NumStencilFuncFlags
	};

	enum DrawBoolFlag
	{
		DepthEnabled,
		DepthWrite,
		DepthRead,
		SeparateBlend,
		StencilEnabled,
		AlphaToCoverageEnabled,
		MultisampleEnabled,

		NumDrawBoolFlags
	};

	enum DrawIntFlag
	{
		StencilReadMask,
		StencilWriteMask,
		DepthFunc,
		CullMode,
		FillMode,

		NumDrawIntFlags
	};

	enum ComparisonFunc
	{
		Never,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,		
		Always,

		NumDrawDepthFuncs
	};

	enum DrawCullMode
	{
		Back,
		Front,
		None,

		NumDrawCullModes
	};

	enum DrawFillMode
	{
		Solid,
		Line,
		Point,

		NumDrawFillModes
	};

	/// applies render state
	void Apply();
    /// resets render state
    void Reset();
	/// gets name of render state
    const eastl::string& GetName() const;

	/// overrides blend mode flag
	void SetBlendModeFlag(unsigned renderTarget, BlendModeFlag flag, BlendMode mode);
	/// overrides blend function flag
	void SetBlendFuncFlag(unsigned renderTarget, BlendFuncFlag flag, BlendOperation op);
	/// overrides blend bool flag
	void SetBlendBoolFlag(unsigned renderTarget, BlendBoolFlag flag, bool value);
	/// overrides stencil op flag
	void SetStencilOpFlag(StencilOpFlag flag, StencilOperation op);
	/// overrides stencil function flag
	void SetStencilFuncFlag(StencilFuncFlag flag, ComparisonFunc func);
	/// overrides depth function
	void SetDepthFunc(ComparisonFunc func);
	/// overrides fill mode
	void SetFillMode(DrawFillMode fillMode);
	/// overrides cull mode
	void SetCullMode(DrawCullMode cullMode);
	/// sets stencil read and write respectively
	void SetStencilReadWrite(unsigned read, unsigned write);	
};

class EffectSampler
{
public:

	enum AddressMode
	{
		Wrap,
		Mirror,
		Clamp,
		Border,
		MirrorOnce,

		NumAddressModes
	};

	enum FilterMode
	{
		MinMagMipPoint,
		MinMagMipLinear,
		MinMagPointMipLinear,
		MinMipPointMagLinear,
		MinPointMipMagLinear,
		MinLinearMipMagPoint,
		MinMipLinearMagPoint,
		MinMagLinearMipPoint,
		Anisotropic,
		ComparisonMinMagMipPoint,
		ComparisonMinMagMipLinear,
		ComparisonMinMagPointMipLinear,
		ComparisonMinMipPointMagLinear,
		ComparisonMinPointMipMagLinear,
		ComparisonMinLinearMipMagPoint,
		ComparisonMinMipLinearMagPoint,
		ComparisonMinMagLinearMipPoint,
		ComparisonAnisotropic,

		NumFilterModes
	};

	enum ComparisonFunc
	{
		Never,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,		
		Always,

		NumDrawDepthFuncs
	};

	/// get name of variable
    const eastl::string& GetName() const;
};

class EffectShader
{
public:
	/// return name of shader
    const eastl::string& GetName() const;
	/// return raw shader code
    const eastl::string& GetCode() const;
	/// return error, only possibly viable after compilation
    const eastl::string& GetError() const;
};

class EffectVarblock : public Annotable
{
public:
	/// commits varblock
	void Commit();
	/// returns name of varblock
    const eastl::string& GetName() const;
    /// returns signature of varblock
    const eastl::string& GetSignature() const;
    /// return size of implementation varblock
    const size_t GetSize() const;

    /// returns true if variable has any use whatsoever in the underlying structure
    const bool IsActive() const;

    /// return list of variable binding information within this varblock
    eastl::vector<VarblockVariableBinding> GetVariables() const;

    /// set buffer, must be an implementation specific
	void SetBuffer(void* handle);
};

class EffectVarbuffer : public Annotable
{
public:
	/// commits varblock
	void Commit();
	/// returns name of varblock
    const eastl::string& GetName() const;

	/// returns true if variable has any use whatsoever in the underlying structure
	const bool IsActive() const;

	/// set buffer, must be an implementation specific
	void SetBuffer(void* handle);
};

class EffectVariable : public Annotable
{
public:
	/// get name of variable
	const eastl::string& GetName() const;
	/// get type of variable
	const VariableType& GetType() const;

	/// returns true if variable has any use whatsoever in the underlying structure
	const bool IsActive() const;

	/// returns true if variable lies within a varblock
	const bool IsInVarblock() const;
    /// returns name of the varblock in which this variable resides
    const eastl::string& GetVarblockName() const;
    /// return byte offset into varblock
    const size_t GetByteOffset() const;
    /// return byte size
    const size_t GetByteSize() const;

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
	/// set texture, handle can be any integer type, such as a DirectX ShaderResourceView pointer or OpenGL integer handle
	void SetTexture(void* handle);
	/// set texture handle, using bindless textures, must be an object of implementation type, i.e. OpenGLTextureHandle
	void SetTextureHandle(void* handle);
};

class EffectProgram : public Annotable
{
public:
	/// applies program shaders and render states
	void Apply();
	/// commits changes to shader variables, call this before rendering
	void Commit();
	/// returns name of program
    const eastl::string& GetName() const;
	/// returns render state
	EffectRenderState* GetRenderState() const;	

	/// returns true if the program requires patch-based rendering
	/// the OpenGL4 backend automatically sets the patch size
	const bool SupportsTessellation() const;

	/// returns true if linking of program was successful
	bool IsValid();
	/// returns linking error string
    const eastl::string& GetError() const;
};

class Effect : public Annotable
{
public:

	enum EffectType
	{
		HLSL,
		GLSL,
		PS,
		WII,

		NumInternalEffectTypes
	};

    /// returns number of programs
    unsigned GetNumPrograms() const;
    /// returns program by index
    EffectProgram* GetProgramByIndex(unsigned i) const;
    /// returns program by name
    EffectProgram* GetProgramByName(const eastl::string& name);
    /// returns all programs as a list
    EffectProgram** GetPrograms() const;
    /// returns true if program exists
    bool HasProgram(const eastl::string& name);

    /// returns number of shaders
    unsigned GetNumShaders() const;
    /// returns shader by index
    EffectShader* GetShaderByIndex(unsigned i) const;
    /// returns shader by name
    EffectShader* GetShaderByName(const eastl::string& name);
    /// returns shaders as a list
    EffectShader** GetShaders() const;
    /// returns true if shader exists
    bool HasShader(const eastl::string& name);

    /// returns number of render states
    unsigned GetNumRenderStates() const;
    /// returns render state by index
    EffectRenderState* GetRenderStateByIndex(unsigned i) const;
    /// returns render state by name
    EffectRenderState* GetRenderStateByName(const eastl::string& name);
    /// returns render states as a list
    EffectRenderState** GetRenderStates() const;
    /// returns true if render state exists
    bool HasRenderState(const eastl::string& name);

    /// returns number of variables
    unsigned GetNumVariables() const;
    /// returns variable by index
    EffectVariable* GetVariableByIndex(unsigned i) const;
    /// returns variable by name
    EffectVariable* GetVariableByName(const eastl::string& name);
    /// returns variables as a list
    EffectVariable** GetVariables() const;
    /// returns true if variable exists
    bool HasVariable(const eastl::string& name);

    /// returns number of varblocks
    unsigned GetNumVarblocks() const;
    /// returns varblock by index
    EffectVarblock* GetVarblockByIndex(unsigned i) const;
    /// returns varblock by name
    EffectVarblock* GetVarblockByName(const eastl::string& name);
    /// returns varblocks as a list
    EffectVarblock** GetVarblocks() const;
    /// returns true if varblock exists
    bool HasVarblock(const eastl::string& name);

    /// returns number of varbuffers
    unsigned GetNumVarbuffers() const;
    /// returns varbuffer by index
    EffectVarbuffer* GetVarbufferByIndex(unsigned i) const;
    /// returns varbuffer by name
    EffectVarbuffer* GetVarbufferByName(const eastl::string& name);
    /// returns varbuffer as a list
    EffectVarbuffer** GetVarbuffers() const;
    /// returns true if varbuffer exists
    bool HasVarbuffer(const eastl::string& name);

    /// returns number of subroutine
    unsigned GetNumSubroutines() const;
    /// returns subroutine by index
    EffectSubroutine* GetSubroutineByIndex(unsigned i) const;
    /// returns subroutine by name
    EffectSubroutine* GetSubroutineByName(const eastl::string& name);
    /// returns list of subroutines
    EffectSubroutine** GetSubroutines() const;
    /// returns true if subroutine exists
    bool HasSubroutine(const eastl::string& name);

    /// returns the number of samplers
    unsigned GetNumSamplers() const;
    /// returns sampler by index
    EffectSampler* GetSamplerByIndex(unsigned i) const;
    /// returns sampler by name
    EffectSampler* GetSamplerByName(const eastl::string& name);
    /// returns samplers as list
    EffectSampler** GetSamplers() const;
    /// returns true if sampler exists
    bool HasSampler(const eastl::string& name);

	/// get type
	const Effect::EffectType& GetType() const;
	/// get major version number
	int GetMajor() const;
	/// get minor version number
	int GetMinor() const;
};

}
