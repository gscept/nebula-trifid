//------------------------------------------------------------------------------
//  sampler.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "sampler.h"
#include "samplerrow.h"
#include "util.h"
#include "typechecker.h"
#include <float.h>
#include "variable.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Sampler::Sampler() :
	numEntries(0)
{	
	this->symbolType = Symbol::SamplerType;

	// reset all masks to false
	unsigned i;

	for (i = 0; i < SamplerRow::NumFloatFlags; i++)
	{
		this->floatExpressions[i] = NULL;
	}

	for (i = 0; i < SamplerRow::NumBoolFlags; i++)
	{
		this->boolExpressions[i] = NULL;
	}

	for (i = 0; i < SamplerRow::NumFloat4Flags; i++)
	{
		this->float4Expressions[i].v[0] = NULL;
		this->float4Expressions[i].v[1] = NULL;
		this->float4Expressions[i].v[2] = NULL;
		this->float4Expressions[i].v[3] = NULL;
	}

	// set all values to default
	intFlags[SamplerRow::Filter] = SamplerRow::Linear;
	intFlags[SamplerRow::AddressU] = SamplerRow::Wrap;
	intFlags[SamplerRow::AddressV] = SamplerRow::Wrap;
	intFlags[SamplerRow::AddressW] = SamplerRow::Wrap;
	intFlags[SamplerRow::ComparisonFunc] = LEqual;

	boolFlags[SamplerRow::Comparison] = false;
	
	floatFlags[SamplerRow::LodBias] = 0.0f;
	floatFlags[SamplerRow::MinLod] = -FLT_MAX;
	floatFlags[SamplerRow::MaxLod] = FLT_MAX;
	floatFlags[SamplerRow::MaxAnisotropic] = 16.0f;

	float4Flags[SamplerRow::BorderColor].v[0] = 0.0f;
	float4Flags[SamplerRow::BorderColor].v[1] = 0.0f;
	float4Flags[SamplerRow::BorderColor].v[2] = 0.0f;
	float4Flags[SamplerRow::BorderColor].v[3] = 0.0f;

	hasTextures = false;
}

//------------------------------------------------------------------------------
/**
*/
Sampler::~Sampler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Sampler::ConsumeRow( const SamplerRow& row )
{
	switch (row.GetFlagType())
	{
	case SamplerRow::StringFlagType:
		if (row.GetFlag() == "Filter")
		{
			const std::string& value = row.GetString();
			unsigned flagVal = -1;
			if (value == "MinMagMipPoint")				flagVal = SamplerRow::MinMagMipPoint;
			else if (value == "MinMagMipLinear")		flagVal = SamplerRow::MinMagMipLinear;
			else if (value == "MinMagPointMipLinear")	flagVal = SamplerRow::MinMagPointMipLinear;
			else if (value == "MinMipPointMagLinear")	flagVal = SamplerRow::MinMipPointMagLinear;
			else if (value == "MinPointMipMagLinear")	flagVal = SamplerRow::MinPointMipMagLinear;
			else if (value == "MinLinearMipMagPoint")	flagVal = SamplerRow::MinLinearMipMagPoint;
			else if (value == "MinMipLinearMagPoint")	flagVal = SamplerRow::MinMipLinearMagPoint;
			else if (value == "MinMagLinearMipPoint")	flagVal = SamplerRow::MinMagLinearMipPoint;
			else if (value == "Anisotropic")			flagVal = SamplerRow::Anisotropic;
			else if (value == "Point")					flagVal = SamplerRow::Point;
			else if (value == "Linear")					flagVal = SamplerRow::Linear;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->intFlags[SamplerRow::Filter] = flagVal;
		}
	
		else if (row.GetFlag() == "AddressU")
		{
			const std::string& value = row.GetString();
			unsigned flagVal = -1;
			if (value == "Wrap")				flagVal = SamplerRow::Wrap;
			else if (value == "Mirror")			flagVal = SamplerRow::Mirror;
			else if (value == "Clamp")			flagVal = SamplerRow::Clamp;
			else if (value == "Border")			flagVal = SamplerRow::Border;
			else if (value == "MirrorOnce")		flagVal = SamplerRow::MirrorOnce;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->intFlags[SamplerRow::AddressU] = flagVal;
		}
		else if (row.GetFlag() == "AddressV")
		{
			const std::string& value = row.GetString();
			unsigned flagVal = -1;
			if (value == "Wrap")				flagVal = SamplerRow::Wrap;
			else if (value == "Mirror")			flagVal = SamplerRow::Mirror;
			else if (value == "Clamp")			flagVal = SamplerRow::Clamp;
			else if (value == "Border")			flagVal = SamplerRow::Border;
			else if (value == "MirrorOnce")		flagVal = SamplerRow::MirrorOnce;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->intFlags[SamplerRow::AddressV] = flagVal;
		}
		else if (row.GetFlag() == "AddressW")
		{
			const std::string& value = row.GetString();
			unsigned flagVal = -1;
			if (value == "Wrap")				flagVal = SamplerRow::Wrap;
			else if (value == "Mirror")			flagVal = SamplerRow::Mirror;
			else if (value == "Clamp")			flagVal = SamplerRow::Clamp;
			else if (value == "Border")			flagVal = SamplerRow::Border;
			else if (value == "MirrorOnce")		flagVal = SamplerRow::MirrorOnce;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->intFlags[SamplerRow::AddressW] = flagVal;
		}
		else if (row.GetFlag() == "ComparisonFunc")
		{
			const std::string& value = row.GetString();
			unsigned flagVal = -1;
			if (value == "Never")					flagVal = Never;
			else if (value == "Less")				flagVal = Less;
			else if (value == "Lequal")				flagVal = LEqual;
			else if (value == "Greater")			flagVal = Greater;
			else if (value == "Gequal")				flagVal = GEqual;
			else if (value == "Equal")				flagVal = Equal;
			else if (value == "NotEqual")			flagVal = NEqual;
			else if (value == "Always")				flagVal = Always;
			else
			{
				InvalidValueContainer foo = { this->numEntries, row.GetFlag(), value };
				this->invalidValues.push_back(foo);
				return;
			}

			this->intFlags[SamplerRow::ComparisonFunc] = flagVal;
		}
		else this->invalidFlags.push_back(row.GetFlag());
		break;
	case SamplerRow::ExpressionFlagType:
		if (row.GetFlag() == "LodBias")				this->floatExpressions[SamplerRow::LodBias] = row.GetExpression();
		else if (row.GetFlag() == "MinLod")			this->floatExpressions[SamplerRow::MinLod] = row.GetExpression();
		else if (row.GetFlag() == "MaxLod")			this->floatExpressions[SamplerRow::MaxLod] = row.GetExpression();
		else if (row.GetFlag() == "MaxAnisotropic")	this->floatExpressions[SamplerRow::MaxAnisotropic] = row.GetExpression();
		else if (row.GetFlag() == "Comparison")		this->boolExpressions[SamplerRow::Comparison] = row.GetExpression();
		else this->invalidFlags.push_back(row.GetFlag());
		break;
	case SamplerRow::Float4FlagType:
		if (row.GetFlag() == "BorderColor")			this->float4Expressions[SamplerRow::BorderColor] = row.GetFloat4();
		else this->invalidFlags.push_back(row.GetFlag());
		break;
	case SamplerRow::TextureListFlagType:
		this->textureList = row.GetTextures();
		this->hasTextures = true;
		break;
	default:
		this->invalidFlags.push_back(row.GetFlag());
		break;
	}
	numEntries++;
}

//------------------------------------------------------------------------------
/**
*/
void 
Sampler::TypeCheck( TypeChecker& typechecker )
{
	// add render state, if failed we must have a redefinition
	if (!typechecker.AddSymbol(this)) return;

	unsigned i;
	for (i = 0; i < this->invalidFlags.size(); i++)
	{
		std::string msg = AnyFX::Format("Invalid sampler flag '%s', %s\n", this->invalidFlags[i].c_str(), this->ErrorSuffix().c_str());
		typechecker.Error(msg);
	}

	for (i = 0; i < this->invalidValues.size(); i++)
	{
		std::string msg = AnyFX::Format("Invalid value '%s' for flag '%s' at entry %d, %s\n", this->invalidValues[i].value.c_str(), this->invalidValues[i].flag.c_str(), this->invalidValues[i].entry, this->ErrorSuffix().c_str());
		typechecker.Error(msg);
	}

	// evaluate float expressions
	for (i = 0; i < SamplerRow::NumFloatFlags; i++)
	{
		if (this->floatExpressions[i])
		{
			this->floatFlags[i] = this->floatExpressions[i]->EvalFloat(typechecker);
			delete this->floatExpressions[i];
		}
	}

	for (i = 0; i < SamplerRow::NumBoolFlags; i++)
	{
		if (this->boolExpressions[i])
		{
			this->boolFlags[i] = this->boolExpressions[i]->EvalBool(typechecker);
			delete this->boolExpressions[i];
		}
	}

	for (i = 0; i < SamplerRow::NumFloat4Flags; i++)
	{
		if (this->float4Expressions[i].v[0] &&
			this->float4Expressions[i].v[1] &&
			this->float4Expressions[i].v[2] &&
			this->float4Expressions[i].v[3])
		{
			this->float4Flags[i].v[0] = this->float4Expressions[i].v[0]->EvalFloat(typechecker);
			this->float4Flags[i].v[1] = this->float4Expressions[i].v[1]->EvalFloat(typechecker);
			this->float4Flags[i].v[2] = this->float4Expressions[i].v[2]->EvalFloat(typechecker);
			this->float4Flags[i].v[3] = this->float4Expressions[i].v[3]->EvalFloat(typechecker);

			delete this->float4Expressions[i].v[0];
			delete this->float4Expressions[i].v[1];
			delete this->float4Expressions[i].v[2];
			delete this->float4Expressions[i].v[3];
		}
	}

	if (this->textureList.GetNumTextures() == 0)
	{
		std::string err = AnyFX::Format("Sampler must be supplied with texture identifier, %s\n", this->ErrorSuffix().c_str());
		typechecker.Error(err);
	}
	else
	{
		unsigned numTextures = this->textureList.GetNumTextures();

		// throw a warning if the number of textures in a sampler exceeds 1, since it will result in multiple 
		// sampler_state declarations and the names may collide with previously defined samplers
		if (typechecker.GetHeader().GetType() == Header::HLSL && typechecker.GetHeader().GetMajor() <= 3)
		{
			std::string warn = AnyFX::Format("Sampler '%s' uses multiple texture definitions, compiler will generate multiple samplers, one for each texture with a number succeeding the name. This object will generate samplers %s%d-%s%d, %s\n", this->name.c_str(), this->name.c_str(), 0, this->name.c_str(), numTextures, this->ErrorSuffix().c_str());
			typechecker.Warning(warn);
		}

		unsigned i;
		for (i = 0; i < numTextures; i++)
		{
			// get texture object
			Symbol* textureObject = typechecker.GetSymbol(this->textureList.GetTexture(i));

			if (textureObject)
			{
				// make sure it is a texture variable
				if (textureObject->GetType() >= Symbol::VariableType)
				{
					Variable* var = (Variable*)textureObject;
					if (!(var->GetVarType().GetType() >= DataType::Sampler1D && var->GetVarType().GetType() <= DataType::SamplerCubeArray))
					{
						std::string err = AnyFX::Format("Variable '%s' is not a texture variable, %s\n", textureObject->GetName().c_str(), this->ErrorSuffix().c_str());
						typechecker.Error(err);
					}
				}
				else
				{
					std::string err = AnyFX::Format("Sampler must be provided with a texture variable, '%s' is not a variable, %s\n", textureObject->GetName().c_str(), this->ErrorSuffix().c_str());
					typechecker.Error(err);
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
Sampler::Compile( BinWriter& writer )
{
	writer.WriteString(this->name);

	unsigned i;
	for (i = 0; i < SamplerRow::NumEnumFlags; i++)
	{
		writer.WriteInt(this->intFlags[i]);
	}

	for (i = 0; i < SamplerRow::NumFloatFlags; i++)
	{
		writer.WriteFloat(this->floatFlags[i]);
	}

	for (i = 0; i < SamplerRow::NumFloat4Flags; i++)
	{
		writer.WriteFloat(this->float4Flags[i].v[0]);
		writer.WriteFloat(this->float4Flags[i].v[1]);
		writer.WriteFloat(this->float4Flags[i].v[2]);
		writer.WriteFloat(this->float4Flags[i].v[3]);
	}
	
	for (i = 0; i < SamplerRow::NumBoolFlags; i++)
	{
		writer.WriteBool(this->boolFlags[i]);
	}

	// write texture list
	unsigned numTextures = this->textureList.GetNumTextures();
	writer.WriteInt(numTextures);
	for (i = 0; i < numTextures; i++)
	{
		writer.WriteString(this->textureList.GetTexture(i));
	}
}

//------------------------------------------------------------------------------
/**
*/
std::string 
Sampler::Format( const Header& header ) const
{
	// TODO: fix filter-writing seeing as it is component based on min/mag/mip instead of a single integer
	std::string res;
	if (header.GetType() == Header::HLSL)
	{
		if (header.GetMajor() > 4)
		{
			std::string contents;
			contents.append(AnyFX::Format("Filter = %d;\n", intFlags[SamplerRow::Filter]));
			contents.append(AnyFX::Format("AddressU = %d;\n", intFlags[SamplerRow::AddressU]));
			contents.append(AnyFX::Format("AddressV = %d;\n", intFlags[SamplerRow::AddressV]));
			contents.append(AnyFX::Format("AddressW = %d;\n", intFlags[SamplerRow::AddressW]));
			contents.append(AnyFX::Format("BorderColor = float4(%f,%f,%f,%f);\n", 
				float4Flags[SamplerRow::BorderColor].v[0], 
				float4Flags[SamplerRow::BorderColor].v[1], 
				float4Flags[SamplerRow::BorderColor].v[2],
				float4Flags[SamplerRow::BorderColor].v[3]));
			contents.append(AnyFX::Format("MaxLOD = %f;\n", floatFlags[SamplerRow::MaxLod]));
			contents.append(AnyFX::Format("MinLOD = %f;\n", floatFlags[SamplerRow::MinLod]));
			contents.append(AnyFX::Format("MipLODBias = %f;\n", floatFlags[SamplerRow::LodBias]));
			contents.append(AnyFX::Format("MaxAnisotropy = %f;\n", floatFlags[SamplerRow::MaxAnisotropic]));
			if (this->boolFlags[SamplerRow::Comparison])
			{
				contents.append(AnyFX::Format("ComparisonFunc = %d;\n", intFlags[SamplerRow::ComparisonFunc]));
				res = AnyFX::Format("SamplerComparisonState %s\n{\n%s\n};\n", this->GetName().c_str(), contents.c_str());
			}
			else
			{			
				res = AnyFX::Format("SamplerState %s\n{\n%s\n};\n", this->GetName().c_str(), contents.c_str());
			}
		}
		else
		{
			// write a sampler state per each attached texture
			unsigned numTextures = this->textureList.GetNumTextures();
			unsigned i;
			for (i = 0; i < numTextures; i++)
			{
				std::string contents;
				contents.append(AnyFX::Format("Texture = <%s>;\n", this->textureList.GetTexture(i).c_str()));
				contents.append(AnyFX::Format("AddressU = %d;\n", intFlags[SamplerRow::AddressU]));
				contents.append(AnyFX::Format("AddressV = %d;\n", intFlags[SamplerRow::AddressV]));
				contents.append(AnyFX::Format("AddressW = %d;\n", intFlags[SamplerRow::AddressW]));
				contents.append(AnyFX::Format("BorderColor = float4(%f,%f,%f,%f);\n", 
					float4Flags[SamplerRow::BorderColor].v[0], 
					float4Flags[SamplerRow::BorderColor].v[1], 
					float4Flags[SamplerRow::BorderColor].v[2],
					float4Flags[SamplerRow::BorderColor].v[3]));
				contents.append(AnyFX::Format("MaxLOD = %f;\n", floatFlags[SamplerRow::MaxLod]));
				contents.append(AnyFX::Format("MinLOD = %f;\n", floatFlags[SamplerRow::MinLod]));
				contents.append(AnyFX::Format("MipLODBias = %f;\n", floatFlags[SamplerRow::LodBias]));
				res = AnyFX::Format("sampler_state %s%d\n{\n%s\n};\n", this->GetName().c_str(), i, contents.c_str());
			}
		}
	}
	return res;
}

} // namespace AnyFX