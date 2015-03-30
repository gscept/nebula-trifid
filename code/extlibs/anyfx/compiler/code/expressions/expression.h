#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Expression
    
    Base class for expressions. 
	In AnyFX, expressions are purely aritmetic, meaning they can only return a value.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "datatype.h"
#include "typechecker.h"

namespace AnyFX
{
class Expression : public Compileable
{
public:
	/// constructor
	Expression();
	/// destructor
	virtual ~Expression();

	/// evaulate type of expression
	virtual DataType EvalType(TypeChecker& typechecker);

	/// evaluates expression as an integer
	virtual int EvalInt(TypeChecker& typechecker);
	/// evaluates expression as an integer
	virtual unsigned EvalUInt(TypeChecker& typechecker);
	/// evaulates expression as a float
	virtual float EvalFloat(TypeChecker& typechecker);
	/// evaluates expression as a boolean
	virtual bool EvalBool(TypeChecker& typechecker);
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------