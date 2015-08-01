#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::EffectRenderState
    
    An EffectRenderState is an interface used to set the current drawing state before rendering
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

#ifdef AFX_API
#error "afxapi.h included before effectrenderstate.h"
#endif

#include "EASTL/string.h"
#include "annotable.h"
namespace AnyFX
{
class InternalEffectRenderState;
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

	/// constructor
	EffectRenderState();
	/// destructor
	virtual ~EffectRenderState();

	/// discard program
	void Discard();
    /// resets render state to the default one
    void Reset();

	/// applies render state
	void Apply();
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
private:
	friend class EffectRenderStateStreamLoader;
	friend class EffectProgramStreamLoader;

	InternalEffectRenderState* internalRenderState;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------