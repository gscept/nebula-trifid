//------------------------------------------------------------------------------
//  variable.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "variable.h"
#include "typechecker.h"
#include "util.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Variable::Variable() :
	hasDefaultValue(false),
	isArray(false),
    isSubroutine(false),
	arrayType(ExplicitArray),
	arraySize(1),
	sizeExpression(0),
	bindingUnit(-1),
	format(NoFormat),
	accessMode(NoAccess),
    qualifierFlags(NoQualifiers),
	isUniform(true),
	hasAnnotation(false)
{
	this->symbolType = Symbol::VariableType;
}

//------------------------------------------------------------------------------
/**
*/
Variable::~Variable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
Variable::AddValue(const DataType& type, const ValueList& value)
{
	std::pair<DataType, ValueList> pair;
	pair.first = type;
	pair.second = value;
	this->hasDefaultValue = true;
	this->valueTable.push_back(pair);
}

//------------------------------------------------------------------------------
/**
*/
void
Variable::AddValue(const ValueList& value)
{
	std::pair<DataType, ValueList> pair;
	pair.first = this->type;
	pair.second = value;
	this->hasDefaultValue = true;
	this->valueTable.push_back(pair);
}

//------------------------------------------------------------------------------
/**
*/
void
Variable::SetBindingUnit(int& samplerCount, int& imageCount)
{
	if (this->type.GetType() >= DataType::Sampler1D && this->type.GetType() <= DataType::SamplerCubeArray)
	{
		this->bindingUnit = samplerCount++;
	}
	else if (this->type.GetType() >= DataType::Image1D && this->type.GetType() <= DataType::ImageCubeArray)
	{
		this->bindingUnit = imageCount++;
	}
	else
	{
		this->bindingUnit = -1;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Variable::Preprocess()
{
	// evaluate qualifiers
	unsigned i;
	for (i = 0; i < this->qualifiers.size(); i++)
	{
		const std::string& qualifier = this->qualifiers[i];

		if (qualifier == "rgba32f")				this->format = Variable::RGBA32F;
		else if (qualifier == "rgba16f")		this->format = Variable::RGBA16F;
		else if (qualifier == "rg32f")			this->format = Variable::RG32F;
		else if (qualifier == "rg16f")			this->format = Variable::RG16F;
		else if (qualifier == "r11g11b10f")		this->format = Variable::R11G11B10F;
		else if (qualifier == "r32f")			this->format = Variable::R32F;
		else if (qualifier == "r16f")			this->format = Variable::R16F;
		else if (qualifier == "rgba16")			this->format = Variable::RGBA16;
		else if (qualifier == "rgba8")			this->format = Variable::RGBA8;
		else if (qualifier == "rgb10a2")		this->format = Variable::RGB10A2;
		else if (qualifier == "rg16")			this->format = Variable::RG16;
		else if (qualifier == "rg8")			this->format = Variable::RG8;
		else if (qualifier == "r16")			this->format = Variable::R16;
		else if (qualifier == "r8")				this->format = Variable::R8;
		else if (qualifier == "rgba16snorm")	this->format = Variable::RGBA16SNORM;
		else if (qualifier == "rgba8snorm")		this->format = Variable::RGBA8SNORM;
		else if (qualifier == "rg16snorm")		this->format = Variable::RG16SNORM;
		else if (qualifier == "rg8snorm")		this->format = Variable::RG8SNORM;
		else if (qualifier == "r16snorm")		this->format = Variable::R16SNORM;
		else if (qualifier == "r8snorm")		this->format = Variable::R8SNORM;
		else if (qualifier == "rgba32i")		this->format = Variable::RGBA32I;
		else if (qualifier == "rgba16i")		this->format = Variable::RGBA16I;
		else if (qualifier == "rgba8i")			this->format = Variable::RGBA8I;
		else if (qualifier == "rg32i")			this->format = Variable::RG32I;
		else if (qualifier == "rg16i")			this->format = Variable::RG16I;
		else if (qualifier == "rg8i")			this->format = Variable::RG8I;
		else if (qualifier == "r32i")			this->format = Variable::R32I;
		else if (qualifier == "r16i")			this->format = Variable::R16I;
		else if (qualifier == "r8i")			this->format = Variable::R8I;
		else if (qualifier == "rgba32ui")		this->format = Variable::RGBA32UI;
		else if (qualifier == "rgba16ui")		this->format = Variable::RGBA16UI;
		else if (qualifier == "rgba8ui")		this->format = Variable::RGBA8UI;
		else if (qualifier == "rg32ui")			this->format = Variable::RG32UI;
		else if (qualifier == "rg16ui")			this->format = Variable::RG16UI;
		else if (qualifier == "rg8ui")			this->format = Variable::RG8UI;
		else if (qualifier == "r32ui")			this->format = Variable::R32UI;
		else if (qualifier == "r16ui")			this->format = Variable::R16UI;
		else if (qualifier == "r8ui")			this->format = Variable::R8UI;
		else if (qualifier == "read")			this->accessMode = Variable::Read;
		else if (qualifier == "write")			this->accessMode = Variable::Write;
		else if (qualifier == "readwrite")		this->accessMode = Variable::ReadWrite;
		else if (qualifier == "groupshared")    this->qualifierFlags |= Variable::GroupShared;
		else if (qualifier == "shared")         this->qualifierFlags |= Variable::Shared;
		else if (qualifier == "bindless")       this->qualifierFlags |= Variable::Bindless;
		else
		{
			std::string message = AnyFX::Format("Unknown qualifier '%s', %s\n", qualifier.c_str(), this->ErrorSuffix().c_str());
			AnyFX::Emit(message.c_str());
		}
	}

	// mark variable as uniform
	if (this->qualifierFlags & Variable::GroupShared)
	{
		this->isUniform = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Variable::TypeCheck(TypeChecker& typechecker)
{
	// if type is user defined, check if symbol is defined
	if (this->GetVarType().GetType() == DataType::UserType)
	{
		if (!typechecker.HasSymbol(this->GetVarType().GetName()))
		{
			std::string msg = AnyFX::Format("Undefined type '%s', %s\n", this->GetVarType().GetName().c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(msg);
		}
	}

	// add variable, if failed we must have a redefinition
	if (!typechecker.AddSymbol(this)) return;

	// type check annotation
	if (this->hasAnnotation)
	{
		this->annotation.TypeCheck(typechecker);
	}

    // check to see that the user type is valid to be used with a variable
    if (this->type.GetType() == DataType::UserType)
    {
        // check that usertype is valid
        Symbol* sym = typechecker.GetSymbol(this->type.GetName());
        if (0 != sym)
        {
            if (sym->GetType() != Symbol::StructureType && sym->GetType() != Symbol::SubroutineType)
            {
                std::string msg = AnyFX::Format("Type '%s' must be either a struct or a subroutine prototype, %s\n", this->type.GetName().c_str(), this->ErrorSuffix().c_str());
                typechecker.Error(msg);
            }

            if (sym->GetType() == Symbol::SubroutineType)
            {
                this->isSubroutine = true;
            }
        }
        else
        {
            std::string msg = AnyFX::Format("Type '%s' undefined, %s\n", this->type.GetName().c_str(), this->ErrorSuffix().c_str());
            typechecker.Error(msg);
        }
    }

    if (this->isSubroutine && this->isArray)
    {
        std::string msg = AnyFX::Format("Variable cannot be both a subroutine method pointer and an array, %s\n", this->ErrorSuffix().c_str());
        typechecker.Error(msg);
    }

	// evaluate array size
	if (this->isArray)
	{
        this->EvaluateArraySize(typechecker);
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

    // groupshared is not valid on textures or images
    if (this->GetVarType().GetType() >= DataType::Sampler1D && this->GetVarType().GetType() <= DataType::AtomicCounter)
    {
        if (this->qualifierFlags & Variable::GroupShared)
        {
            std::string message = AnyFX::Format("Qualifier 'groupshared' is not allowed on opaque types (images, samplers and counters), %s\n", this->ErrorSuffix().c_str());
            typechecker.Error(message);
        }
    }

	// evaluate type correctness of array initializers
	if (this->isArray)
	{
		if (this->valueTable.size() != 0)
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
			else if (this->valueTable.size() != this->arraySize)
			{
				std::string message = AnyFX::Format("Wrong amount of initializers for array, got %d initializers, expected %d, %s\n", this->valueTable.size(), this->arraySize, this->ErrorSuffix().c_str());
				typechecker.Error(message);
			}	

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
					std::string message = AnyFX::Format("Initializer at index %d is not completely initialized, got %d initializers, expected %d, %s\n", i+1, numVals, vectorSize, this->ErrorSuffix().c_str());
					typechecker.Error(message);
				}
			}
		}
	}
	else
	{
		// get vector size of this type
		unsigned vectorSize = DataType::ToVectorSize(this->type);
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


	// assume we have a format set when we have an image
	if (this->type.GetType() >= DataType::Image1D && this->type.GetType() <= DataType::ImageCubeArray)
	{
		if (this->format == NoFormat)
		{
			std::string message = AnyFX::Format("Variable '%s' is of image type but does not have a required format qualifier, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}

		if (this->accessMode == NoAccess)
		{
			std::string message = AnyFX::Format("Variable '%s' is of image type but does not have a required access qualifier, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}
	else
	{
		if (this->format != NoFormat)
		{
			std::string message = AnyFX::Format("Variable '%s' has an image format qualifier, but is not an image variable, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Warning(message);
		}

		if (this->accessMode != NoAccess)
		{
			std::string message = AnyFX::Format("Variable '%s' has an image access qualifier, but is not an image variable, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Error(message);
		}
	}
	
	const Header::Type type = typechecker.GetHeader().GetType();
	if (type == Header::GLSL)
	{
		if (this->type.GetStyle() != DataType::GLSL && this->type.GetStyle() != DataType::Generic)
		{
			std::string message = AnyFX::Format("Variable '%s' is not formatted in GLSL, although compilation target is GLSL, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Warning(message);
		}
	}
	else if (type == Header::HLSL)
	{
		if (this->type.GetStyle() != DataType::HLSL && this->type.GetStyle() != DataType::Generic)
		{
			std::string message = AnyFX::Format("Variable '%s' is not formatted in HLSL, although compilation target is HLSL, %s\n", this->name.c_str(), this->ErrorSuffix().c_str());
			typechecker.Warning(message);
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Variable::Compile(BinWriter& writer)
{
    bool shared = (this->qualifierFlags & Variable::Shared) == 0 ? false : true;
	writer.WriteString(this->name);
    writer.WriteBool(shared);
    writer.WriteBool((this->qualifierFlags & Bindless) != 0);
	writer.WriteInt(this->bindingUnit);
	writer.WriteInt(this->type.GetType());

	// if this is a compute variable, write the format and access mode to stream
	if (this->type.GetType() >= DataType::Image1D && this->type.GetType() <= DataType::ImageCubeArray)
	{
		writer.WriteInt(this->format);
		writer.WriteInt(this->accessMode);
	}

	// write if annotation is used
	writer.WriteBool(this->hasAnnotation);
	if (this->hasAnnotation)
	{
		this->annotation.Compile(writer);
	}	

	// write if this variable is an array
	writer.WriteBool(this->isArray);
	if (this->isArray)
	{
		writer.WriteInt(this->arraySize);
	}

    // write if variable is a subroutine method pointer
    writer.WriteBool(this->isSubroutine);

	// write if we have a default value
	writer.WriteBool(this->hasDefaultValue);
	
	if (this->HasDefaultValue())
	{
		// we want to concatenate all values into a single string
		std::string valueString;

		// reformat all string as a long list of values
		unsigned i;
		for (i = 0; i < this->valueTable.size(); i++)
		{
			if (i > 0)
			{
				valueString.append(", ");
			}
			valueString.append(this->valueTable[i].second.GetString());
		}

		// write amount of array default variables
		writer.WriteString(valueString);
	}
}

//------------------------------------------------------------------------------
/**
	Format variable code, this is pretty generic and is only dependent on how to format the types
*/
std::string
Variable::Format(const Header& header, bool inVarblock) const
{
	std::string formattedCode;

	if (header.GetFlags() & Header::NoSubroutines && this->isSubroutine) return formattedCode;
	if (this->type.GetType() >= DataType::Image1D && this->type.GetType() <= DataType::ImageCubeArray)
	{
		if (this->bindingUnit == -1) AnyFX::Emit("Texture %s has an invalid texture unit!", this->ErrorSuffix().c_str());
		formattedCode.append(AnyFX::Format("layout(binding = %d, ", this->bindingUnit));
		formattedCode.append(this->FormatImageFormat(header));
		formattedCode.append(") ");
		formattedCode.append(this->FormatImageAccess(header));
		formattedCode.append(" ");
	}    
    else if (this->type.GetType() >= DataType::Sampler1D && this->type.GetType() <= DataType::SamplerCubeArray)
    {
		if (this->bindingUnit == -1) AnyFX::Emit("Texture %s has an invalid texture unit!", this->ErrorSuffix().c_str());
		if (this->qualifierFlags & Bindless) formattedCode.append(AnyFX::Format("layout(binding = %d, bindless_sampler) ", this->bindingUnit));
		else								 formattedCode.append(AnyFX::Format("layout(binding = %d) ", this->bindingUnit));
    }
	else if (this->type.GetType() >= DataType::Matrix2x2 && this->type.GetType() <= DataType::Matrix4x4 && inVarblock)
	{
		formattedCode.append("layout(column_major) ");
	}

	// add GLSL uniform qualifier
	if (header.GetType() == Header::GLSL && !inVarblock)
	{
        if (this->isSubroutine)                 formattedCode.append("subroutine ");
        if (this->qualifierFlags & GroupShared) formattedCode.append("shared ");
		else                                    formattedCode.append("uniform ");
	}

	formattedCode.append(DataType::ToProfileType(this->GetVarType(), header.GetType()));
	formattedCode.append(" ");
	formattedCode.append(this->GetName());
	if (this->isArray)
	{
		std::string number = AnyFX::Format("%d", this->arraySize);
		formattedCode.append("[");
		if (this->arrayType != UnsizedArray) formattedCode.append(number);
		formattedCode.append("]");
	}
	formattedCode.append(";\n");
	return formattedCode;
}

//------------------------------------------------------------------------------
/**
*/
const unsigned 
Variable::GetByteSize() const
{
    return DataType::ToByteSize(this->type);
}

//------------------------------------------------------------------------------
/**
*/
std::string 
Variable::FormatImageFormat(const Header& header) const
{
	if (header.GetType() == Header::GLSL)
	{
		switch (this->format)
		{
		case RGBA32F:
			return "rgba32f";
		case RGBA16F:
			return "rgba16f";
		case RG32F:
			return "rg32f";
		case RG16F:
			return "rg16f";
		case R11G11B10F:
			return "r11f_g11f_b10f";
		case R32F:
			return "r32f";
		case R16F:
			return "r16f";
		case RGBA16:
			return "rgba16";
		case RGB10A2:
			return "rgb10_a2";
		case RGBA8:
			return "rgba8";
		case RG16:
			return "rg16";
		case RG8:
			return "rg8";
		case R16:
			return "r16";			
		case R8:
			return "r8";
		case RGBA16SNORM:
			return "rgba16_snorm";
		case RGBA8SNORM:
			return "rgba8_snorm";
		case RG16SNORM:
			return "rg16_snorm";
		case RG8SNORM:
			return "rg8_snorm";
		case R16SNORM:
			return "r16_snorm";
		case R8SNORM:
			return "r8_snorm";
		case RGBA32I:
			return "rgba32i";
		case RGBA16I:
			return "rgba16i";
		case RGBA8I:
			return "rgba8i";
		case RG32I:
			return "rg32i";
		case RG16I:
			return "rg16i";
		case RG8I:
			return "rg8i";
		case R32I:
			return "r32i";
		case R16I:
			return "r16i";
		case R8I:
			return "r8i";
		case RGBA32UI:
			return "rgba32ui";
		case RGBA16UI:
			return "rgba16ui";
		case RGBA8UI:
			return "rgba8ui";
		case RG32UI:
			return "rg32ui";
		case RG16UI:
			return "rg16ui";
		case RG8UI:
			return "rg8ui";
		case R32UI:
			return "r32ui";
		case R16UI:
			return "r16ui";
		case R8UI:
			return "r8ui";
		}
	}
    else
    {
        // IMPLEMENT ME
    }

	// fallthrough
	return "";
}

//------------------------------------------------------------------------------
/**
*/
std::string 
Variable::FormatImageAccess( const Header& header ) const
{
	if (header.GetType() == Header::GLSL)
	{
		switch (this->accessMode)
		{
		case Read:
			return "readonly";
		case Write:
			return "writeonly";
		case ReadWrite:
			return "restrict";
		}
	}
    else
    {
        // IMPLEMENT ME
    }

	// fallthrough
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
Variable::EvaluateArraySize(TypeChecker& typechecker)
{
    if (this->sizeExpression)
    {
        // evaluate constant array size
        this->arraySize = this->sizeExpression->EvalInt(typechecker);
        delete this->sizeExpression;
        this->sizeExpression = 0;
    }
    else if (this->arrayType == SimpleArray)
    {
        this->arraySize = this->valueTable[0].second.GetNumValues();
    }
    else if (this->arrayType == TypedArray)
    {
        this->arraySize = this->valueTable.size();
    }
    else if (this->arrayType == UnsizedArray)
    {
        // an unsized array should count as a single type when determining the size, since it's practically undecided.
        this->arraySize = 1;
    }
}
} // namespace AnyFX
