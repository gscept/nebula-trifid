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

class Annotable
{
public:
	/// get int value
	int GetAnnotationInt(const std::string& name);
	/// get bool value
	bool GetAnnotationBool(const std::string& name);
	/// get double value
	double GetAnnotationDouble(const std::string& name);
	/// get float value
	float GetAnnotationFloat(const std::string& name);
	/// get string value
	const std::string& GetAnnotationString(const std::string& name);
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
	const std::string& GetName() const;

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
	const std::string& GetName() const;
};

class EffectShader
{
public:
	/// return name of shader
	const std::string& GetName() const;
	/// return raw shader code
	const std::string& GetCode() const;
	/// return error, only possibly viable after compilation
	const std::string& GetError() const;	
};

class EffectVarblock : public Annotable
{
public:
	/// commits varblock
	void Commit();
	/// returns name of varblock
	const std::string& GetName() const;	

	/// set buffer to be manually flushed, this requires explicit calls to FlushBuffer in order to perform updates
	void SetFlushManually(bool b);
	/// flush buffer, this only works on buffers which are tagged to be flushed manually
	void FlushBuffer();
};

class EffectVarbuffer : public Annotable
{
public:
	/// commits varblock
	void Commit();
	/// returns name of varblock
	const std::string& GetName() const;

	/// set buffer, must be an implementation specific
	void SetBuffer(void* handle);

	struct OpenGLBuffer
	{
		int handle;
		unsigned offset;
		unsigned size;
		bool bindRange;
	};

	struct DirectXBuffer
	{
		void* handle;
	};
};

class EffectVariable : public Annotable
{
public:
	/// get name of variable
	const std::string& GetName() const;
	/// get type of variable
	const VariableType& GetType() const;

	/// returns true if variable has any use whatsoever in the underlying structure
	const bool IsActive() const;

	/// returns true if variable lies within a varblock
	const bool IsInVarblock() const;

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

	struct OpenGLTexture
	{
		int textureType;
		int texture;
	};

	struct OpenGLTextureHandle
	{
		int64_t handle;
	};
};

class EffectProgram : public Annotable
{
public:
	/// applies program shaders and render states
	void Apply();
	/// commits changes to shader variables, call this before rendering
	void Commit();
    /// performs pre-draw stuff
    void PreDraw();
    /// performs post-draw stuff
    void PostDraw();
	/// returns name of program
	const std::string& GetName() const;
	/// returns render state
	EffectRenderState* GetRenderState() const;	

	/// returns true if the program requires patch-based rendering
	/// the OpenGL4 backend automatically sets the patch size
	const bool SupportsTessellation() const;

	/// returns true if linking of program was successful
	bool IsValid();
	/// returns linking error string
	const std::string& GetError() const;
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
	EffectProgram* GetProgramByName(const std::string& name);
	/// returns all programs as a list
	EffectProgram** GetPrograms() const;

	/// returns number of shaders
	unsigned GetNumShaders() const;
	/// returns shader by index
	EffectShader* GetShaderByIndex(unsigned i) const;
	/// returns shader by name
	EffectShader* GetShaderByName(const std::string& name);
	/// returns shaders as a list
	EffectShader** GetShaders() const;

	/// returns number of render states
	unsigned GetNumRenderStates() const;
	/// returns render state by index
	EffectRenderState* GetRenderStateByIndex(unsigned i) const;
	/// returns render state by name
	EffectRenderState* GetRenderStateByName(const std::string& name);
	/// returns render states as a list
	EffectRenderState** GetRenderStates() const;

	/// returns number of variables
	unsigned GetNumVariables() const;
	/// returns variable by index
	EffectVariable* GetVariableByIndex(unsigned i) const;
	/// returns variable by name
	EffectVariable* GetVariableByName(const std::string& name);
	/// returns variables as a list
	EffectVariable** GetVariables() const;

	/// returns number of varblocks
	unsigned GetNumVarblocks() const;
	/// returns varblock by index
	EffectVarblock* GetVarblockByIndex(unsigned i) const;
	/// returns varblock by name
	EffectVarblock* GetVarblockByName(const std::string& name);
	/// returns varblocks as a list
	EffectVarblock** GetVarblocks() const;

    /// returns number of varbuffers
    unsigned GetNumVarbuffers() const;
    /// returns varbuffer by index
    EffectVarbuffer* GetVarbufferByIndex(unsigned i) const;
    /// returns varbuffer by name
    EffectVarbuffer* GetVarbufferByName(const std::string& name);
    /// returns varbuffer as a list
    EffectVarbuffer** GetVarbuffers() const;

    /// returns number of subroutine
    unsigned GetNumSubroutines() const;
    /// returns subroutine by index
    EffectSubroutine* GetSubroutineByIndex(unsigned i) const;
    /// returns subroutine by name
    EffectSubroutine* GetSubroutineByName(const std::string& name);
    /// returns list of subroutines
    EffectSubroutine** GetSubroutines() const;

	/// returns the number of samplers
	unsigned GetNumSamplers() const;
	/// returns sampler by index
	EffectSampler* GetSamplerByIndex(unsigned i) const;
	/// returns sampler by name
	EffectSampler* GetSamplerByName(const std::string& name);
	/// returns samplers as list
	EffectSampler** GetSamplers() const;

	/// get type
	const Effect::EffectType& GetType() const;
	/// get major version number
	int GetMajor() const;
	/// get minor version number
	int GetMinor() const;
};

}
