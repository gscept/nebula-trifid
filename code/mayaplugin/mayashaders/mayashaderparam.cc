//------------------------------------------------------------------------------
//  mayashaderparam.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayashaderparam.h"

namespace Maya
{

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
MayaShaderParam::MayaShaderParam() :
    type(InvalidType),
    isGui(true),
    isExport(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
MayaShaderParam::SetupFromXml(const Ptr<XmlReader>& xmlReader)
{
    this->id = xmlReader->GetString("id");
    this->label = xmlReader->GetString("label");
    this->description = xmlReader->GetString("desc");
    this->isGui = xmlReader->GetOptBool("gui", true);
    this->isExport = xmlReader->GetOptBool("export", true);
    this->type = MayaShaderParam::StringToType(xmlReader->GetString("type"));
    if (ColorEnvelope != this->type)
    {
        this->defaultValue = xmlReader->GetString("def");
    }
    if ((Int == this->type) ||
        (Float == this->type) ||
        (Envelope == this->type))
    {
        this->minValue = xmlReader->GetString("min");
        this->maxValue = xmlReader->GetString("max");
    }
}

//------------------------------------------------------------------------------
/**
*/
MayaShaderParam::Type
MayaShaderParam::StringToType(const String& str)
{
    if (str == "Int") return Int;
    else if (str == "Bool") return Bool;
    else if (str == "Float") return Float;
    else if (str == "Texture") return Texture;
    else if (str == "Vector") return Vector;
    else if (str == "Color") return Color;
    else if (str == "Envelope") return Envelope;
    else if (str == "ColorEnvelope") return ColorEnvelope;
    else
    {
        n_error("Invalid shader param type '%s'!\n", str.AsCharPtr());
        return InvalidType;
    }
}

//------------------------------------------------------------------------------
/**
*/
String
MayaShaderParam::TypeToString(Type t)
{
    switch (t)
    {
        case Int:           return "Int";
        case Bool:          return "Bool";
        case Float:         return "Float";
        case Texture:       return "Texture";
        case Vector:        return "Vector";
        case Color:         return "Color";
        case Envelope:      return "Envelope";
        case ColorEnvelope: return "ColorEnvelope";
        default:
            n_error("Invalid shader param type '%d'!\n", t);
            return "INVALID";
    }
}

} // namespace Maya