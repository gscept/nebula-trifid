//------------------------------------------------------------------------------
//  constant.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "constant.h"
#include "typechecker.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Constant::Constant() :
	isArray(false),
	sizeExpression(NULL),
	arrayType(ExplicitArray),
	arraySize(1)
{
	this->symbolType = Symbol::ConstantType;
}

//------------------------------------------------------------------------------
/**
*/
Constant::~Constant()
{
	// size expression gets deleted elsewhere, so empty
}

//------------------------------------------------------------------------------
/**
*/
void
Constant::AddValue(const DataType& type, const ValueList& value)
{
	std::pair<DataType, ValueList> pair;
	pair.first = type;
	pair.second = value;
	this->valueTable.push_back(pair);
}

//------------------------------------------------------------------------------
/**
*/
void
Constant::AddValue(const ValueList& value)
{
	std::pair<DataType, ValueList> pair;
	pair.first = this->type;
	pair.second = value;
	this->valueTable.push_back(pair);
}

//------------------------------------------------------------------------------
/**
*/
std::string
Constant::Format(const Header& header) const
{
	std::string result;
	if (header.GetType() == Header::GLSL)
	{
		result.append("const ");
		result.append(DataType::ToProfileType(this->type, header.GetType()));
		result.append(" ");
		result.append(this->name);
		if (this->isArray)
		{
			std::string number = AnyFX::Format("%d", this->arraySize);
			result.append("[");
			result.append(number);
			result.append("]");
		}
		result.append(" = ");
		result.append(DataType::ToProfileType(this->type, header.GetType()));
		if (this->isArray)
		{
			std::string arraySize = AnyFX::Format("[%d]", this->arraySize);
			result.append(arraySize);
			result.append("(");

			if (this->valueTable.size() != this->arraySize)
			{
				result.append(this->valueTable[0].second.GetString());
			}
			else
			{
				unsigned i;
				for (i = 0; i < this->valueTable.size(); i++)
				{
					if (i > 0)
					{
						result.append(", ");
					}
					result.append(DataType::ToProfileType(this->type, header.GetType()));
					result.append("(");
					result.append(this->valueTable[i].second.GetString());
					result.append(")");
				}
			}			
			result.append(");\n");
		}
		else
		{
			result.append("(");
			result.append(this->valueTable[0].second.GetString());
			result.append(");\n");
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
void
Constant::TypeCheck(TypeChecker& typechecker)
{
	// add symbol first, if this fails, do not continue with type checking
	if (!typechecker.AddSymbol(this)) return;

	// if we have an array, evalute the size
	if (this->isArray)
	{
		if (this->sizeExpression)
		{
			// evaluate constant array size
			this->arraySize = this->sizeExpression->EvalInt(typechecker);
			delete this->sizeExpression;
		}
		else if (this->arrayType == SimpleArray)
		{
			this->arraySize = this->valueTable[0].second.GetNumValues();
		}
		else if (this->arrayType == TypedArray)
		{
			this->arraySize = this->valueTable.size();
		}
	}	

	unsigned i, j;

	// format value table
	for (i = 0; i < this->valueTable.size(); i++)
	{
		this->valueTable[i].second.ConvertToString(this->valueTable[i].first, typechecker);
	}

	// delete expressions
	for (i = 0; i < this->valueTable.size(); i++)
	{
		const ValueList& valueList = this->valueTable[i].second;
		for (j = 0; j < valueList.GetNumValues(); j++)
		{
			if (valueList.GetValue(j)) delete valueList.GetValue(j);
		}		
	}

	// evaluate type correctness of array initializers
	if (this->isArray)
	{
		// branch between verbose arrays (for example with vec2, vec3 etc) and linear arrays (such as arrays of floats, ints, bools...)
		if (this->valueTable.size() != this->arraySize)
		{
			// if we have an int-array, float-array, bool-array...
			if (this->arrayType == SimpleArray)
			{
				std::pair<DataType, ValueList> pair = this->valueTable[0];
				if (this->type.GetStyle() != DataType::Generic)
				{
					std::string message = AnyFX::Format("Non-basic types (types other than float, double, bool, int, uint and short) needs explicit type initializers, %s\n", this->ErrorSuffix().c_str());
					typechecker.Error(message);
				}
				if (pair.second.GetNumValues() != this->arraySize)
				{
					std::string message = AnyFX::Format("Wrong amount of initializers for array, got %d values, expected %d, %s\n", pair.second.GetNumValues(), this->arraySize, this->ErrorSuffix().c_str());
					typechecker.Error(message);
				}
			}
			else
			{
				std::string message = AnyFX::Format("Wrong amount of initializers for array, got %d initializers, expected %d, %s\n", this->valueTable.size(), this->arraySize, this->ErrorSuffix().c_str());
				typechecker.Error(message);
			}			
		}
		else
		{
			unsigned i;
			for (i = 0; i < this->valueTable.size(); i++)
			{
				std::pair<DataType, ValueList> pair = this->valueTable[i];
				if (pair.first != this->type)
				{
					std::string message = AnyFX::Format("Array initializer at index %d has different type than array, %s\n", i+1, this->ErrorSuffix().c_str());
					typechecker.Error(message);
				}
				unsigned numVals = pair.second.GetNumValues();
				unsigned vectorSize = DataType::ToVectorSize(this->type);
				if (numVals != vectorSize)
				{
					std::string message = AnyFX::Format("Initializer at index %d is not completely initialized, got %d initializers, expected %d, %s", i+1, numVals, vectorSize, this->ErrorSuffix().c_str());
					typechecker.Error(message);
				}
			}
		}
	}
	else
	{
		// get vector size of this type
		unsigned vectorSize = DataType::ToVectorSize(this->type);

		unsigned i;
		for (i = 0; i < this->valueTable.size(); i++)
		{
			std::pair<DataType, ValueList> pair = this->valueTable[i];
			if (pair.first != this->type)
			{
				std::string message = AnyFX::Format("Cannot implicitly cast from '%s' to '%s', %s\n", DataType::ToString(this->type).c_str(), DataType::ToString(pair.first).c_str(), this->ErrorSuffix().c_str());
				typechecker.Error(message);
			}
			if (pair.second.GetNumValues() != vectorSize)
			{
				std::string message = AnyFX::Format("Type constructor at index %d isn't fully initialized, got %d values, expected %d, %s\n", i+1, pair.second.GetNumValues(), vectorSize, this->ErrorSuffix().c_str());
				typechecker.Error(message);
			}
		}
	}
}


} // namespace AnyFX
