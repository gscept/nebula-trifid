//------------------------------------------------------------------------------
//  floatexpression.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "floatexpression.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
FloatExpression::FloatExpression(float value) :
	value(value)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FloatExpression::~FloatExpression()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AnyFX::DataType 
FloatExpression::EvalType( TypeChecker& typechecker )
{
	DataType type;
	type.SetStyle(DataType::Generic);
	type.SetType(DataType::Float);
	return type;
}

//------------------------------------------------------------------------------
/**
	Convert this float to an integer
*/
int 
FloatExpression::EvalInt( TypeChecker& typechecker )
{
	return (int)this->value;
}

//------------------------------------------------------------------------------
/**
*/
unsigned
FloatExpression::EvalUInt(TypeChecker& typechecker)
{
	return (unsigned)this->value;
}

//------------------------------------------------------------------------------
/**
*/
float
FloatExpression::EvalFloat(TypeChecker& typechecker)
{
	return this->value;
}

//------------------------------------------------------------------------------
/**
*/
bool
FloatExpression::EvalBool(TypeChecker& typechecker)
{
	std::string err = Format("Float cannot be explicitly cast to bool, %s\n", this->ErrorSuffix().c_str());
	typechecker.Error(err);
	return false;
}
} // namespace AnyFX