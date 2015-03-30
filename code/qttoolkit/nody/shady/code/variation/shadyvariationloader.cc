//------------------------------------------------------------------------------
//  shadyvariationloader.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyvariationloader.h"
#include "shadyvariation.h"
#include "variable/shadyvariable.h"

using namespace Util;
using namespace Nody;
using namespace IO;
namespace Shady
{
__ImplementClass(Shady::ShadyVariationLoader, 'SHVL', Nody::VariationLoader);

//------------------------------------------------------------------------------
/**
*/
ShadyVariationLoader::ShadyVariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyVariationLoader::~ShadyVariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Nody::Variation> 
ShadyVariationLoader::Load( const Ptr<IO::Stream>& stream )
{
    // create variation
    Ptr<ShadyVariation> variation;

    // create XML reader
    Ptr<XmlReader> reader = XmlReader::Create();
    reader->SetStream(stream);
    reader->Open();

    // set to main scope
    if (!reader->HasNode("/Shady/Variation"))
    {
        n_confirm("ShadyVariationLoader: '%s' is not a valid Shady variation!", stream->GetURI().LocalPath().AsCharPtr());
        return variation.upcast<Nody::Variation>();
    }

    reader->SetToNode("/Shady/Variation");
    variation = ShadyVariation::Create();
    String fileName = stream->GetURI().GetHostAndLocalPath().ExtractFileName();
    fileName.StripFileExtension();
    variation->SetName(fileName);
    this->ParseVariation(reader, variation);
    return variation.upcast<Nody::Variation>();
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyVariationLoader::ParseVariation( const Ptr<XmlReader>& reader, const Ptr<ShadyVariation>& variation )
{
    String desc = reader->GetString("desc");

    // get simulation value name
    String simval = reader->GetOptString("simulationValue", "");
    String simcmd = reader->GetOptString("simulationCommand", "");
    bool siminteractive = reader->GetOptBool("simulationInteractive", false);
    variation->SetSimulationValue(simval);
    variation->SetSimulationCommand(simcmd);
    variation->SetSimulationInteractive(siminteractive);
    variation->SetDescription(desc);
    if (reader->SetToFirstChild("Input")) do
    {
        this->ParseInput(reader, variation);
    }
    while (reader->SetToNextChild("Input"));

    if (reader->SetToFirstChild("Output")) do
    {
        this->ParseOutput(reader, variation);
    }
    while (reader->SetToNextChild("Output"));

    if (reader->SetToFirstChild("Hidden")) do
    {
        this->ParseHidden(reader, variation);
    }
    while (reader->SetToNextChild("Hidden"));

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
ShadyVariationLoader::ParseInput( const Ptr<XmlReader>& reader, const Ptr<ShadyVariation>& variation )
{
    Ptr<ShadyVariable> var = ShadyVariable::Create();

    // get data from xml
    String name = reader->GetString("name");
    String type = reader->GetString("type");
    String defines = reader->GetOptString("defines", "");

    var->SetName(name);
    var->SetType(VarType::FromString(type));
    var->SetIOFlag(Variable::Input);
    var->SetDefines(defines);

    // add variable
    variation->AddInput(var.upcast<Variable>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyVariationLoader::ParseOutput( const Ptr<XmlReader>& reader, const Ptr<ShadyVariation>& variation )
{
    Ptr<ShadyVariable> var = ShadyVariable::Create();

    // get data from xml
    String name = reader->GetString("name");
    String type = reader->GetString("type");
    String defines = reader->GetOptString("defines", "");
    bool isConstant = reader->GetOptBool("constant", false);
    bool isParam = reader->GetOptBool("param", false);
    bool isStatic = reader->GetOptBool("static", false);

    n_assert2(!(isConstant & isParam), "Output cannot be both a constant and a parameter");
    n_assert2(isStatic ? isParam : true, "Output is declared as static but not as parameter");

    uint flags = 0;
    flags |= isConstant ? ShadyVariable::Constant : 0;
    flags |= isParam ? ShadyVariable::Parameter : 0;
    flags |= isStatic ? ShadyVariable::Static : 0;

    var->SetFlags(flags);
    var->SetName(name);
    var->SetType(VarType::FromString(type));
    var->SetIOFlag(Variable::Output);
    var->SetDefines(defines);

    // add variable
    variation->AddOutput(var.upcast<Variable>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyVariationLoader::ParseHidden( const Ptr<IO::XmlReader>& reader, const Ptr<ShadyVariation>& variation )
{
    Ptr<ShadyVariable> var = ShadyVariable::Create();

    // get data from xml
    String name = reader->GetString("name");
    String type = reader->GetString("type");
    bool isStatic = reader->GetOptBool("static", false);

    uint flags = 0;
    flags |= isStatic ? ShadyVariable::Static : 0;

    var->SetName(name);
    var->SetType(VarType::FromString(type));
    var->SetIOFlag(Variable::Hidden);
    var->SetFlags(flags);

    // add variable
    variation->AddHidden(var.upcast<Variable>());
}

//------------------------------------------------------------------------------
/**
*/
void 
ShadyVariationLoader::ParseSource( const Ptr<XmlReader>& reader, const Ptr<ShadyVariation>& variation )
{
    // get source language, then depending on the language, put the source in the variation
    String language = reader->GetString("type");

    Shady::Language lang;
    if (language == "GLSL")         lang = GLSL;
    else if (language == "HLSL")    lang = HLSL;
    else if (language == "PS3")     lang = PS3;
    else if (language == "WII")     lang = WII;
    else if (language == "LUA")     lang = LUA;
    else if (language == "JS")      lang = JS;
    else                            n_error("Unknown source language '%s'!", language.AsCharPtr());

    // traverse sources, each source should point to a singular output, so get code and replace the output with the given output name
    if (reader->SetToFirstChild("Code")) do 
    {
        String varName = reader->GetString("output");
        String source = reader->GetContent();
        const Ptr<ShadyVariable>& var = variation->GetOutput(varName).downcast<ShadyVariable>();
        var->SetSource(source, lang);
    } 
    while (reader->SetToNextChild("Code"));

}


} // namespace Shady