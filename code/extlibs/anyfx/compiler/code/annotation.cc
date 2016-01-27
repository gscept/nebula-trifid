//------------------------------------------------------------------------------
//  annotation.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "annotation.h"
#include "util.h"
#include "typechecker.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Annotation::Annotation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Annotation::~Annotation()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Annotation::AddType(const DataType& type)
{
	this->types.push_back(type);
}

//------------------------------------------------------------------------------
/**
*/
void
Annotation::AddName(const std::string& name)
{
	this->names.push_back(name);
}

//------------------------------------------------------------------------------
/**
*/
void
Annotation::AddExpression(Expression* expr)
{
	variant val;
	val.expr = expr;
	this->values.push_back(val);
}

//------------------------------------------------------------------------------
/**
*/
void
Annotation::AddString(const std::string& value)
{
	variant val;
	val.stringValue = new std::string(value);
	this->values.push_back(val);
}

//------------------------------------------------------------------------------
/**
*/
void
Annotation::TypeCheck(TypeChecker& typechecker)
{
	unsigned i;

	// evaluate expressions
	for (i = 0; i < this->values.size(); i++)
	{
		if (this->values[i].expr)
		{
			switch(this->types[i].GetType())
			{
			case DataType::Integer:
				this->values[i].intValue = this->values[i].expr->EvalInt(typechecker);
				break;
			case DataType::Bool:
				this->values[i].boolValue = this->values[i].expr->EvalBool(typechecker);
				break;
			case DataType::Double:
				this->values[i].doubleValue = this->values[i].expr->EvalFloat(typechecker);
				break;
			case DataType::Float:
				this->values[i].floatValue = this->values[i].expr->EvalFloat(typechecker);
				break;
			case DataType::String:	// skip strings, because otherwise we will delete the string outside the switch
				continue;
			}
			delete this->values[i].expr;
			this->values[i].expr = NULL;
		}
	}

	for (i = 0; i < this->types.size(); i++)
	{
		// check for redefinition
		unsigned j;
		for (j = i+1; j < this->names.size(); j++)
		{
			if (this->names[i] == this->names[j])
			{
				std::string message = Format("Annotation name '%s' redefined, %s\n", this->names[i].c_str(), this->ErrorSuffix().c_str());
				typechecker.Error(message);
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Annotation::Compile(BinWriter& writer)
{
	writer.WriteInt(this->values.size());
	unsigned i;
	for (i = 0; i < this->values.size(); i++)
	{
		// write name of annotation
		writer.WriteString(this->names[i]);

		// write data type
		DataType::Type type = this->types[i].GetType();
		writer.WriteInt(type);

		switch (type)
		{
		case DataType::Integer:
			writer.WriteInt(this->values[i].intValue);
			break;
		case DataType::Bool:
			writer.WriteBool(this->values[i].boolValue);
			break;
		case DataType::Double:
			writer.WriteDouble(this->values[i].doubleValue);
			break;
		case DataType::Float:
			writer.WriteFloat(this->values[i].floatValue);
			break;
		case DataType::String:
			writer.WriteString(*this->values[i].stringValue);
			delete this->values[i].stringValue;
			break;
		}
		
	}
}
} // namespace AnyFX