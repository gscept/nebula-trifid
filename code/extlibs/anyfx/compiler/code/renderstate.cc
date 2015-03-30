//------------------------------------------------------------------------------
//  drawstate.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "renderstate.h"
#include "types.h"
#include "typechecker.h"
namespace AnyFX
{


//------------------------------------------------------------------------------
/**
*/
RenderState::RenderState() :
	hasAnnotation(false),
	numEntries(0)
{
	this->symbolType = Symbol::RenderStateType;

	// set enumerated flags
	this->drawEnumFlags[RenderStateRow::StencilFrontFunc] = this->drawEnumFlags[RenderStateRow::StencilBackFunc] = RenderStateRow::Always;
	this->drawEnumFlags[RenderStateRow::StencilFrontDepthFailOp] = this->drawEnumFlags[RenderStateRow::StencilBackDepthFailOp] = RenderStateRow::Keep;
	this->drawEnumFlags[RenderStateRow::StencilFrontPassOp] = this->drawEnumFlags[RenderStateRow::StencilBackPassOp] = RenderStateRow::Keep;
	this->drawEnumFlags[RenderStateRow::StencilFrontFailOp] = this->drawEnumFlags[RenderStateRow::StencilBackFailOp] = RenderStateRow::Keep;
	this->drawEnumFlags[RenderStateRow::DepthFunc] = RenderStateRow::Less;
	this->drawEnumFlags[RenderStateRow::CullMode] = RenderStateRow::Back;
	this->drawEnumFlags[RenderStateRow::RasterizerMode] = RenderStateRow::Fill;
	
	// set default blend flags
	unsigned i;
	for (i = 0; i < MaxNumRenderTargets; i++)
	{
		this->blendBoolFlags[i][BlendStateRow::BlendEnabled] = false;
		this->blendEnumFlags[i][BlendStateRow::SrcBlend] = BlendStateRow::One;
		this->blendEnumFlags[i][BlendStateRow::DstBlend] = BlendStateRow::Zero;
		this->blendEnumFlags[i][BlendStateRow::BlendOp] = BlendStateRow::Add;
		this->blendEnumFlags[i][BlendStateRow::SrcBlendAlpha] = BlendStateRow::One;
		this->blendEnumFlags[i][BlendStateRow::DstBlendAlpha] = BlendStateRow::Zero;
		this->blendEnumFlags[i][BlendStateRow::BlendOpAlpha] = BlendStateRow::Add;	
	}

	// set default render settings
	this->drawBoolFlags[RenderStateRow::DepthEnabled] = true;
	this->drawBoolFlags[RenderStateRow::DepthWrite] = true;
	this->drawBoolFlags[RenderStateRow::DepthClamp] = true;
	this->drawBoolFlags[RenderStateRow::SeparateBlend] = false;
	this->drawBoolFlags[RenderStateRow::ScissorEnabled] = false;
	this->drawBoolFlags[RenderStateRow::StencilEnabled] = false;
	this->drawBoolFlags[RenderStateRow::MultisampleEnabled] = false;
	this->drawBoolFlags[RenderStateRow::AlphaToCoverageEnabled] = false;
	this->drawBoolFlags[RenderStateRow::PolygonOffsetEnabled] = false;
	this->drawIntFlags[RenderStateRow::StencilFrontRef] = 0;
	this->drawIntFlags[RenderStateRow::StencilBackRef] = 0;
	this->drawUintFlags[RenderStateRow::StencilReadMask] = 1;
	this->drawUintFlags[RenderStateRow::StencilWriteMask] = 1;
	this->drawFloatFlags[RenderStateRow::PolygonOffsetFactor] = 0.0f;
	this->drawFloatFlags[RenderStateRow::PolygonOffsetUnits] = 0.0f;

	for (i = 0; i < RenderStateRow::NumIntFlags; i++)
	{
		this->drawIntExpressions[i] = NULL;
	}

	for (i = 0; i < RenderStateRow::NumUintFlags; i++)
	{
		this->drawUintExpressions[i] = NULL;
	}

	for (i = 0; i < RenderStateRow::NumBoolFlags; i++)
	{
		this->drawBoolExpressions[i] = NULL;
	}

	for (i = 0; i < RenderStateRow::NumFloatFlags; i++)
	{
		this->drawFloatExpressions[i] = NULL;
	}
}

//------------------------------------------------------------------------------
/**
*/
RenderState::~RenderState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderState::ConsumeRenderRow( const RenderStateRow& row )
{
	switch (row.GetFlagType())
	{
	case RenderStateRow::StringFlagType:
		if (row.GetFlag() == "DepthFunc")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Never")							flagVal = RenderStateRow::Never;
			else if (flag == "Less")						flagVal = RenderStateRow::Less;
			else if (flag == "Lequal")						flagVal = RenderStateRow::LEqual;
			else if (flag == "Greater")						flagVal = RenderStateRow::Greater;
			else if (flag == "Gequal")						flagVal = RenderStateRow::GEqual;
			else if (flag == "Equal")						flagVal = RenderStateRow::Equal;
			else if (flag == "Nequal")						flagVal = RenderStateRow::NEqual;
			else if (flag == "Always")						flagVal = RenderStateRow::Always;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::DepthFunc] = flagVal;
		}
		else if (row.GetFlag() == "FillMode")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Fill")								flagVal = RenderStateRow::Fill;
			else if (flag == "Line")						flagVal = RenderStateRow::Line;
			else if (flag == "Point")						flagVal = RenderStateRow::Point;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::RasterizerMode] = flagVal;
		}
		else if (row.GetFlag() == "CullMode")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Back")								flagVal = RenderStateRow::Back;
			else if (flag == "Front")						flagVal = RenderStateRow::Front;
			else if (flag == "None")						flagVal = RenderStateRow::None;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::CullMode] = flagVal;
		}
		else if (row.GetFlag() == "StencilFrontFailOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontFailOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilBackFailOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilBackFailOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilFrontPassOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontPassOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilBackPassOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilBackPassOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilFrontDepthFailOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontDepthFailOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilBackDepthFailOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilBackDepthFailOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilFrontFunc")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Never")						flagVal = RenderStateRow::Never;
			else if (flag == "Less")					flagVal = RenderStateRow::Less;
			else if (flag == "Lequal")					flagVal = RenderStateRow::LEqual;
			else if (flag == "Greater")					flagVal = RenderStateRow::Greater;
			else if (flag == "Gequal")					flagVal = RenderStateRow::GEqual;
			else if (flag == "Equal")					flagVal = RenderStateRow::Equal;
			else if (flag == "Nequal")					flagVal = RenderStateRow::NEqual;
			else if (flag == "Always")					flagVal = RenderStateRow::Always;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontFunc] = flagVal;
		}
		else if (row.GetFlag() == "StencilBackFunc")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Never")						flagVal = RenderStateRow::Never;
			else if (flag == "Less")					flagVal = RenderStateRow::Less;
			else if (flag == "Lequal")					flagVal = RenderStateRow::LEqual;
			else if (flag == "Greater")					flagVal = RenderStateRow::Greater;
			else if (flag == "Gequal")					flagVal = RenderStateRow::GEqual;
			else if (flag == "Equal")					flagVal = RenderStateRow::Equal;
			else if (flag == "Nequal")					flagVal = RenderStateRow::NEqual;
			else if (flag == "Always")					flagVal = RenderStateRow::Always;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilBackFunc] = flagVal;
		}
		else if (row.GetFlag() == "StencilFailOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontFailOp] = flagVal;
			this->drawEnumFlags[RenderStateRow::StencilBackFailOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilPassOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontPassOp] = flagVal;
			this->drawEnumFlags[RenderStateRow::StencilBackPassOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilDepthFailOp")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Keep")					flagVal = RenderStateRow::Keep;
			else if (flag == "Zero")			flagVal = RenderStateRow::Zero;
			else if (flag == "Replace")			flagVal = RenderStateRow::Replace;
			else if (flag == "Increase")		flagVal = RenderStateRow::Increase;
			else if (flag == "IncreaseWrap")	flagVal = RenderStateRow::IncreaseWrap;
			else if (flag == "Decrease")		flagVal = RenderStateRow::Decrease;
			else if (flag == "DecreaseWrap")	flagVal = RenderStateRow::DecreaseWrap;
			else if (flag == "Invert")			flagVal = RenderStateRow::Invert;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontDepthFailOp] = flagVal;
			this->drawEnumFlags[RenderStateRow::StencilBackDepthFailOp] = flagVal;
		}
		else if (row.GetFlag() == "StencilFunc")
		{
			const std::string& flag = row.GetString();
			unsigned flagVal = -1;

			if (flag == "Never")						flagVal = RenderStateRow::Never;
			else if (flag == "Less")					flagVal = RenderStateRow::Less;
			else if (flag == "Lequal")					flagVal = RenderStateRow::LEqual;
			else if (flag == "Greater")					flagVal = RenderStateRow::Greater;
			else if (flag == "Gequal")					flagVal = RenderStateRow::GEqual;
			else if (flag == "Equal")					flagVal = RenderStateRow::Equal;
			else if (flag == "Nequal")					flagVal = RenderStateRow::NEqual;
			else if (flag == "Always")					flagVal = RenderStateRow::Always;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), flag };
				this->invalidValues.push_back(foo);
				return;
			}

			this->drawEnumFlags[RenderStateRow::StencilFrontFunc] = flagVal;
			this->drawEnumFlags[RenderStateRow::StencilBackFunc] = flagVal;			
		}
		else
		{
			this->invalidStringFlags.push_back(row.GetFlag());
		}
		break;
	case RenderStateRow::ExpressionFlagType:
		if (row.GetFlag() == "StencilFrontRef")				this->drawIntExpressions[RenderStateRow::StencilFrontRef] = row.GetExpression();
		else if (row.GetFlag() == "StencilBackRef")			this->drawIntExpressions[RenderStateRow::StencilBackRef] = row.GetExpression();
		else if (row.GetFlag() == "StencilReadMask")		this->drawUintExpressions[RenderStateRow::StencilReadMask] = row.GetExpression();
		else if (row.GetFlag() == "StencilWriteMask")		this->drawUintExpressions[RenderStateRow::StencilWriteMask] = row.GetExpression();
		else if (row.GetFlag() == "DepthEnabled")			this->drawBoolExpressions[RenderStateRow::DepthEnabled] = row.GetExpression();
		else if (row.GetFlag() == "DepthWrite")				this->drawBoolExpressions[RenderStateRow::DepthWrite] = row.GetExpression();
		else if (row.GetFlag() == "DepthClamp")				this->drawBoolExpressions[RenderStateRow::DepthClamp] = row.GetExpression();
		else if (row.GetFlag() == "SeparateBlend")			this->drawBoolExpressions[RenderStateRow::SeparateBlend] = row.GetExpression();
		else if (row.GetFlag() == "ScissorEnabled")			this->drawBoolExpressions[RenderStateRow::ScissorEnabled] = row.GetExpression();
		else if (row.GetFlag() == "StencilEnabled")			this->drawBoolExpressions[RenderStateRow::StencilEnabled] = row.GetExpression();
		else if (row.GetFlag() == "AlphaToCoverageEnabled") this->drawBoolExpressions[RenderStateRow::AlphaToCoverageEnabled] = row.GetExpression();
		else if (row.GetFlag() == "MultisampleEnabled")		this->drawBoolExpressions[RenderStateRow::MultisampleEnabled] = row.GetExpression();
		else if (row.GetFlag() == "PolygonOffsetEnabled")	this->drawBoolExpressions[RenderStateRow::PolygonOffsetEnabled] = row.GetExpression();
		else if (row.GetFlag() == "PolygonOffsetFactor")	this->drawFloatExpressions[RenderStateRow::PolygonOffsetFactor] = row.GetExpression();
		else if (row.GetFlag() == "PolygonOffsetUnits")		this->drawFloatExpressions[RenderStateRow::PolygonOffsetUnits] = row.GetExpression();
		else this->invalidExpressionFlags.push_back(row.GetFlag());
		break;
	}
	this->numEntries++;
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderState::ConsumeBlendRow( const BlendStateRow& row )
{
	switch (row.GetFlagType())
	{
	case BlendStateRow::StringFlagType:
		if (row.GetFlag() == "SrcBlend")
		{
			const std::string& value = row.GetString();
			unsigned flagVal;
			if (value == "Zero")						flagVal = BlendStateRow::Zero;
			else if (value == "One")					flagVal = BlendStateRow::One;
			else if (value == "SrcColor")				flagVal = BlendStateRow::SrcColor;
			else if (value == "OneMinusSrcColor")		flagVal = BlendStateRow::OneMinusSrcColor;
			else if (value == "DstColor")				flagVal = BlendStateRow::DstColor;
			else if (value == "OneMinusDstColor")		flagVal = BlendStateRow::OneMinusDstColor;
			else if (value == "SrcAlpha")				flagVal = BlendStateRow::SrcAlpha;
			else if (value == "OneMinusSrcAlpha")		flagVal = BlendStateRow::OneMinusSrcAlpha;
			else if (value == "DstAlpha")				flagVal = BlendStateRow::DstAlpha;
			else if (value == "OneMinusDstAlpha")		flagVal = BlendStateRow::OneMinusDstAlpha;
			else if (value == "SrcAlphaSaturate")		flagVal = BlendStateRow::SrcAlphaSaturate;
			else if (value == "ConstantColor")			flagVal = BlendStateRow::ConstantColor;
			else if (value == "OneMinusConstantColor")	flagVal = BlendStateRow::OneMinusConstantColor;
			else if (value == "ConstantAlpha")			flagVal = BlendStateRow::ConstantAlpha;
			else if (value == "OneMinusConstantAlpha")	flagVal = BlendStateRow::OneMinusConstantAlpha;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->blendEnumIndexExpressions.push_back(row.GetRenderTarget());
			this->blendEnumPairs.push_back(std::pair<int, BlendStateRow::EnumFlag>(flagVal, BlendStateRow::SrcBlend));
		}
		else if (row.GetFlag() == "DstBlend")
		{
			const std::string& value = row.GetString();
			unsigned flagVal;
			if (value == "Zero")						flagVal = BlendStateRow::Zero;
			else if (value == "One")					flagVal = BlendStateRow::One;
			else if (value == "SrcColor")				flagVal = BlendStateRow::SrcColor;
			else if (value == "OneMinusSrcColor")		flagVal = BlendStateRow::OneMinusSrcColor;
			else if (value == "DstColor")				flagVal = BlendStateRow::DstColor;
			else if (value == "OneMinusDstColor")		flagVal = BlendStateRow::OneMinusDstColor;
			else if (value == "SrcAlpha")				flagVal = BlendStateRow::SrcAlpha;
			else if (value == "OneMinusSrcAlpha")		flagVal = BlendStateRow::OneMinusSrcAlpha;
			else if (value == "DstAlpha")				flagVal = BlendStateRow::DstAlpha;
			else if (value == "OneMinusDstAlpha")		flagVal = BlendStateRow::OneMinusDstAlpha;
			else if (value == "SrcAlphaSaturate")		flagVal = BlendStateRow::SrcAlphaSaturate;
			else if (value == "ConstantColor")			flagVal = BlendStateRow::ConstantColor;
			else if (value == "OneMinusConstantColor")	flagVal = BlendStateRow::OneMinusConstantColor;
			else if (value == "ConstantAlpha")			flagVal = BlendStateRow::ConstantAlpha;
			else if (value == "OneMinusConstantAlpha")	flagVal = BlendStateRow::OneMinusConstantAlpha;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->blendEnumIndexExpressions.push_back(row.GetRenderTarget());
			this->blendEnumPairs.push_back(std::pair<int, BlendStateRow::EnumFlag>(flagVal, BlendStateRow::DstBlend));
		}
		else if (row.GetFlag() == "SrcBlendAlpha")
		{
			const std::string& value = row.GetString();
			unsigned flagVal;
			if (value == "Zero")						flagVal = BlendStateRow::Zero;
			else if (value == "One")					flagVal = BlendStateRow::One;
			else if (value == "SrcColor")				flagVal = BlendStateRow::SrcColor;
			else if (value == "OneMinus")				flagVal = BlendStateRow::OneMinusSrcColor;
			else if (value == "DstColor")				flagVal = BlendStateRow::DstColor;
			else if (value == "OneMinusDstColor")		flagVal = BlendStateRow::OneMinusDstColor;
			else if (value == "SrcAlpha")				flagVal = BlendStateRow::SrcAlpha;
			else if (value == "OneMinusSrcAlpha")		flagVal = BlendStateRow::OneMinusSrcAlpha;
			else if (value == "DstAlpha")				flagVal = BlendStateRow::DstAlpha;
			else if (value == "OneMinusDstAlpha")		flagVal = BlendStateRow::OneMinusDstAlpha;
			else if (value == "SrcAlphaSaturate")		flagVal = BlendStateRow::SrcAlphaSaturate;
			else if (value == "ConstantColor")			flagVal = BlendStateRow::ConstantColor;
			else if (value == "OneMinusConstantColor")	flagVal = BlendStateRow::OneMinusConstantColor;
			else if (value == "ConstantAlpha")			flagVal = BlendStateRow::ConstantAlpha;
			else if (value == "OneMinusConstantAlpha")	flagVal = BlendStateRow::OneMinusConstantAlpha;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->blendEnumIndexExpressions.push_back(row.GetRenderTarget());
			this->blendEnumPairs.push_back(std::pair<int, BlendStateRow::EnumFlag>(flagVal, BlendStateRow::SrcBlendAlpha));
		}
		else if (row.GetFlag() == "DstBlendAlpha")
		{
			const std::string& value = row.GetString();
			unsigned flagVal;
			if (value == "Zero")						flagVal = BlendStateRow::Zero;
			else if (value == "One")					flagVal = BlendStateRow::One;
			else if (value == "SrcColor")				flagVal = BlendStateRow::SrcColor;
			else if (value == "OneMinus")				flagVal = BlendStateRow::OneMinusSrcColor;
			else if (value == "DstColor")				flagVal = BlendStateRow::DstColor;
			else if (value == "OneMinusDstColor")		flagVal = BlendStateRow::OneMinusDstColor;
			else if (value == "SrcAlpha")				flagVal = BlendStateRow::SrcAlpha;
			else if (value == "OneMinusSrcAlpha")		flagVal = BlendStateRow::OneMinusSrcAlpha;
			else if (value == "DstAlpha")				flagVal = BlendStateRow::DstAlpha;
			else if (value == "OneMinusDstAlpha")		flagVal = BlendStateRow::OneMinusDstAlpha;
			else if (value == "SrcAlphaSaturate")		flagVal = BlendStateRow::SrcAlphaSaturate;
			else if (value == "ConstantColor")			flagVal = BlendStateRow::ConstantColor;
			else if (value == "OneMinusConstantColor")	flagVal = BlendStateRow::OneMinusConstantColor;
			else if (value == "ConstantAlpha")			flagVal = BlendStateRow::ConstantAlpha;
			else if (value == "OneMinusConstantAlpha")	flagVal = BlendStateRow::OneMinusConstantAlpha;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->blendEnumIndexExpressions.push_back(row.GetRenderTarget());
			this->blendEnumPairs.push_back(std::pair<int, BlendStateRow::EnumFlag>(flagVal, BlendStateRow::DstBlendAlpha));
		}
		else if (row.GetFlag() == "BlendOp")
		{
			const std::string& value = row.GetString();
			unsigned flagVal;
			if (value == "Add")						flagVal = BlendStateRow::Add;
			else if (value == "Sub")				flagVal = BlendStateRow::Sub;
			else if (value == "InvSub")				flagVal = BlendStateRow::InvSub;
			else if (value == "Min")				flagVal = BlendStateRow::Min;
			else if (value == "Max")				flagVal = BlendStateRow::Max;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->blendEnumIndexExpressions.push_back(row.GetRenderTarget());
			this->blendEnumPairs.push_back(std::pair<int, BlendStateRow::EnumFlag>(flagVal, BlendStateRow::BlendOp));
		}
		else if (row.GetFlag() == "BlendOpAlpha")
		{
			const std::string& value = row.GetString();
			unsigned flagVal;
			if (value == "Add")						flagVal = BlendStateRow::Add;
			else if (value == "Sub")				flagVal = BlendStateRow::Sub;
			else if (value == "InvSub")				flagVal = BlendStateRow::InvSub;
			else if (value == "Min")				flagVal = BlendStateRow::Min;
			else if (value == "Max")				flagVal = BlendStateRow::Max;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->blendEnumIndexExpressions.push_back(row.GetRenderTarget());
			this->blendEnumPairs.push_back(std::pair<int, BlendStateRow::EnumFlag>(flagVal, BlendStateRow::BlendOpAlpha));
		}
		else 
		{
			this->invalidStringFlags.push_back(row.GetFlag());
		}
		break;
	case BlendStateRow::ExpressionFlagType:
		if (row.GetFlag() == "BlendEnabled")
		{
			this->blendBoolIndexExpressions.push_back(row.GetRenderTarget());
			this->blendBoolPairs.push_back(std::pair<Expression*, BlendStateRow::BoolFlag>(row.GetExpression(), BlendStateRow::BlendEnabled));
		}
		else 
		{
			this->invalidExpressionFlags.push_back(row.GetFlag());
		}
		break;
	}
	this->numEntries++;
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderState::TypeCheck( TypeChecker& typechecker )
{
	// add render state, if failed we must have a redefinition
	if (!typechecker.AddSymbol(this)) return;

	// report all if any unwanted options
	unsigned i;
	for (i = 0; i < this->invalidExpressionFlags.size(); i++)
	{
		std::string msg = Format("Render state field '%s' is invalid, %s\n", this->invalidExpressionFlags[i].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(msg);
	}

	for (i = 0; i < this->invalidStringFlags.size(); i++)
	{
		std::string msg = Format("Render state field '%s' is invalid, %s\n", this->invalidStringFlags[i].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(msg);
	}

	for (i = 0; i < this->invalidValues.size(); i++)
	{
		std::string msg = AnyFX::Format("Invalid value '%s' for flag '%s' at entry %d, %s\n", this->invalidValues[i].value.c_str(), this->invalidValues[i].flag.c_str(), this->invalidValues[i].entry, this->ErrorSuffix().c_str());
		typechecker.Error(msg);
	}

	for (i = 0; i < this->blendEnumIndexExpressions.size(); i++)
	{
		int index = this->blendEnumIndexExpressions[i]->EvalInt(typechecker);
		int flag = this->blendEnumPairs[i].second;
		int value = this->blendEnumPairs[i].first;

		if (index > MaxNumRenderTargets)
		{
			std::string msg = AnyFX::Format("Blend index '%d' must be lower than %d (maximum amount of render targets) in '%s', %s\n", index, MaxNumRenderTargets, this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(msg);
		}
		else
		{
			this->blendEnumFlags[index][flag] = value;
		}		
		delete this->blendEnumIndexExpressions[i];
	}

	for (i = 0; i < this->blendBoolIndexExpressions.size(); i++)
	{
		int index = this->blendBoolIndexExpressions[i]->EvalInt(typechecker);
		int flag = this->blendBoolPairs[i].second;
		bool value = this->blendBoolPairs[i].first->EvalBool(typechecker);

		if (index > MaxNumRenderTargets)
		{
			std::string msg = AnyFX::Format("Blend index '%d' must be lower than %d (maximum amount of render targets) in '%s', %s\n", index, MaxNumRenderTargets, this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(msg);
		}
		else
		{
			this->blendBoolFlags[index][flag] = value;
		}
		delete this->blendBoolIndexExpressions[i];
		delete this->blendBoolPairs[i].first;
	}


	for (i = 0; i < RenderStateRow::NumIntFlags; i++)
	{
		if (this->drawIntExpressions[i])
		{
			this->drawIntFlags[i] = this->drawIntExpressions[i]->EvalInt(typechecker);
			delete this->drawIntExpressions[i];
			this->drawIntExpressions[i] = NULL;
		}
	}

	for (i = 0; i < RenderStateRow::NumUintFlags; i++)
	{
		if (this->drawUintExpressions[i])
		{
			this->drawUintFlags[i] = this->drawUintExpressions[i]->EvalUInt(typechecker);
			delete this->drawUintExpressions[i];
			this->drawUintExpressions[i] = NULL;
		}
	}

	for (i = 0; i < RenderStateRow::NumBoolFlags; i++)
	{
		if (this->drawBoolExpressions[i])
		{
			this->drawBoolFlags[i] = this->drawBoolExpressions[i]->EvalBool(typechecker);
			delete this->drawBoolExpressions[i];
			this->drawBoolExpressions[i] = NULL;
		}
	}

	for (i = 0; i < RenderStateRow::NumFloatFlags; i++)
	{
		if (this->drawFloatExpressions[i])
		{
			this->drawFloatFlags[i] = this->drawFloatExpressions[i]->EvalFloat(typechecker);
			delete this->drawFloatExpressions[i];
			this->drawFloatExpressions[i] = NULL;
		}
	}

	// type check annotation
	if (this->hasAnnotation)
	{
		this->annotation.TypeCheck(typechecker);
	}

	// fail type checking if the name of this render state is placeholder
	if (this->name == "placeholder")
	{
		std::string message = Format("Type error: Render state with name 'placeholder' is not allowed, %s", this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderState::Compile( BinWriter& writer )
{
	// write name
	writer.WriteString(this->name);

	// write if annotation is used
	writer.WriteBool(this->hasAnnotation);

	// compile annotation if
	if (this->hasAnnotation)
	{
		this->annotation.Compile(writer);
	}	

	// write blend flags
	unsigned i;
	for (i = 0; i < MaxNumRenderTargets; i++)
	{
		// write bool flags
		unsigned j;
		for (j = 0; j < BlendStateRow::NumBoolFlags; j++)
		{
			// write bool flags
			writer.WriteBool(blendBoolFlags[i][j]);
		}
		
		for (j = 0; j < BlendStateRow::NumEnumFlags; j++)
		{
			// writer blend flags
			writer.WriteInt(blendEnumFlags[i][j]);
		}
	}


	// write draw bool flags
	for (i = 0; i < RenderStateRow::NumBoolFlags; i++)
	{
		writer.WriteBool(drawBoolFlags[i]);
	}

	// write enumerated draw flags
	for (i = 0; i < RenderStateRow::NumEnumFlags; i++)
	{
		writer.WriteUInt(drawEnumFlags[i]);
	}

	// write draw int flags
	for (i = 0; i < RenderStateRow::NumIntFlags; i++)
	{
		writer.WriteInt(drawIntFlags[i]);
	}

	// write draw uint flags
	for (i = 0; i < RenderStateRow::NumUintFlags; i++)
	{
		writer.WriteUInt(drawUintFlags[i]);
	}

	// write draw float flags
	for (i = 0; i < RenderStateRow::NumFloatFlags; i++)
	{
		writer.WriteFloat(drawFloatFlags[i]);
	}
}

} // namespace AnyFX