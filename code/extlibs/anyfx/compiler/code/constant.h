#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Constant
    
    A constant is a shader-declared constant which cannot be altered from CPU.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "symbol.h"
#include "datatype.h"
#include "valuelist.h"
#include "expressions/expression.h"
namespace AnyFX
{
class Constant : public Symbol
{
public:

	enum ArrayType
	{
		SimpleArray,		// evaluate array size by using value table single entry number of values
		TypedArray,			// evaluate array size by using value table size
		ExplicitArray		// evaluate array size by expression
	};

	/// constructor
	Constant();
	/// destructor
	virtual ~Constant();

	/// sets array size expression
	void SetSizeExpression(Expression* expr);
	/// gets array size expression
	Expression* GetSizeExpression() const;

	/// sets array type
	void SetArrayType(const ArrayType& type);
	/// gets array type
	const ArrayType& GetArrayType() const;

	/// gets array size
	unsigned GetArraySize() const;
	/// sets constant type
	void SetDataType(const DataType& type);
	/// gets constant type
	const DataType& GetDataType() const;

	/// adds a value-type pair
	void AddValue(const DataType& type, const ValueList& value);
	/// adds a value-type for basic types
	void AddValue(const ValueList& value);

	/// gets number of values in this constant
	unsigned GetNumValues() const;
	/// gets value at index
	const ValueList& GetValue(unsigned i) const;

	/// type checks constant
	void TypeCheck(TypeChecker& typechecker);
	/// formats constant
	std::string Format(const Header& header) const;

private:
	DataType type;
	std::vector<std::pair<DataType, ValueList> > valueTable;

	ArrayType arrayType;
	Expression* sizeExpression;
	unsigned arraySize;
	bool isArray;
	
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
Constant::SetSizeExpression(Expression* expr)
{
	this->sizeExpression = expr;
	this->isArray = true;
}

//------------------------------------------------------------------------------
/**
*/
inline Expression* 
Constant::GetSizeExpression() const
{
	return this->sizeExpression;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Constant::SetArrayType(const ArrayType& type)
{
	this->isArray = true;
	this->arrayType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const Constant::ArrayType& 
Constant::GetArrayType() const
{
	return this->arrayType;
}

//------------------------------------------------------------------------------
/**
*/
inline unsigned 
Constant::GetArraySize() const
{
	return this->arraySize;
}

//------------------------------------------------------------------------------
/**
*/
inline unsigned 
Constant::GetNumValues() const
{
	return this->valueTable.size();
}

//------------------------------------------------------------------------------
/**
*/
inline const ValueList&
Constant::GetValue(unsigned i) const
{
	return this->valueTable[i].second;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Constant::SetDataType(const DataType& type)
{
	this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const DataType& 
Constant::GetDataType() const
{
	return this->type;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
