//------------------------------------------------------------------------------
//  samplerrow.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "samplerrow.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
SamplerRow::SamplerRow() :
	expr(NULL)
{
	this->rgbExprs.v[0] = NULL;
	this->rgbExprs.v[1] = NULL;
	this->rgbExprs.v[2] = NULL;
	this->rgbExprs.v[3] = NULL;
}

//------------------------------------------------------------------------------
/**
*/
SamplerRow::~SamplerRow()
{
	// everything in here is deleted elsewhere
}

//------------------------------------------------------------------------------
/**
*/
void
SamplerRow::SetString(const std::string& flag, const std::string& value)
{
	this->flagType = StringFlagType;
	this->flag = flag;
	this->stringValue = value;
}

//------------------------------------------------------------------------------
/**
*/
void 
SamplerRow::SetFloat4(const std::string& flag, Expression* value0, Expression* value1, Expression* value2, Expression* value3)
{
	this->flagType = Float4FlagType;
	this->flag = flag;
	this->rgbExprs.v[0] = value0;
	this->rgbExprs.v[1] = value1;
	this->rgbExprs.v[2] = value2;
	this->rgbExprs.v[3] = value3;
}

//------------------------------------------------------------------------------
/**
*/
void
SamplerRow::SetTextures(const SamplerTextureList& list)
{
	this->flagType = TextureListFlagType;
	this->flag = "Textures";
	this->textureList = list;
}

//------------------------------------------------------------------------------
/**
*/
void
SamplerRow::SetExpression(const std::string& flag, Expression* expr)
{
	this->flagType = ExpressionFlagType;
	this->flag = flag;
	this->expr = expr;
}

//------------------------------------------------------------------------------
/**
*/
const std::string
SamplerRow::StringFlagToString(const EnumFlag& flag)
{
	switch (flag)
	{
	case Filter:
		return "Filter";
	case AddressU:
		return "AddressU";
	case AddressV:
		return "AddressV";
	case AddressW:
		return "AddressW";
	case ComparisonFunc:
		return "ComparisonFunc";
	default:
		return "";
	}
}



} // namespace AnyFX
