#pragma once
//------------------------------------------------------------------------------
/**
    @class Sampler
    
    A sampler handles sampling settings as a CPU-side object. 
	In OpenGL, sampler can be set from the CPU as special objects and bound to a texture. Textures can have their own sampler settings aswell.
	In DirectX, sampler objects are required to override default sampler settings.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "samplerrow.h"
#include "symbol.h"
#include "annotation.h"
namespace AnyFX
{

class Sampler : public Symbol
{
public:
	/// constructor
	Sampler();
	/// destructor
	virtual ~Sampler();

	/// set annotation
	void SetAnnotation(const Annotation& annotation);

	/// type checks sampler
	void TypeCheck(TypeChecker& typechecker);
	/// compiles sampler
	void Compile(BinWriter& writer);

	/// consumes a sampler row and sets its internal settings in the actual sampler object
	void ConsumeRow(const SamplerRow& row);

	/// format sampler to fit target language
	std::string Format(const Header& header) const;

private:

	struct InvalidValueContainer
	{
		unsigned entry;
		std::string flag;
		std::string value;
	};

	std::vector<InvalidValueContainer> invalidValues;
	std::vector<std::string> invalidFlags;
	unsigned numEntries;

	Expression* floatExpressions[SamplerRow::NumFloatFlags];
	float floatFlags[SamplerRow::NumFloatFlags];
	Expression* boolExpressions[SamplerRow::NumBoolFlags];
	bool boolFlags[SamplerRow::NumBoolFlags];

	int intFlags[SamplerRow::NumEnumFlags];	
	vector<float, 4> float4Flags[SamplerRow::NumFloat4Flags];
	vector<Expression*, 4> float4Expressions[SamplerRow::NumFloatFlags];

	bool hasTextures;
	SamplerTextureList textureList;

	bool hasAnnotation;	
	Annotation annotation;
}; 

} // namespace AnyFX
//------------------------------------------------------------------------------
