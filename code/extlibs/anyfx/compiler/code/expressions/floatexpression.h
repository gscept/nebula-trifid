#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::FloatExpression
    
    A simple expression which holds a floating point value.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "expression.h"
namespace AnyFX
{
class FloatExpression : public Expression
{
public:
	/// constructor
	FloatExpression(float value);
	/// destructor
	virtual ~FloatExpression();

	/// evaulate type of expression
	DataType EvalType(TypeChecker& typechecker);

	/// evaluates expression as an integer
	int EvalInt(TypeChecker& typechecker);
	/// evaluates expression as an integer
	unsigned EvalUInt(TypeChecker& typechecker);
	/// evaulates expression as a float
	float EvalFloat(TypeChecker& typechecker);
	/// evaluates expression as a bool
	bool EvalBool(TypeChecker& typechecker);
private:
	float value;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------