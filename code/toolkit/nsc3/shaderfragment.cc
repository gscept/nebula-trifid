//------------------------------------------------------------------------------
//  shaderfragment.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shaderfragment.h"

namespace Tools
{
__ImplementClass(Tools::ShaderFragment, 'SFRG', Core::RefCounted);

using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
ShaderFragment::ShaderFragment() :
    isExclusive(false),
    hasVertexShader(false),
    hasPixelShader(false),
    hasRenderStates(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ShaderFragment::~ShaderFragment()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ShaderFragment>
ShaderFragment::Duplicate(const String& newName) const
{
    Ptr<ShaderFragment> newFragment = ShaderFragment::Create();
    newFragment->fileUri             = this->fileUri;
    newFragment->name                = newName;
    newFragment->category            = this->category;
    newFragment->desc                = this->desc;
    newFragment->isExclusive         = this->isExclusive;
    newFragment->hasVertexShader     = this->hasVertexShader;
    newFragment->hasPixelShader      = this->hasPixelShader;
    newFragment->hasRenderStates     = this->hasRenderStates;
    newFragment->vertexShaderInputs  = this->vertexShaderInputs;
    newFragment->vertexShaderOutputs = this->vertexShaderOutputs;
    newFragment->vertexShaderCode    = this->vertexShaderCode;
    newFragment->pixelShaderInputs   = this->pixelShaderInputs;
    newFragment->pixelShaderOutputs  = this->pixelShaderOutputs;
    newFragment->pixelShaderCode     = this->pixelShaderCode;
    newFragment->renderStates        = this->renderStates;
    return newFragment;
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderFragment::Parse(const Ptr<XmlReader>& xmlReader)
{
    n_assert(xmlReader->GetCurrentNodeName() == "Fragment");
   
    // parse attributes
    n_assert(xmlReader->HasAttr("name"));
    n_assert(xmlReader->HasAttr("category"));
    n_assert(xmlReader->HasAttr("desc"));
    this->name = xmlReader->GetString("name");
    this->category = xmlReader->GetString("category");
    this->desc = xmlReader->GetString("desc");
    if (xmlReader->HasAttr("exclusive"))
    {
        this->isExclusive = xmlReader->GetBool("exclusive");
    }

    // parse vertex shader fragment definitions, if exists
    if (xmlReader->SetToFirstChild("VertexShader"))
    {
        this->hasVertexShader = true;
        this->ParseShaderFragment(xmlReader, this->vertexShaderInputs, this->vertexShaderOutputs, this->vertexShaderCode);
        xmlReader->SetToParent();
    }

    // parse pixel shader fragment definitions, if exists
    if (xmlReader->SetToFirstChild("PixelShader"))
    {
        this->hasPixelShader = true;
        this->ParseShaderFragment(xmlReader, this->pixelShaderInputs, this->pixelShaderOutputs, this->pixelShaderCode);
        xmlReader->SetToParent();
    }

    // parse render state definitions, if exists
    if (xmlReader->SetToFirstChild("RenderStates"))
    {
        this->hasRenderStates = true;
        this->ParseRenderStates(xmlReader);
        xmlReader->SetToParent();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Parse a vertex shader or pixel shader fragment block.
*/
void
ShaderFragment::ParseShaderFragment(const Ptr<XmlReader>& xmlReader,
                                    Dictionary<String,ShaderParam>& outShaderInputs,
                                    Dictionary<String,ShaderParam>& outShaderOutputs,
                                    String& outShaderCode)
{
    // parse shader input params
    if (xmlReader->SetToFirstChild("Inputs"))
    {
        if (xmlReader->SetToFirstChild("Param")) do
        {
            ShaderParam newParam;
            newParam.Parse(xmlReader);
            n_assert(!outShaderInputs.Contains(newParam.GetName()));
            outShaderInputs.Add(newParam.GetName(), newParam);
        }
        while (xmlReader->SetToNextChild("Param"));
        xmlReader->SetToParent();
    }

    // parse shader output params
    if (xmlReader->SetToFirstChild("Outputs"))
    {
        if (xmlReader->SetToFirstChild("Param")) do
        {
            ShaderParam newParam;
            newParam.Parse(xmlReader);
            n_assert(!outShaderOutputs.Contains(newParam.GetName()));
            outShaderOutputs.Add(newParam.GetName(), newParam);
        }
        while (xmlReader->SetToNextChild("Param"));
        xmlReader->SetToParent();
    }

    // parse shader code
    if (xmlReader->SetToFirstChild("Code"))
    {
        n_assert(xmlReader->HasContent());
        outShaderCode = xmlReader->GetContent();
        xmlReader->SetToParent();
    }
}

//------------------------------------------------------------------------------
/**
    Parse render state definitions from XML reader.
*/
void
ShaderFragment::ParseRenderStates(const Ptr<XmlReader>& xmlReader)
{
    if (xmlReader->SetToFirstChild("State")) do
    {
        n_assert(xmlReader->HasAttr("name"));
        n_assert(xmlReader->HasAttr("value"));
        String name = xmlReader->GetString("name");
        String value = xmlReader->GetString("value");
        n_assert(!this->renderStates.Contains(name));
        this->renderStates.Add(name, value);
    }
    while (xmlReader->SetToNextChild("State"));
}

//------------------------------------------------------------------------------
/**
    Returns a function name for the vertex shader function.
*/
String
ShaderFragment::GetVertexShaderFunctionName() const
{
    n_assert(this->HasVertexShader());
    String funcName = "vs";
    funcName.Append(this->name);
    return funcName;
}

//------------------------------------------------------------------------------
/**
    Return a function name for the pixel shader functions.
*/
String
ShaderFragment::GetPixelShaderFunctionName() const
{
    n_assert(this->HasPixelShader());
    String funcName = "ps";
    funcName.Append(this->name);
    return funcName;
}

} // namespace Tools
