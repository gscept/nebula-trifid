//------------------------------------------------------------------------------
//  expression.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "expression.h"
#include "util.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Expression::Expression()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Expression::~Expression()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AnyFX::DataType
Expression::EvalType(TypeChecker& typechecker)
{
	Error("Expression::EvalType() called!\n");
	DataType type;
	type.SetType(DataType::Undefined);
	return type;
}

//------------------------------------------------------------------------------
/**
*/
int
Expression::EvalInt(TypeChecker& typechecker)
{
	Error("Expression::EvalInt() called!\n");
	return -1;
}

//------------------------------------------------------------------------------
/**
*/
unsigned
Expression::EvalUInt(TypeChecker& typechecker)
{
	Error("Expression::EvalUInt() called!\n");
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
float
Expression::EvalFloat(TypeChecker& typechecker)
{
	Error("Expression::EvalFloat() called!\n");
	return 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
bool
Expression::EvalBool(TypeChecker& typechecker)
{
	Error("Expression::EvalBool() called!\n");
	return false;
}

} // namespace AnyFX