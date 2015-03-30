#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::BinaryExpression
    
    A binary expression is a two-part expression, using a left-hand side and right-hand side argument together with an operator.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "expression.h"
namespace AnyFX
{
class BinaryExpression : public Expression
{
public:
	/// constructor
	BinaryExpression(const std::string& op, Expression* left, Expression* right);
	/// destructor
	virtual ~BinaryExpression();

	/// evaulate type of expression
	DataType EvalType(TypeChecker& typechecker);

	/// evaluates expression as an integer
	int EvalInt(TypeChecker& typechecker);
	/// evaluates expression as an integer
	unsigned EvalUInt(TypeChecker& typechecker);
	/// evaulates expression as a float
	float EvalFloat(TypeChecker& typechecker);
	/// evaluates expression as bool
	bool EvalBool(TypeChecker& typechecker);
private:

	/// helper functions for bool evaluations
	bool EvalBool(int lhs, int rhs, TypeChecker& typechecker);
	bool EvalBool(float lhs, int rhs, TypeChecker& typechecker);
	bool EvalBool(int lhs, float rhs, TypeChecker& typechecker);
	bool EvalBool(float lhs, float rhs, TypeChecker& typechecker);
	bool EvalBool(bool lhs, bool rhs, TypeChecker& typechecker);

	std::string op;
	Expression* left;
	Expression* right;
}; 


} // namespace AnyFX
//------------------------------------------------------------------------------