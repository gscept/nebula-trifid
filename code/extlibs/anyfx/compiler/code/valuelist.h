#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::ValueList
    
    A value list is a value initialization list. It contains the values, the amount of values, and also the inferred type.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "compileable.h"
#include "datatype.h"
#include "expressions/expression.h"
namespace AnyFX
{
class ValueList : public Compileable
{
public:

	/// constructor
	ValueList();
	/// destructor
	virtual ~ValueList();

	/// set value as string, decomposes into inferred type
	void AddValue(Expression* expr);
	/// get number of values
	const unsigned GetNumValues() const;
	/// get value
	Expression* GetValue(unsigned index) const;

	/// return value list as string
	const std::string& GetString() const;
	
	/// converts valuelist to string
	void ConvertToString(const DataType& type, TypeChecker& typechecker);

private:
	std::string formattedString;
	std::vector<Expression*> values;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
ValueList::GetNumValues() const
{
	return this->values.size();
}

//------------------------------------------------------------------------------
/**
*/
inline Expression*
ValueList::GetValue( unsigned index ) const
{
	return this->values[index];
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
ValueList::GetString() const
{
	return this->formattedString;
}


} // namespace AnyFX
//------------------------------------------------------------------------------