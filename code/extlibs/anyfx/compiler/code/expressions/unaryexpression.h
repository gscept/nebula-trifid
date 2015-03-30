#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::UnaryExpression
    
    A unary expression is a one-sided expression.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "expression.h"
namespace AnyFX
{
class UnaryExpression : public Expression
{
public:
	/// constructor
	UnaryExpression(char op, Expression* expr);
	/// destructor
	virtual ~UnaryExpression();

	/// evaulate type of expression
	DataType EvalType(TypeChecker& typechecker);

	/// evaluates as integer
	int EvalInt(TypeChecker& typechecker);
	/// evaluates expression as an integer
	unsigned EvalUInt(TypeChecker& typechecker);
	/// evaluates as float
	float EvalFloat(TypeChecker& typechecker);
	/// evaluates bool
	bool EvalBool(TypeChecker& typechecker);

private:
	char op;
	Expression* expr;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------