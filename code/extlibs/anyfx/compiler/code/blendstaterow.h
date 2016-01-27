#pragma once
//------------------------------------------------------------------------------
/**
    @class BlendState
    
    A blend state holds information related to per render target alpha blending.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "util.h"
#include "expressions/expression.h"
namespace AnyFX
{

class BlendStateRow
{
public:

	enum BlendModes
	{
		// blend modes
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		SrcAlphaSaturate,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,

		NumBlendModes
	};

	enum BlendOps
	{
		Add,
		Sub,
		InvSub,
		Min,
		Max,

		NumBlendOps
	};

	enum FlagType
	{
		StringFlagType,
		ExpressionFlagType,

		NumFlagTypes
	};

	enum EnumFlag
	{
		SrcBlend,
		DstBlend,
		BlendOp,
		SrcBlendAlpha,
		DstBlendAlpha,
		BlendOpAlpha,

		NumEnumFlags
	};

	enum BoolFlag
	{
		BlendEnabled,

		NumBoolFlags
	};


	/// constructor
	BlendStateRow();
	/// destructor
	virtual ~BlendStateRow();

	/// get flag type
	const FlagType& GetFlagType() const;
	/// get flag
	const std::string& GetFlag() const;

	/// get render target expression
	Expression* GetRenderTarget() const;

	/// sets integer flag
	void SetString(Expression* renderTarget, const std::string& flag, const std::string& value);
	/// get integer value
	const std::string& GetString() const;
	/// sets boolean flag
	void SetExpression(Expression* renderTarget, const std::string& flag, Expression* expr);
	/// get bool value
	Expression* GetExpression() const;

private:

	FlagType flagType;
	std::string flag;
	Expression* renderTarget;

	std::string stringValue;
	Expression* expr;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const BlendStateRow::FlagType& 
BlendStateRow::GetFlagType() const
{
	return this->flagType;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
BlendStateRow::GetFlag() const
{
	return this->flag;
}

//------------------------------------------------------------------------------
/**
*/
inline Expression* 
BlendStateRow::GetRenderTarget() const
{
	return this->renderTarget;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
BlendStateRow::GetString() const
{
	assert(this->flagType == StringFlagType);
	return this->stringValue;
}

//------------------------------------------------------------------------------
/**
*/
inline Expression* 
BlendStateRow::GetExpression() const
{
	assert(this->flagType == ExpressionFlagType);
	return this->expr;
}

} // namespace AnyFX
//------------------------------------------------------------------------------