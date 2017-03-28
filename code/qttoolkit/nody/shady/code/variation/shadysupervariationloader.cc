//------------------------------------------------------------------------------
//  shadysupervariationloader.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadysupervariationloader.h"
#include "shadysupervariation.h"
#include "variable/shadyvariable.h"

using namespace Util;
using namespace Nody;
using namespace IO;
namespace Shady
{
__ImplementClass(Shady::ShadySuperVariationLoader, 'SSUL', Nody::SuperVariationLoader);

//------------------------------------------------------------------------------
/**
*/
ShadySuperVariationLoader::ShadySuperVariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadySuperVariationLoader::~ShadySuperVariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Nody::SuperVariation> 
ShadySuperVariationLoader::Load( const Ptr<IO::Stream>& stream )
{
    // create variation
    Ptr<ShadySuperVariation> variation;

    // create XML reader
    Ptr<XmlReader> reader = XmlReader::Create();
    reader->SetStream(stream);
    reader->Open();

    // set to main scope
    if (!reader->HasNode("/Shady/SuperVariation"))
    {
        n_confirm("ShadyVariationLoader: '%s' is not a valid Shady super variation!", stream->GetURI().LocalPath().AsCharPtr());
        return variation.upcast<Nody::SuperVariation>();
    }

    reader->SetToNode("/Shady/SuperVariation");
    variation = ShadySuperVariation::Create();
    String fileName = stream->GetURI().GetHostAndLocalPath().ExtractFileName();
    fileName.StripFileExtension();
    variation->SetName(fileName);
    this->ParseVariation(reader, variation);
    return variation.upcast<Nody::SuperVariation>();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadySuperVariationLoader::ParseVariation( const Ptr<XmlReader>& reader, const Ptr<ShadySuperVariation>& variation )
{
    String desc = reader->GetString("desc");
    String defines = reader->GetOptString("defines", "");
    variation->SetDefines(defines);
    variation->SetDescription(desc);

    // inputs are treated as 
    if (reader->SetToFirstChild("Input")) do
    {
        this->ParseInput(reader, variation);
    }
    while (reader->SetToNextChild("Input"));

    if (reader->SetToFirstChild("Source")) do
    {
        this->ParseSource(reader, variation);
    }
    while (reader->SetToNextChild("Source"));
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadySuperVariationLoader::ParseInput( const Ptr<XmlReader>& reader, const Ptr<ShadySuperVariation>& variation )
{
    Ptr<ShadyVariable> var = ShadyVariable::Create();

    // get data from xml
    String name = reader->GetString("name");
    String type = reader->GetString("type");
    String result = reader->GetString("result");
    String defines = reader->GetOptString("defines", "");
    var->SetName(name);
    var->SetType(VarType::FromString(type));
    var->SetIOFlag(Variable::Input);
    var->SetResult(ShadyVariable::ResultFromString(result));
    var->SetDefines(defines);

    // add variable
    variation->AddInput(var.upcast<Variable>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadySuperVariationLoader::ParseSource( const Ptr<XmlReader>& reader, const Ptr<ShadySuperVariation>& variation )
{
    // get source language, then depending on the language, put the source in the variation
    String language = reader->GetString("type");
    String path = reader->GetString("template");
    String header = reader->GetString("header");
	String target = reader->GetString("target");

    if (language == "GLSL")         { variation->SetTemplate(path, GLSL); variation->SetHeader(header, GLSL); }
    else if (language == "HLSL")    { variation->SetTemplate(path, HLSL); variation->SetHeader(header, HLSL); }
    else if (language == "PS3")     { variation->SetTemplate(path, PS3); variation->SetHeader(header, PS3); }
    else if (language == "WII")     { variation->SetTemplate(path, WII); variation->SetHeader(header, WII); }
    else if (language == "LUA")     { variation->SetTemplate(path, LUA); variation->SetHeader(header, LUA); }
    else if (language == "JS")      { variation->SetTemplate(path, JS); variation->SetHeader(header, JS); }
    else                            n_error("Unknown source language '%s'!", language.AsCharPtr());
	variation->SetTarget(target);

	// go through includes
	if (reader->SetToFirstChild("Include")) do
	{
		this->ParseInclude(reader, language, variation);
	}
	while (reader->SetToNextChild("Include"));
}

//------------------------------------------------------------------------------
/**
*/
void
ShadySuperVariationLoader::ParseInclude(const Ptr<IO::XmlReader>& reader, const Util::String& language, const Ptr<ShadySuperVariation>& variation)
{
	IO::URI path = reader->GetString("path");
	variation->AddInclude(language, path);
}

} // namespace Shady