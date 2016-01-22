#pragma once
//------------------------------------------------------------------------------
/**
    @class DrawStateRow
    
    A state row is a single assignment of a state setting
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "util.h"
#include "expressions/expression.h"

namespace AnyFX
{

class RenderStateRow
{
public:

	enum ComparisonFunctions
	{
		Never,
		Less,
		LEqual,
		Greater,
		GEqual,
		Equal,
		NEqual,
		Always,

		NumComparisonFunctions
	};

	enum StencilOp
	{
		Keep,
		Zero,
		Replace,
		Increase,
		IncreaseWrap,
		Decrease,
		DecreaseWrap,
		Invert,

		NumStencilOps
	};

	enum CullModes
	{
		Back,
		Front,
		None,

		NumCullModes
	};

	enum FillModes
	{
		Fill,
		Line,
		Point,

		NumFillModes
	};

	enum FlagType
	{
		ExpressionFlagType,
		IntFlagType,
		StringFlagType,
		BoolFlagType,

		NumFlagTypes
	};

	enum BoolFlag
	{
		DepthEnabled,
		DepthWrite,
		DepthClamp,
		SeparateBlend,
		ScissorEnabled,
		StencilEnabled,
		AlphaToCoverageEnabled,
		MultisampleEnabled,
		PolygonOffsetEnabled,

		NumBoolFlags
	};

	enum EnumFlag
	{
		DepthFunc,
		CullMode,
		RasterizerMode,
		StencilFrontFailOp,
		StencilBackFailOp,
		StencilFrontPassOp,
		StencilBackPassOp,
		StencilFrontDepthFailOp,
		StencilBackDepthFailOp,
		StencilFrontFunc,
		StencilBackFunc,

		NumEnumFlags
	};

	enum UintFlag
	{
		StencilReadMask,
		StencilWriteMask,

		NumUintFlags
	};

	enum IntFlag
	{
		StencilFrontRef,
		StencilBackRef,

		NumIntFlags
	};

	enum FloatFlag
	{
		PolygonOffsetFactor,
		PolygonOffsetUnits,

		NumFloatFlags
	};

	/// constructor
	RenderStateRow();
	/// destructor
	virtual ~RenderStateRow();

	/// gets flag type
	const FlagType& GetFlagType() const;
	/// gets flag
	const std::string& GetFlag() const;

	/// sets expression
	void SetExpression(const std::string& flag, Expression* expr);
	/// gets expression
	Expression* GetExpression() const;
	/// sets string
	void SetString(const std::string& flag, const std::string& value);
	/// gets string
	const std::string& GetString() const;

private:

	std::string flag;
	FlagType flagType;

	Expression* expr;
	std::string stringValue;	
}; 


//------------------------------------------------------------------------------
/**
*/
inline const RenderStateRow::FlagType& 
RenderStateRow::GetFlagType() const
{
	return this->flagType;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
RenderStateRow::GetFlag() const
{
	return this->flag;
}

//------------------------------------------------------------------------------
/**
*/
inline Expression* 
RenderStateRow::GetExpression() const
{
	assert(this->flagType == ExpressionFlagType);
	return this->expr;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string&
RenderStateRow::GetString() const
{
	assert(this->flagType == StringFlagType);
	return this->stringValue;
}

} // namespace AnyFX
//------------------------------------------------------------------------------