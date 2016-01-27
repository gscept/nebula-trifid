//------------------------------------------------------------------------------
//  boolexpression.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "boolexpression.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
BoolExpression::BoolExpression(bool value) :
	value(value)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BoolExpression::~BoolExpression()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AnyFX::DataType 
BoolExpression::EvalType( TypeChecker& typechecker )
{
	DataType type;
	type.SetStyle(DataType::Generic);
	type.SetType(DataType::Bool);
	return type;
}

//------------------------------------------------------------------------------
/**
*/
int
BoolExpression::EvalInt(TypeChecker& typechecker)
{
	return (int)this->value;
}

//------------------------------------------------------------------------------
/**
*/
unsigned
BoolExpression::EvalUInt(TypeChecker& typechecker)
{
	return (unsigned) this->value;
}

//------------------------------------------------------------------------------
/**
*/
float
BoolExpression::EvalFloat(TypeChecker& typechecker)
{
	std::string err = Format("Bool cannot be explicitly cast to float, %s\n", this->ErrorSuffix().c_str());
	typechecker.Error(err);
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
BoolExpression::EvalBool(TypeChecker& typechecker)
{
	return this->value;
}
} // namespace AnyFX