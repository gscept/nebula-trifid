//------------------------------------------------------------------------------
//  functionattribute.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "functionattribute.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
FunctionAttribute::FunctionAttribute() :
	flagType(InvalidFlagType),
	expr(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FunctionAttribute::~FunctionAttribute()
{
	// everything in here is deleted elsewhere
}

//------------------------------------------------------------------------------
/**
*/
void
FunctionAttribute::SetBool(const std::string& flag, bool value)
{
	this->flag = flag;
	this->flagType = BoolFlagType;
	this->boolValue = value;
}

//------------------------------------------------------------------------------
/**
*/
void
FunctionAttribute::SetString(const std::string& flag, const std::string& value)
{
	this->flag = flag;
	this->flagType = StringFlagType;
	this->stringValue = value;
}

//------------------------------------------------------------------------------
/**
*/
void
FunctionAttribute::SetExpression(const std::string& flag, Expression* expr)
{
	this->flag = flag;
	this->flagType = ExpressionFlagType;
	this->expr = expr;
}

} // namespace AnyFX