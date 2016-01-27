//------------------------------------------------------------------------------
//  valuelist.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "valuelist.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
ValueList::ValueList()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ValueList::~ValueList()
{
	this->values.clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ValueList::AddValue(Expression* expr)
{
	this->values.push_back(expr);
}

//------------------------------------------------------------------------------
/**
*/
void
ValueList::ConvertToString(const DataType& type, TypeChecker& typechecker)
{
	const char FLOAT = 0;
	const char INTEGER = 1;
	const char BOOLEAN = 2;
	char code = 0;
	switch (type.GetType())
	{
	case DataType::Float:
	case DataType::Float2:
	case DataType::Float3:
	case DataType::Float4:
	case DataType::Double:
	case DataType::Double2:
	case DataType::Double3:
	case DataType::Double4:
		code = FLOAT;
		break;
	case DataType::Integer:
	case DataType::Integer2:
	case DataType::Integer3:
	case DataType::Integer4:
	case DataType::UInteger:
	case DataType::UInteger2:
	case DataType::UInteger3:
	case DataType::UInteger4:
	case DataType::Short:
	case DataType::Short2:
	case DataType::Short3:
	case DataType::Short4:
		code = INTEGER;
		break;
	case DataType::Bool:
	case DataType::Bool2:
	case DataType::Bool3:
	case DataType::Bool4:
		code = BOOLEAN;
		break;
	}

	// clear string
	this->formattedString.clear();

	unsigned i;
	for (i = 0; i < this->values.size(); i++)
	{
		std::string fragment;

		if (i > 0)
		{
			this->formattedString.append(", ");
		}

		if (code == FLOAT)
		{
			fragment = Format("%f", this->values[i]->EvalFloat(typechecker));
			this->formattedString.append(fragment);
		}
		else if (code == INTEGER ||
				 code == BOOLEAN)
		{
			fragment = Format("%d", this->values[i]->EvalInt(typechecker));
			this->formattedString.append(fragment);
		}
		else
		{
			std::string typeName = DataType::ToString(type);
			std::string err = Format("Type '%s' cannot be formatted as a value list\n", typeName.c_str());
			Error(err.c_str());
		}
	}
}
} // namespace AnyFX