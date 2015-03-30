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
	expression(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FunctionAttribute::~FunctionAttribute()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
FunctionAttribute::SetBool( const std::string& flag, bool value )
{
	this->flag = flag;
	this->flagType = BoolFlagType;
	this->boolValue = value;
}

//------------------------------------------------------------------------------
/**
*/
void 
FunctionAttribute::SetString( const std::string& flag, const std::string& value )
{
	this->flag = flag;
	this->flagType = StringFlagType;
	if (this->stringValue)
	{
		delete this->stringValue;
	}
	this->stringValue = new std::string(value);
}

//------------------------------------------------------------------------------
/**
*/
void 
FunctionAttribute::SetExpression( const std::string& flag, Expression* expr )
{
	this->flag = flag;
	this->flagType = ExpressionFlagType;
	this->expression = expr;
}

} // namespace AnyFX