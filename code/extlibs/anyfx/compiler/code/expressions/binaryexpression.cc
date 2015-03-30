//------------------------------------------------------------------------------
//  binaryexpression.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "binaryexpression.h"
#include "datatype.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
BinaryExpression::BinaryExpression(const std::string& op, Expression* left, Expression* right) :
	op(op),
	left(left),
	right(right)
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/
BinaryExpression::~BinaryExpression()
{
	delete this->left;
	delete this->right;
}

//------------------------------------------------------------------------------
/**
*/
AnyFX::DataType 
BinaryExpression::EvalType( TypeChecker& typechecker )
{
	DataType lhs = this->left->EvalType(typechecker);
	DataType rhs = this->right->EvalType(typechecker);

	if (lhs == rhs)
	{	
		return lhs;
	}
	else if (lhs.GetType() == DataType::Integer)
	{
		if (rhs.GetType() == DataType::Float)
		{
			return rhs;
		}
		else if (rhs.GetType() == DataType::Double)
		{
			return rhs;
		}
		else goto error;
	}
	else if (lhs.GetType() == DataType::Float)
	{
		if (rhs.GetType() == DataType::Integer)
		{
			return lhs;
		}
		else if (rhs.GetType() == DataType::Double)
		{
			return rhs;
		}
		else goto error;
	}
	else if (lhs.GetType() == DataType::Double)
	{
		if (rhs.GetType() == DataType::Integer)
		{
			return lhs;
		}
		else if (rhs.GetType() == DataType::Float)
		{
			return lhs;
		}
		else goto error;
	}
	else goto error;

error:
	std::string msg = Format("Type '%s' cannot use binary operator '%s' with type '%s', %s\n", DataType::ToString(lhs).c_str(), this->op.c_str(), DataType::ToString(rhs).c_str(), this->ErrorSuffix().c_str());
	typechecker.Error(msg);
	return lhs;
}

//------------------------------------------------------------------------------
/**
*/
int 
BinaryExpression::EvalInt( TypeChecker& typechecker )
{
	int lVal = this->left->EvalInt(typechecker);
	int rVal = this->right->EvalInt(typechecker);

	if (this->op == "+")
	{
		return lVal + rVal;
	}
	else if (this->op == "-")
	{
		return lVal - rVal;
	}
	else if (this->op == "*")
	{
		return lVal * rVal;
	}
	else if (this->op == "/")
	{
		return lVal / rVal;
	}
	else if (this->op == "<")
	{
		return (int)(lVal < rVal);
	}
	else if (this->op == ">")
	{
		return (int)(lVal > rVal);
	}
	else if (this->op == "<=")
	{
		return (int)(lVal <= rVal);
	}
	else if (this->op == ">=")
	{
		return (int)(lVal >= rVal);
	}
	else
	{
		std::string msg = Format("Invalid operator '%s' with int, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(msg);
		return -1;
	}
}

//------------------------------------------------------------------------------
/**
*/
unsigned
BinaryExpression::EvalUInt(TypeChecker& typechecker)
{
	unsigned lVal = this->left->EvalUInt(typechecker);
	unsigned rVal = this->right->EvalUInt(typechecker);

	if (this->op == "+")
	{
		return lVal + rVal;
	}
	else if (this->op == "-")
	{
		return lVal - rVal;
	}
	else if (this->op == "*")
	{
		return lVal * rVal;
	}
	else if (this->op == "/")
	{
		return lVal / rVal;
	}
	else if (this->op == "<")
	{
		return (unsigned)(lVal < rVal);
	}
	else if (this->op == ">")
	{
		return (unsigned)(lVal > rVal);
	}
	else if (this->op == "<=")
	{
		return (unsigned)(lVal <= rVal);
	}
	else if (this->op == ">=")
	{
		return (unsigned)(lVal >= rVal);
	}
	else
	{
		std::string msg = Format("Invalid operator '%s' with int, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(msg);
		return -1;
	}
}

//------------------------------------------------------------------------------
/**
*/
float 
BinaryExpression::EvalFloat( TypeChecker& typechecker )
{
	float lVal = this->left->EvalFloat(typechecker);
	float rVal = this->right->EvalFloat(typechecker);

	if (this->op == "+")
	{
		return lVal + rVal;
	}
	else if (this->op == "-")
	{
		return lVal - rVal;
	}
	else if (this->op == "*")
	{
		return lVal * rVal;
	}
	else if (this->op == "/")
	{
		return lVal / rVal;
	}
	else
	{
		std::string msg = Format("Invalid operator '%s' with float, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(msg);
		return -1;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinaryExpression::EvalBool( TypeChecker& typechecker )
{
	DataType lhs = this->left->EvalType(typechecker);
	DataType rhs = this->right->EvalType(typechecker);

	if (lhs.GetType() == DataType::Integer)
	{
		int lVal = this->left->EvalInt(typechecker);
		if (rhs.GetType() == DataType::Float)
		{
			float rVal = this->right->EvalFloat(typechecker);
			return EvalBool(lVal, rVal, typechecker);
		}
		else if (rhs.GetType() == DataType::Integer)
		{
			int rVal = this->right->EvalInt(typechecker);
			return EvalBool(lVal, rVal, typechecker);
		}
		else goto error;
	}
	else if (lhs.GetType() == DataType::Float)
	{
		float lVal = this->left->EvalFloat(typechecker);
		if (rhs.GetType() == DataType::Integer)
		{
			float rVal = this->right->EvalInt(typechecker);
			return EvalBool(lVal, rVal, typechecker);
		}
		else if (rhs.GetType() == DataType::Float)
		{
			float rVal = this->right->EvalInt(typechecker);
			return EvalBool(lVal, rVal, typechecker);
		}
		else goto error;
	}
	else if (lhs.GetType() == DataType::Bool)
	{
		bool lVal = this->left->EvalBool(typechecker);
		if (rhs.GetType() == DataType::Bool)
		{
			bool rVal = this->right->EvalBool(typechecker);
			return EvalBool(lVal, rVal, typechecker);
		}
		else goto error;
	}

	else goto error;

error:
	std::string msg = Format("Type '%s' cannot use binary operator '%s' with type '%s', %s\n", DataType::ToString(lhs).c_str(), this->op.c_str(), DataType::ToString(rhs).c_str(), this->ErrorSuffix().c_str());
	typechecker.Error(msg);
	return false;

}

//------------------------------------------------------------------------------
/**
*/
bool 
BinaryExpression::EvalBool( int lhs, int rhs, TypeChecker& typechecker )
{
	if (this->op == "<")
	{
		return lhs < rhs;
	}
	else if (this->op == ">")
	{
		return lhs > rhs;
	}
	else if (this->op == "<=")
	{
		return lhs <= rhs;
	}
	else if (this->op == ">=")
	{
		return lhs >= rhs;
	}
	else if (this->op == "==")
	{
		return lhs == rhs;
	}
	else if (this->op == "!=")
	{
		return lhs != rhs;
	}
	else
	{
		std::string err = Format("Invalid operator '%s' between int and int, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(err);
		return false;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinaryExpression::EvalBool( float lhs, int rhs, TypeChecker& typechecker )
{
	if (this->op == "<")
	{
		return lhs < rhs;
	}
	else if (this->op == ">")
	{
		return lhs > rhs;
	}
	else if (this->op == "<=")
	{
		return lhs <= rhs;
	}
	else if (this->op == ">=")
	{
		return lhs >= rhs;
	}
	else if (this->op == "==")
	{
		return lhs == rhs;
	}
	else if (this->op == "!=")
	{
		return lhs != rhs;
	}
	else
	{
		std::string err = Format("Invalid operator '%s' between float and int, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(err);
		return false;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinaryExpression::EvalBool( int lhs, float rhs, TypeChecker& typechecker )
{
	if (this->op == "<")
	{
		return lhs < rhs;
	}
	else if (this->op == ">")
	{
		return lhs > rhs;
	}
	else if (this->op == "<=")
	{
		return lhs <= rhs;
	}
	else if (this->op == ">=")
	{
		return lhs >= rhs;
	}
	else if (this->op == "==")
	{
		return lhs == rhs;
	}
	else if (this->op == "!=")
	{
		return lhs != rhs;
	}
	else if (this->op == "||")
	{
		return lhs || rhs;
	}
	else if (this->op == "&&")
	{
		return lhs && rhs;
	}
	else
	{
		std::string err = Format("Invalid operator '%s' between int and float, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(err);
		return false;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinaryExpression::EvalBool( float lhs, float rhs, TypeChecker& typechecker )
{
	if (this->op == "<")
	{
		return lhs < rhs;
	}
	else if (this->op == ">")
	{
		return lhs > rhs;
	}
	else if (this->op == "<=")
	{
		return lhs <= rhs;
	}
	else if (this->op == ">=")
	{
		return lhs >= rhs;
	}
	else if (this->op == "==")
	{
		return lhs == rhs;
	}
	else if (this->op == "!=")
	{
		return lhs != rhs;
	}
	else
	{
		std::string err = Format("Invalid operator '%s' between float and float, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(err);
		return false;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinaryExpression::EvalBool( bool lhs, bool rhs, TypeChecker& typechecker )
{
	if (this->op == "==")
	{
		return lhs == rhs;
	}
	else if (this->op == "!=")
	{
		return lhs != rhs;
	}
	else
	{
		std::string err = Format("Invalid operator '%s' between bool and bool, %s\n", this->op.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(err);
		return false;
	}
}
} // namespace AnyFX