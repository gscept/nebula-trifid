//------------------------------------------------------------------------------
//  sequencer/xmlattrparser.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "sequencer/xmlattrparser.h"
#include "sequencer/beziergraph.h"
#include "sequencer/beziercurve.h"
#include "util/string.h"

namespace Sequencer
{

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
XmlAttrParser::XmlAttrParser()
{
}

//------------------------------------------------------------------------------
/**
*/
XmlAttrParser::~XmlAttrParser()
{
}

//------------------------------------------------------------------------------
/**
    Parse a Transformation XML Node
*/
void
XmlAttrParser::ParseTransformation(const Ptr<IO::XmlReader>& xml, 
        float4& translation,
        float4& rotation,
        float4& scale)
{
    // initialze
    translation = float4(0, 0, 0, 0);
    scale = float4(1, 1, 1, 0);
    rotation = float4(0, 0, 0, 0);

    // parse attributes
    // parse children
    if (xml->SetToFirstChild(String("Attribute"))) do
    {
        String nameString = xml->GetString(String("name"));
        n_assert(nameString.IsValid());
        if ("tx" == nameString)
        {
            translation.x() = xml->GetFloat(String("value"));
        }
        else if ("ty" == nameString)
        {
            translation.y() = xml->GetFloat(String("value"));
        }
        else if ("tz" == nameString)
        {
            translation.z() = xml->GetFloat(String("value"));
        }
        else if ("sx" == nameString)
        {
            scale.x() = xml->GetFloat(String("value"));
        }
        else if ("sy" == nameString)
        {
            scale.y() = xml->GetFloat(String("value"));
        }
        else if ("sz" == nameString)
        {
            scale.z() = xml->GetFloat(String("value"));
        }
        else if ("rx" == nameString)
        {
            rotation.x() = xml->GetFloat(String("value"));
        }
        else if ("ry" == nameString)
        {
            rotation.y() = xml->GetFloat(String("value"));
        }
        else if ("rz" == nameString)
        {
            rotation.z() = xml->GetFloat(String("value"));
        }
    }
    while (xml->SetToNextChild("Attribute"));
}

//------------------------------------------------------------------------------
/**
    Parse a Vector3 Xml Node
*/
void
XmlAttrParser::ParseVector4(Ptr<IO::XmlReader>& xml, float4& vecValue)
{
    // initialze
    vecValue = float4(0, 0, 0, 0);

    // parse attributes
    // parse children
    if (xml->SetToFirstChild("Attribute")) do
    {
        String nameString = xml->GetString(String("name"));
        n_assert(nameString.IsValid());
        if ("x" == nameString)
        {
            vecValue.x() = xml->GetFloat(String("value"));
        }
        else if ("y" == nameString)
        {
            vecValue.y() = xml->GetFloat(String("value"));
        }
        else if ("z" == nameString)
        {
            vecValue.z() = xml->GetFloat(String("value"));
        }
    }
    while (xml->SetToNextChild("Attribute"));
}
//------------------------------------------------------------------------------
/**
    Parse a Vector3 Xml Node
*/
float2
XmlAttrParser::ParseVector2(const String& value)
{
    float2 retValue;
    Util::Array<String> tokens = value.Tokenize(",");
    n_assert(tokens.Size() ==2);
    retValue.x() = tokens[0].AsFloat();
    retValue.y() = tokens[1].AsFloat();
    return retValue;
}
//------------------------------------------------------------------------------
/**
*/
void
XmlAttrParser::ParseAnimatedFloat(const Ptr<IO::XmlReader>& xml, AnimatedParameter<float> &animFloat, bool isDegrees)
{
    if(isDegrees)
    {
        float deg = xml->GetFloat(String("value"));
        float rad = deg/180 * PI;
        animFloat.SetValue(rad);
    }
    else
    {
        animFloat.SetValue(xml->GetFloat(String("value")));
    }

    if ("True" == xml->GetString("isAnimated"))
    {
        Ptr<BezierGraph> graph = BezierGraph::Create();
        // set to graph element
        bool validChild = xml->SetToFirstChild("Graph");
        n_assert(validChild);
        XmlAttrParser::ParseBezierGraph(xml, graph, isDegrees);
        animFloat.SetGraph(graph);
        animFloat.SetAnimated(true);
        xml->SetToNode("..");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
XmlAttrParser::ParseAnimatedVector4(Ptr<IO::XmlReader>& xml, AnimatedParameter<float4> &animVec)
{
    // find value x
    bool validChild = xml->SetToFirstChild("Attribute");
    n_assert(validChild);
    XmlAttrParser::ParseAnimatedFloat(xml, animVec.GetFloat(0));
    validChild = xml->SetToNextChild("Attribute");
    n_assert(validChild);
    XmlAttrParser::ParseAnimatedFloat(xml, animVec.GetFloat(1));
    validChild = xml->SetToNextChild("Attribute");
    n_assert(validChild);
    XmlAttrParser::ParseAnimatedFloat(xml, animVec.GetFloat(2));
    validChild = xml->SetToNextChild("Attribute");
    //Hack
    //n_assert(validChild);
   	if(validChild)
   	{
   		XmlAttrParser::ParseAnimatedFloat(xml, animVec.GetFloat(3));
   		xml->SetToNode("..");
   	}
}

//------------------------------------------------------------------------------
/**
*/
void
XmlAttrParser::ParseAnimatedTransform(	const Ptr<IO::XmlReader>& xml, 
										AnimatedParameter<float4>& translation,
										AnimatedParameter<float4>& rotation,
										AnimatedParameter<float4>& scale)
{
    if (xml->SetToFirstChild("Attribute")) do
    {
        String nameString = xml->GetString(String("name"));
        n_assert(nameString.IsValid());
        if ("tx" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, translation.GetFloat(0));
        }
        else if ("ty" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, translation.GetFloat(1));
        }
        else if ("tz" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, translation.GetFloat(2));
        }
        else if ("sx" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, scale.GetFloat(0));
        }
        else if ("sy" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, scale.GetFloat(1));
        }
        else if ("sz" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, scale.GetFloat(2));
        }
        else if ("rx" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, rotation.GetFloat(0), true);
        }
        else if ("ry" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, rotation.GetFloat(1), true);
        }
        else if ("rz" == nameString)
        {
            XmlAttrParser::ParseAnimatedFloat(xml, rotation.GetFloat(2), true);
        }
    }
    while (xml->SetToNextChild("Attribute"));
}
//------------------------------------------------------------------------------
/**
*/
void
XmlAttrParser::ParseBezierGraph(const Ptr<IO::XmlReader>& xml, Ptr<BezierGraph> graph, bool isDegrees)
{
    graph->SetPreInfinity(xml->GetString("preInfinity"));
    graph->SetPostInfinity(xml->GetString("postInfinity"));
    if (xml->SetToFirstChild("BezierSpline")) do
    {
        Ptr<BezierCurve> curve = BezierCurve::Create();
        
        if(isDegrees)
        {
            float2 key;
            float deg = 0;
            float rad = 0;

            key = ParseVector2(xml->GetString("startPoint"));
            deg = key.y();
            rad = deg/180 * PI;
            curve->SetStartP(float2(key.x(), rad));

            key = ParseVector2(xml->GetString("startCP"));
            deg = key.y();
            rad = deg/180 * PI;
            curve->SetStartCP(float2(key.x(), rad));

            key = ParseVector2(xml->GetString("endPoint"));
            deg = key.y();
            rad = deg/180 * PI;
            curve->SetEndP(float2(key.x(), rad));

            key = ParseVector2(xml->GetString("endCP"));
            deg = key.y();
            rad = deg/180 * PI;
            curve->SetEndCP(float2(key.x(), rad));
        }
        else
        {
            curve->SetStartP(ParseVector2(xml->GetString("startPoint")));
            curve->SetStartCP(ParseVector2(xml->GetString("startCP")));
            curve->SetEndP(ParseVector2(xml->GetString("endPoint")));
            curve->SetEndCP(ParseVector2(xml->GetString("endCP")));
        }
        
        curve->CalculatePolynome();
        graph->AddBezierCurve(curve);
    }
    while (xml->SetToNextChild("BezierSpline"));
}


}  // namespace Sequencer