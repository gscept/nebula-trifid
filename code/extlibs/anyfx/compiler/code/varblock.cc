//------------------------------------------------------------------------------
//  varblock.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "varblock.h"
#include "parameter.h"
#include <algorithm>
#include "typechecker.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
VarBlock::VarBlock() :
    bufferExpression(NULL),
    bufferCount(1),
	hasAnnotation(false),
	shared(false),
	noSync(false)
{
	this->symbolType = Symbol::VarblockType;
}

//------------------------------------------------------------------------------
/**
*/
VarBlock::~VarBlock()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
VarBlock::AddVariable( const Variable& var )
{
	this->variables.push_back(var);
}

//------------------------------------------------------------------------------
/**
*/
void
VarBlock::TypeCheck(TypeChecker& typechecker)
{
	// add varblock, if failed we must have a redefinition
	if (!typechecker.AddSymbol(this)) return;

	// type check annotation
	if (this->hasAnnotation)
	{
		this->annotation.TypeCheck(typechecker);
	}

	// evaluate qualifiers
	for (unsigned i = 0; i < this->qualifiers.size(); i++)
	{
		const std::string& qualifier = this->qualifiers[i];
		if (qualifier == "shared") this->shared = true;
		else if (qualifier == "nosync") this->noSync = true;
		else
		{  
			std::string message = AnyFX::Format("Unknown qualifier '%s', %s\n", qualifier.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}

    // evaluate buffer expression
    if (0 != this->bufferExpression)
    {
        DataType bufferExpressionType = this->bufferExpression->EvalType(typechecker);
        if (bufferExpressionType != DataType::Integer)
        {
            std::string message = AnyFX::Format("Varblock buffer size must be an integer, %s\n", this->ErrorSuffix().c_str());
            typechecker.Error(message);
        }
        else
        {
            this->bufferCount = this->bufferExpression->EvalInt(typechecker);
        }
		delete this->bufferExpression;
    }

	unsigned i;
	for (i = 0; i < this->variables.size(); i++)
	{
        Variable& var = this->variables[i];
        if (var.GetArrayType() == Variable::UnsizedArray)
        {
            std::string message = AnyFX::Format("Varblocks cannot contain variables of unsized type! (%s), %s\n", var.GetName().c_str(), this->ErrorSuffix().c_str());
            typechecker.Error(message);
        }

        // since TypeCheck might modify the variable, we must replace the old one. 
		var.TypeCheck(typechecker);
	}

	const Header& header = typechecker.GetHeader();
	Header::Type type = header.GetType();
	int major = header.GetMajor();
	if (type == Header::GLSL)
	{
		if (major < 3)
		{
			std::string message = AnyFX::Format("Varblocks are only supported in GLSL versions 3+, %s\n", this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}
	else if (type == Header::HLSL)
	{
		if (major < 4)
		{
			std::string message = AnyFX::Format("Varblocks are only supported in HLSL versions 4+, %s\n", this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
std::string
VarBlock::Format(const Header& header, const int index) const
{
	std::string formattedCode;

    // only output if we have variables
    if (this->variables.empty()) return formattedCode;

    if (this->shared)
    {
        // varblocks of this type are only available in GLSL3-4 and HLSL4-5
		if (header.GetType() == Header::GLSL)
		{
			std::string layout = AnyFX::Format("layout(shared, binding=%d) uniform ", index);
			formattedCode.append(layout);
		}
        else if (header.GetType() == Header::HLSL) formattedCode.append("shared cbuffer ");
    }
	else
	{
		// varblocks of this type are only available in GLSL3-4 and HLSL4-5
		if (header.GetType() == Header::GLSL)
		{
			std::string layout = AnyFX::Format("layout(shared, binding=%d) uniform ", index);
			formattedCode.append(layout);
		}
        else if (header.GetType() == Header::HLSL) formattedCode.append("cbuffer ");
    }
	
	formattedCode.append(this->GetName());
	formattedCode.append("\n{\n");

	unsigned i;
	for (i = 0; i < this->variables.size(); i++)
	{
		const Variable& var = this->variables[i];

		// format code and add to code
		formattedCode.append(var.Format(header, true));
	}
	
	// finalize and return
	formattedCode.append("};\n\n");
	return formattedCode;
}

//------------------------------------------------------------------------------
/**
*/
void
VarBlock::Compile(BinWriter& writer)
{
	writer.WriteString(this->name);
	writer.WriteBool(this->shared);
	writer.WriteBool(this->noSync);
    writer.WriteUInt(this->bufferCount);

	// write if annotation is used
	writer.WriteBool(this->hasAnnotation);

	// compile annotation if
	if (this->hasAnnotation)
	{
		this->annotation.Compile(writer);
	}	

	// write how many variables we have 
	writer.WriteInt(this->variables.size());

	// compile all variable blocks
	unsigned i;
	for (i = 0; i < this->variables.size(); i++)
	{
		this->variables[i].Compile(writer);
	}
}

//------------------------------------------------------------------------------
/**
	sort hook for sorting parameters
*/
bool
VarblockParamCompare(const Variable& v1, const Variable& v2)
{
	return v1.GetByteSize() * v1.GetArraySize() > v2.GetByteSize() * v1.GetArraySize();
}

//------------------------------------------------------------------------------
/**
	sort variables based on type size in decreasing order, meaning the first value has the biggest size
	(this only needs to be here because there seems to be some bug with uniform buffers (GLSL) on ATI cards)
	but it's always good practice to properly pack your data ;)
*/
void 
VarBlock::SortVariables()
{
	std::sort(this->variables.begin(), this->variables.end(), VarblockParamCompare);
}
} // namespace AnyFX
