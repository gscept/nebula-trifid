//------------------------------------------------------------------------------
//  mayashader.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayashader.h"

namespace Maya
{
using namespace CoreGraphics;
using namespace Util;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
MayaShader::MayaShader() :
    isObsolete(false),
    shaderFilter(MayaShaderFilter::InvalidShaderFilter)
{
    this->uvSetMapping.SetSize(4);
    this->uvSetMapping[0] = "map1";
}

//------------------------------------------------------------------------------
/**
*/
MayaShader::~MayaShader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
MayaShader::SetupFromXml(const Ptr<XmlReader>& xmlReader)
{
    this->id = xmlReader->GetString("id");
    this->displayName = xmlReader->GetString("label");
    this->shaderName = xmlReader->GetString("shader");
    this->nodeClass = xmlReader->GetString("class");
    this->shaderFilter = MayaShaderFilter::FromString(xmlReader->GetString("filter").AsCharPtr());
    this->isObsolete = xmlReader->GetOptBool("obsolete", false);
    
    // parse description
    xmlReader->SetToNode("Desc");
    this->description = xmlReader->GetContent();
    xmlReader->SetToParent();

    // parse vertex components
    xmlReader->SetToNode("VertexComponents");
    if (xmlReader->SetToFirstChild("Comp")) do
    {
        // build vertex component object
        String semNameString = xmlReader->GetString("name");
        IndexT semIndex = xmlReader->GetInt("index");
        String fmtString = xmlReader->GetString("format");
        VertexComponent::SemanticName semName = VertexComponent::StringToSemanticName(semNameString);
        VertexComponent::Format fmt = VertexComponent::StringToFormat(fmtString);
        VertexComponent comp(semName, semIndex, fmt);
        this->vertexComponents.Append(comp);

        // setup UV set name mapping if defined
        if (semName == VertexComponent::TexCoord)
        {
            this->uvSetMapping[semIndex] = xmlReader->GetString("mayaUvSet");
        }
    }
    while (xmlReader->SetToNextChild("Comp"));
    xmlReader->SetToParent();

    // parse shader parameters
    xmlReader->SetToNode("Params");
    if (xmlReader->SetToFirstChild("Param")) do
    {
        MayaShaderParam newParam;
        newParam.SetupFromXml(xmlReader);
        this->params.Append(newParam);
        this->paramIndexMap.Add(newParam.GetId(), this->params.Size() - 1);
    }
    while (xmlReader->SetToNextChild("Param"));
    xmlReader->SetToParent();
}

} // namespace Maya
