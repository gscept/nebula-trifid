#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::BoolExpression
    
    A boolean expression holds a single boolean argument.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "expression.h"
namespace AnyFX
{
class BoolExpression : public Expression
{
public:
	/// constructor
	BoolExpression(bool value);
	/// destructor
	virtual ~BoolExpression();

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
	bool value;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------