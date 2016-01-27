//------------------------------------------------------------------------------
//  intexpression.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "intexpression.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
IntExpression::IntExpression(int value) :
	value(value)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
IntExpression::~IntExpression()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AnyFX::DataType
IntExpression::EvalType(TypeChecker& typechecker)
{
	DataType type;
	type.SetStyle(DataType::Generic);
	type.SetType(DataType::Integer);
	return type;
}

//------------------------------------------------------------------------------
/**
*/
int
IntExpression::EvalInt(TypeChecker& typechecker)
{
	return this->value;
}

//------------------------------------------------------------------------------
/**
*/
unsigned
IntExpression::EvalUInt(TypeChecker& typechecker)
{
	return (unsigned)this->value;
}

//------------------------------------------------------------------------------
/**
	Convert this integer to a float
*/
float
IntExpression::EvalFloat(TypeChecker& typechecker)
{
	return (float)this->value;
}

//------------------------------------------------------------------------------
/**
*/
bool
IntExpression::EvalBool(TypeChecker& typechecker)
{
	return this->value != 0;
}
} // namespace AnyFX