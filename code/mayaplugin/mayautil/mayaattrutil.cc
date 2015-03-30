//------------------------------------------------------------------------------
//  mayaattrutil.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayautil/mayaattrutil.h"
#include "mayautil/mayatype.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnStringArrayData.h>

namespace Maya
{
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
bool
MayaAttrUtil::HasAttr(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MFnDependencyNode fnDepNode(depNode, &status);  
    n_assert(MS::kSuccess == status);
    return fnDepNode.hasAttribute(attrId);
}

//------------------------------------------------------------------------------
/**
    Determines the MayaAttrType of a given Maya node attribute. Unknown
    or unsupported types are returned as MayaAttrType::InvalidAttrType.
*/
MayaAttrType::Code
MayaAttrUtil::AttrType(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MFnDependencyNode fnDepNode(depNode, &status);              
    n_assert(MS::kSuccess == status);
    MPlug plug = fnDepNode.findPlug(attrId, true, &status);     
    n_assert(MS::kSuccess == status);
    MObject attrObj = plug.attribute(&status);                  
    n_assert(MS::kSuccess == status);
    switch (attrObj.apiType())
    {
        case MFn::kAttribute2Float:
        case MFn::kAttribute2Double:
        case MFn::kAttribute3Float:
        case MFn::kAttribute3Double:
            if (plug.isArray())
            {
                return MayaAttrType::VectorArray;
            }
            else
            {
                return MayaAttrType::Vector;
            }
            break;

        case MFn::kEnumAttribute:
            return MayaAttrType::Enum;
            break;

        case MFn::kNumericAttribute:
            {
                MFnNumericAttribute fnNumericAttr(attrObj, &status);
                n_assert(MS::kSuccess == status);
                switch (fnNumericAttr.unitType())
                {
                    case MFnNumericData::kLong:
                        if (plug.isArray()) return MayaAttrType::IntArray;
                        else                return MayaAttrType::Int;
                    case MFnNumericData::kFloat:
                    case MFnNumericData::kDouble:
                        if (plug.isArray()) return MayaAttrType::FloatArray;
                        else                return MayaAttrType::Float;
                    case MFnNumericData::kBoolean:
                        return MayaAttrType::Bool;
                }
            }
            break;

        case MFn::kTypedAttribute:
            {
                MFnTypedAttribute fnTypedAttr(attrObj, &status);
                n_assert(MS::kSuccess == status);
                if (MFnData::kString == fnTypedAttr.attrType())
                {
                    return MayaAttrType::String;
                }
                else if (MFnData::kStringArray == fnTypedAttr.attrType())
                {
                    return MayaAttrType::StringArray;
                }
            }
            break;

        default:
            break;
    }
    return MayaAttrType::InvalidAttrType;
}

//------------------------------------------------------------------------------
/**
*/
MStringArray
MayaAttrUtil::ListAttrs(const MObject& depNode)
{
    MStatus status;
    MFnDependencyNode fnDepNode(depNode, &status);  
    n_assert(MS::kSuccess == status);

    uint numAttrs = fnDepNode.attributeCount();
    MStringArray attrNames;
    uint attrIndex;
    for (attrIndex = 0; attrIndex < numAttrs; attrIndex++)
    {
        MObject attrObj = fnDepNode.attribute(attrIndex);
        MFnAttribute fnAttr(attrObj, &status);      
        n_assert(MS::kSuccess == status);
        MString attrName = fnAttr.name();
        attrNames.append(attrName);
    }
    return attrNames;
}

//------------------------------------------------------------------------------
/**
*/
MPlug
MayaAttrUtil::GetPlug(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MFnDependencyNode fnDepNode(depNode, &status);
    n_assert(MS::kSuccess == status);
    MPlug plug = fnDepNode.findPlug(attrId, &status);
    n_assert(MS::kSuccess == status);
    return plug;
}

//------------------------------------------------------------------------------
/**
*/
int
MayaAttrUtil::GetInt(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);
    int value;
    status = plug.getValue(value);
    n_assert(MS::kSuccess == status);
    return value;
}

//------------------------------------------------------------------------------
/**
*/
float
MayaAttrUtil::GetFloat(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);
    float value;
    status = plug.getValue(value);
    n_assert(MS::kSuccess == status);
    return value;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaAttrUtil::GetBool(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);
    bool value;
    status = plug.getValue(value);
    n_assert(MS::kSuccess == status);
    return value;
}

//------------------------------------------------------------------------------
/**
*/
float4
MayaAttrUtil::GetFloat4FromPlug(const MPlug& plug)
{
    MStatus status;
    MObject valueObj;
    status = plug.getValue(valueObj);
    n_assert(MS::kSuccess == status);
    MFnNumericData fnNumericData(valueObj, &status);
    n_assert(MS::kSuccess == status);
    MFnNumericData::Type type = fnNumericData.numericType(&status);
    n_assert(MS::kSuccess == status);
    float4 f4(0.0f, 0.0f, 0.0f, 0.0f);
    double d[4];
    switch (type)
    {
        case MFnNumericData::k2Float:
            status = fnNumericData.getData2Float(f4.x(), f4.y());
            n_assert(MS::kSuccess == status);
            break;

        case MFnNumericData::k3Float:
            status = fnNumericData.getData3Float(f4.x(), f4.y(), f4.z());
            n_assert(MS::kSuccess == status);
            break;

        case MFnNumericData::k2Double:
            status = fnNumericData.getData2Double(d[0], d[1]);
            n_assert(MS::kSuccess == status);
            f4.x() = float(d[0]);
            f4.y() = float(d[1]);
            break;

        case MFnNumericData::k3Double:
            status = fnNumericData.getData3Double(d[0], d[1], d[2]);
            n_assert(MS::kSuccess == status);
            f4.x() = float(d[0]);
            f4.y() = float(d[1]);
            f4.z() = float(d[2]);
            break;

        case MFnNumericData::k4Double:
            status = fnNumericData.getData4Double(d[0], d[1], d[2], d[3]);
            n_assert(MS::kSuccess == status);
            f4.x() = float(d[0]);
            f4.y() = float(d[1]);
            f4.z() = float(d[2]);
            f4.w() = float(d[3]);
            break;

        default:
            n_error("Invalid data type in GetFloat4FromPlug()!");
            break;
    }
    return f4;
}

//------------------------------------------------------------------------------
/**
*/
float4
MayaAttrUtil::GetVector(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);
    float4 f4 = MayaAttrUtil::GetFloat4FromPlug(plug);
    return f4;
}

//------------------------------------------------------------------------------
/**
*/
String
MayaAttrUtil::GetString(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);
    MString value;
    status = plug.getValue(value);
    n_assert(MS::kSuccess == status);
    return MayaType::Cast<MString,String>(value);
}

//------------------------------------------------------------------------------
/**
*/
String
MayaAttrUtil::GetEnum(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MFnDependencyNode fnDepNode(depNode, &status);
    n_assert(MS::kSuccess == status);

    // get enum attribute properties and get the enum field names
    MObject attrObj = fnDepNode.attribute(attrId, &status);
    n_assert(MS::kSuccess == status);
    MFnEnumAttribute fnEnumAttribute(attrObj, &status);
    n_assert(MS::kSuccess == status);

    // now get the enum index, and convert to string
    int enumIndex = MayaAttrUtil::GetInt(depNode, attrId);
    MString result = fnEnumAttribute.fieldName(short(enumIndex), &status);
    n_assert(MS::kSuccess == status);
    return MayaType::Cast<MString,String>(result);
}

//------------------------------------------------------------------------------
/**
*/
Array<int>
MayaAttrUtil::GetIntArray(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);   
    n_assert(plug.isArray());
    uint i;
    uint num = plug.numElements();
    Array<int> result;
    result.Reserve(num);
    for (i = 0; i < num; i++)
    {
        MPlug plugElement = plug.elementByPhysicalIndex(i);
        int val;
        plugElement.getValue(val);
        result.Append(val);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
Array<float>
MayaAttrUtil::GetFloatArray(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);   
    n_assert(plug.isArray());
    uint i;
    uint num = plug.numElements();
    Array<float> result;
    result.Reserve(num);
    for (i = 0; i < num; i++)
    {
        MPlug plugElement = plug.elementByPhysicalIndex(i);
        float val;
        plugElement.getValue(val);
        result.Append(val);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
Array<float4>
MayaAttrUtil::GetVectorArray(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);   
    n_assert(plug.isArray());
    uint i;
    uint num = plug.numElements();
    Array<float4> result;
    result.Reserve(num);
    for (i = 0; i < num; i++)
    {
        MPlug plugElement = plug.elementByPhysicalIndex(i);
        float4 val = MayaAttrUtil::GetFloat4FromPlug(plugElement);
        result.Append(val);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
Array<String>
MayaAttrUtil::GetStringArray(const MObject& depNode, const MString& attrId)
{
    MStatus status;
    MPlug plug = MayaAttrUtil::GetPlug(depNode, attrId);
    n_assert(plug.isArray());
    MObject plugValue;
    status = plug.getValue(plugValue);
    n_assert(MS::kSuccess == status);
    MFnStringArrayData fnStringArrayData(plugValue, &status);
    n_assert(MS::kSuccess == status);
    MStringArray result;
    status = fnStringArrayData.copyTo(result);
    n_assert(MS::kSuccess == status);
    return MayaType::Cast<MStringArray,Array<String>>(result);
}

} // namespace Maya