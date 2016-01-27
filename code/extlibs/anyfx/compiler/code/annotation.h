#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Annotation
    
    An annotation represents three lists which denotes a set of annotations, type, name and value.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <vector>
#include <string>
#include "datatype.h"
#include "compileable.h"
#include "expressions/expression.h"
namespace AnyFX
{
class Annotation : public Compileable
{
public:
	/// constructor
	Annotation();
	/// destructor
	virtual ~Annotation();

	/// add type
	void AddType(const DataType& type);
	/// add name
	void AddName(const std::string& name);

	/// add expression
	void AddExpression(Expression* expr);

	/// add string value
	void AddString(const std::string& value);

	/// type check annotation
	void TypeCheck(TypeChecker& typechecker);
	/// compile annotation
	void Compile(BinWriter& writer);

private:

	struct variant
	{
		int intValue;
		bool boolValue;
		double doubleValue;
		float floatValue;

		Expression* expr;
		std::string* stringValue;

		variant() :
			expr(NULL),
			stringValue(NULL)
		{
			// empty
		}
	};

	std::vector<DataType> types;
	std::vector<std::string> names;
	std::vector<variant> values;
}; 
} // namespace AnyFX
//------------------------------------------------------------------------------
