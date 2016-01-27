//------------------------------------------------------------------------------
//  unaryexpression.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "unaryexpression.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
UnaryExpression::UnaryExpression(char op, Expression* expr) :
	op(op),
	expr(expr)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
UnaryExpression::~UnaryExpression()
{
	delete this->expr;
}

//------------------------------------------------------------------------------
/**
*/
AnyFX::DataType
UnaryExpression::EvalType(TypeChecker& typechecker)
{
	return this->expr->EvalType(typechecker);
}

//------------------------------------------------------------------------------
/**
*/
int
UnaryExpression::EvalInt(TypeChecker& typechecker)
{
	if (this->op == '-')
	{
		return this->expr->EvalInt(typechecker) * -1;
	}
	else
	{
		std::string msg = Format("Invalid operator '%c' for int, %s\n", this->op, this->ErrorSuffix().c_str());
		typechecker.Error(msg);
		return -1;
	}
}

//------------------------------------------------------------------------------
/**
*/
unsigned
UnaryExpression::EvalUInt(TypeChecker& typechecker)
{
	if (this->op == '-')
	{
		return this->expr->EvalUInt(typechecker) * -1;
	}
	else
	{
		std::string msg = Format("Invalid operator '%c' for unsigned int, %s\n", this->op, this->ErrorSuffix().c_str());
		typechecker.Error(msg);
		return -1;
	}
}

//------------------------------------------------------------------------------
/**
*/
float
UnaryExpression::EvalFloat(TypeChecker& typechecker)
{
	if (this->op == '-')
	{
		return this->expr->EvalFloat(typechecker) * -1;
	}
	else
	{
		std::string msg = Format("Invalid operator '%c' for float, %s\n", this->op, this->ErrorSuffix().c_str());
		typechecker.Error(msg);
		return 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
UnaryExpression::EvalBool(TypeChecker& typechecker)
{
	if (this->op == '!')
	{
		return !this->expr->EvalBool(typechecker);
	}
	else
	{
		std::string msg = Format("Invalid operator '%c' for bool, %s\n", this->op, this->ErrorSuffix().c_str());
		typechecker.Error(msg);
		return false;
	}
}
} // namespace AnyFX