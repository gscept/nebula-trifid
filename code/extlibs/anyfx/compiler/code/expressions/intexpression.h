#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::IntExpression
    
    An int expression is a simple expression which holds an integer.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "expression.h"
namespace AnyFX
{
class IntExpression : public Expression
{
public:
	/// constructor
	IntExpression(int value);
	/// destructor
	virtual ~IntExpression();

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
	int value;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------