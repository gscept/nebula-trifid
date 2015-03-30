//------------------------------------------------------------------------------
//  function.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "function.h"
#include "antlr3collections.h"
#include "util.h"
#include "typechecker.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Function::Function() :
    fileIndex(-1),
	shaderTarget(false)
{
	this->symbolType = Symbol::FunctionType;

	unsigned i;
	for (i = 0; i < FunctionAttribute::NumIntFlags; i++)
	{
		this->intAttributeMask[i] = false;
		this->intExpressions[i] = 0;
		this->intAttributes[i] = 0;
	}

	for (i = 0; i < FunctionAttribute::NumFloatFlags; i++)
	{
		this->floatAttributeMask[i] = false;
		this->floatExpressions[i] = 0;
		this->floatAttributes[i] = 0;
	}

	for (i = 0; i < FunctionAttribute::NumBoolFlags; i++)
	{
		this->boolAttributeMask[i] = false;
		this->boolAttributes[i] = false;
	}

	for (i = 0; i < FunctionAttribute::NumStringFlags; i++)
	{
		this->stringAttributeMask[i] = false;
		this->stringAttributes[i] = "";
	}
}

//------------------------------------------------------------------------------
/**
*/
Function::~Function()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Function::SetParameters( const std::vector<Parameter>& parameters )
{
	this->parameters = parameters;
}

//------------------------------------------------------------------------------
/**
*/
void 
Function::SetReturnType( const DataType& type )
{
	this->returnType = type;
}

//------------------------------------------------------------------------------
/**
*/
void 
Function::SetCode( const std::string& data )
{
	this->code = data;
}

//------------------------------------------------------------------------------
/**
*/
void 
Function::ConsumeAttribute( const FunctionAttribute& attr )
{
	switch (attr.GetFlagType())
	{
	case FunctionAttribute::ExpressionFlagType:
		if (attr.GetFlag() == "maxvertexcount")			{ this->intExpressions[FunctionAttribute::MaxVertexCount] = attr.GetExpression();  this->intAttributeMask[FunctionAttribute::MaxVertexCount] = true; }
		else if (attr.GetFlag() == "instances")			{ this->intExpressions[FunctionAttribute::Instances] = attr.GetExpression(); this->intAttributeMask[FunctionAttribute::Instances] = true; }
		else if (attr.GetFlag() == "inputvertices")		{ this->intExpressions[FunctionAttribute::InputVertices] = attr.GetExpression(); this->intAttributeMask[FunctionAttribute::InputVertices] = true; }
		else if (attr.GetFlag() == "outputvertices")	{ this->intExpressions[FunctionAttribute::OutputVertices] = attr.GetExpression(); this->intAttributeMask[FunctionAttribute::OutputVertices] = true; }
		else if (attr.GetFlag() == "localsizex")		{ this->intExpressions[FunctionAttribute::LocalSizeX] = attr.GetExpression(); this->intAttributeMask[FunctionAttribute::LocalSizeX] = true; }
		else if (attr.GetFlag() == "localsizey")		{ this->intExpressions[FunctionAttribute::LocalSizeY] = attr.GetExpression(); this->intAttributeMask[FunctionAttribute::LocalSizeY] = true; }
		else if (attr.GetFlag() == "localsizez")		{ this->intExpressions[FunctionAttribute::LocalSizeZ] = attr.GetExpression(); this->intAttributeMask[FunctionAttribute::LocalSizeZ] = true; }		
		else if (attr.GetFlag() == "maxtess")			{ this->floatExpressions[FunctionAttribute::MaxTessellation] = attr.GetExpression(); this->floatAttributeMask[FunctionAttribute::MaxTessellation] = true; }
		else											this->invalidAttributes.push_back(attr.GetFlag());
		break;
	case FunctionAttribute::BoolFlagType:
		if (attr.GetFlag() == "earlydepth")				{ this->boolAttributes[FunctionAttribute::EarlyDepth] = attr.GetBool(); this->boolAttributeMask[FunctionAttribute::EarlyDepth] = true; }
		else											this->invalidAttributes.push_back(attr.GetFlag());
		break;
	case FunctionAttribute::StringFlagType:
		if (attr.GetFlag() == "patchfunction")			{ this->stringAttributes[FunctionAttribute::PatchFunction] = attr.GetString(); this->stringAttributeMask[FunctionAttribute::PatchFunction] = true; }
		else if (attr.GetFlag() == "topology")
		{
			const std::string& value = attr.GetString();
			if (value == "triangle")					{ this->intAttributes[FunctionAttribute::Topology] = FunctionAttribute::Triangle; this->intAttributeMask[FunctionAttribute::Topology] = true; } 
			else if (value == "quad")					{ this->intAttributes[FunctionAttribute::Topology] = FunctionAttribute::Quad; this->intAttributeMask[FunctionAttribute::Topology] = true; } 
			else if (value == "line")					{ this->intAttributes[FunctionAttribute::Topology] = FunctionAttribute::Line; this->intAttributeMask[FunctionAttribute::Topology] = true; } 
			else if (value == "point")					{ this->intAttributes[FunctionAttribute::Topology] = FunctionAttribute::Point; this->intAttributeMask[FunctionAttribute::Topology] = true; } 
			else										this->invalidAttributeValues.push_back(value);
		}
		else if (attr.GetFlag() == "winding")
		{
			const std::string& value = attr.GetString();
			if (value == "cw")							{ this->intAttributes[FunctionAttribute::WindingOrder] = FunctionAttribute::CW; this->intAttributeMask[FunctionAttribute::WindingOrder] = true; }
			else if (value == "ccw")					{ this->intAttributes[FunctionAttribute::WindingOrder] = FunctionAttribute::CCW; this->intAttributeMask[FunctionAttribute::WindingOrder] = true; }
			else										this->invalidAttributeValues.push_back(value);
		}
		else if (attr.GetFlag() == "partition")
		{
			const std::string& value = attr.GetString();
			if (value == "integer")						{ this->intAttributes[FunctionAttribute::PartitionMethod] = FunctionAttribute::Integer; this->intAttributeMask[FunctionAttribute::PartitionMethod] = true; }
			else if (value == "even")					{ this->intAttributes[FunctionAttribute::PartitionMethod] = FunctionAttribute::Even; this->intAttributeMask[FunctionAttribute::PartitionMethod] = true; }
			else if (value == "odd")					{ this->intAttributes[FunctionAttribute::PartitionMethod] = FunctionAttribute::Odd; this->intAttributeMask[FunctionAttribute::PartitionMethod] = true; }
			else if (value == "pow")					{ this->intAttributes[FunctionAttribute::PartitionMethod] = FunctionAttribute::Pow; this->intAttributeMask[FunctionAttribute::PartitionMethod] = true; }
			else										this->invalidAttributeValues.push_back(value);
		}
		else if (attr.GetFlag() == "inputprimitive")
		{
			const std::string& value = attr.GetString();
			if (value == "points")						{ this->intAttributes[FunctionAttribute::InputPrimitive] = FunctionAttribute::IPoints; this->intAttributeMask[FunctionAttribute::InputPrimitive] = true; }
			else if (value == "lines")					{ this->intAttributes[FunctionAttribute::InputPrimitive] = FunctionAttribute::ILines; this->intAttributeMask[FunctionAttribute::InputPrimitive] = true; }
			else if (value == "lines_adjacent")			{ this->intAttributes[FunctionAttribute::InputPrimitive] = FunctionAttribute::ILinesAdjacent; this->intAttributeMask[FunctionAttribute::InputPrimitive] = true; }
			else if (value == "triangles")				{ this->intAttributes[FunctionAttribute::InputPrimitive] = FunctionAttribute::ITriangles; this->intAttributeMask[FunctionAttribute::InputPrimitive] = true; }
			else if (value == "triangles_adjacent")		{ this->intAttributes[FunctionAttribute::InputPrimitive] = FunctionAttribute::ITrianglesAdjacent; this->intAttributeMask[FunctionAttribute::InputPrimitive] = true; }
			else										this->invalidAttributeValues.push_back(value);
		}
		else if (attr.GetFlag() == "outputprimitive")
		{
			const std::string& value = attr.GetString();
			if (value == "points")						{ this->intAttributes[FunctionAttribute::OutputPrimitive] = FunctionAttribute::OPoints; this->intAttributeMask[FunctionAttribute::OutputPrimitive] = true; }
			else if (value == "line_strip")				{ this->intAttributes[FunctionAttribute::OutputPrimitive] = FunctionAttribute::OLineStrip; this->intAttributeMask[FunctionAttribute::OutputPrimitive] = true; }
			else if (value == "triangle_strip")			{ this->intAttributes[FunctionAttribute::OutputPrimitive] = FunctionAttribute::OTriangleStrip; this->intAttributeMask[FunctionAttribute::OutputPrimitive] = true; }
			else										this->invalidAttributeValues.push_back(value);
		}
		else											this->invalidAttributes.push_back(attr.GetFlag());
		break;
	case FunctionAttribute::InvalidFlagType:
		this->invalidAttributes.push_back(attr.GetFlag());
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Function::TypeCheck( TypeChecker& typechecker )
{
	// add function, if failed we must have a redefinition
	if (!typechecker.AddSymbol(this)) return;

	if (this->returnType.GetType() == DataType::Undefined)
	{
		std::string message = Format("Return type of function '%s' is undefined, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	// remove any mcpp #line markers in the source code
	size_t pos = this->code.find("#line");

	while (pos != this->code.npos)
	{
		size_t lineLength = this->code.find_first_of("\n", pos);
		std::string line = this->code.substr(pos, lineLength);
		size_t stringBegin = line.find_first_of('"');
		size_t stringEnd = line.find_last_of('"') + 1;
		this->code = this->code.erase(pos + stringBegin, stringEnd - stringBegin);
		pos = this->code.find("#line", pos + stringEnd - stringBegin);
	}

	// make sure no parameter has an equal attribute
	unsigned i;

	// evaluate all int expressions 
	for (i = 0; i < FunctionAttribute::NumIntFlags; i++)
	{
		if (this->intExpressions[i]) 
		{
			this->intAttributes[i] = this->intExpressions[i]->EvalInt(typechecker);
			delete this->intExpressions[i];
		}
	}

	// then eval all float expressions
	for (i = 0; i < FunctionAttribute::NumFloatFlags; i++)
	{
		if (this->floatExpressions[i])
		{
			this->floatAttributes[i] = this->floatExpressions[i]->EvalFloat(typechecker);
			delete this->floatExpressions[i];
		}
	}

	for (i = 0; i < this->invalidAttributes.size(); i++)
	{
		std::string message = Format("Invalid function attribute '%s', %s\n", this->invalidAttributes[i].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	for (i = 0; i < this->invalidAttributeValues.size(); i++)
	{
		std::string message = Format("Invalid function attribute value '%s', %s\n", this->invalidAttributeValues[i].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(message);
	}

	unsigned j;
	for (i = 0; i < this->parameters.size(); i++)
	{
		const Parameter& firstParam = this->parameters[i];

		// if parameters have no attribute we must make sure we don't have duplicates
		if (firstParam.GetAttribute() != Parameter::NoAttribute)
		{
			for (j = i+1; j < this->parameters.size(); j++)
			{
				const Parameter& secondParam = this->parameters[j];

				if (firstParam.GetName() != secondParam.GetName())
				{
					if (firstParam.GetAttribute() == secondParam.GetAttribute())
					{
						std::string firstAttr = Parameter::AttributeToString(firstParam.GetAttribute());
						std::string secondAttr = Parameter::AttributeToString(secondParam.GetAttribute());
						std::string message = Format("Parameters '%s' (%s) and '%s' (%s) may not have the same qualifiers, %s\n", 
							firstParam.GetName().c_str(), 
							firstAttr.c_str(),
							secondParam.GetName().c_str(),
							secondAttr.c_str(),
							this->ErrorSuffix().c_str());
						typechecker.Error(message);
					}
				}				
			}
		}		
	}

	// type check parameters
	for (i = 0; i < this->parameters.size(); i++)
	{
		this->parameters[i].TypeCheck(typechecker);		
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Function::Restore( const Header& header, int index )
{
	std::string restoredCode;
    this->fileIndex = index;

	const Header::Type& type = header.GetType();

	// restore return value and name
	restoredCode.append(DataType::ToProfileType(this->returnType, type));
	restoredCode.append("\n");
	restoredCode.append(this->name);
	restoredCode.append("(");

	unsigned i;
	for (i = 0; i < this->parameters.size(); i++)
	{
		const Parameter& param = this->parameters[i];

		// write const qualifier if present
		if (param.GetConst())
		{
			restoredCode.append("const ");
		}

		// write IO mode
		if (param.GetIO() == Parameter::Input)
		{
			restoredCode.append("in ");
		}
		else if (param.GetIO() == Parameter::Output)
		{
			restoredCode.append("out ");
		}
		else if (param.GetIO() == Parameter::InputOutput)
		{
			restoredCode.append("inout ");
		}

		restoredCode.append(DataType::ToProfileType(param.GetDataType(), type));
		restoredCode.append(" ");
		restoredCode.append(param.GetName());

		if (param.IsArray())
		{
			std::string arrayString = Format("[%d]", param.GetArraySize());
			restoredCode.append(arrayString);
		}

		if (i < this->parameters.size()-1)
		{
			restoredCode.append(", ");
		}
	}

	// finalize by closing parameter list and append the code
	restoredCode.append(")\n{\n");
	std::string line = Format("#line %d %d\n", this->codeLine, index);
	restoredCode.append(line);
	restoredCode.append(this->code);
	restoredCode.append("\n}\n");

	// now, replace the code with the restored code
	this->code = restoredCode;
}

//------------------------------------------------------------------------------
/**
*/
std::vector<Parameter*> 
Function::GetInputParameters()
{
	std::vector<Parameter*> result;
	unsigned i;
	for (i = 0; i < this->parameters.size(); i++)
	{
		Parameter& param = this->parameters[i];
		if (param.GetIO() == Parameter::Input)
		{
			result.push_back(&param);
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
std::vector<Parameter*> 
Function::GetOutputParameters()
{
	std::vector<Parameter*> result;
	unsigned i;
	for (i = 0; i < this->parameters.size(); i++)
	{
		Parameter& param = this->parameters[i];
		if (param.GetIO() == Parameter::Output)
		{
			result.push_back(&param);
		}
	}
	return result;
}

//------------------------------------------------------------------------------
/**
*/
Parameter* 
Function::GetParameter( unsigned index )
{
	return &this->parameters[index];
}

//------------------------------------------------------------------------------
/**
*/
const Parameter* 
Function::GetParameter( unsigned index ) const
{
    return &this->parameters[index];
}

} // namespace AnyFX
