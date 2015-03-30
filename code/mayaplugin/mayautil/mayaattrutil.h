#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaAttrUtil
  
    Query Maya node attributes.

    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include "mayautil/mayaattrtype.h"
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <maya/MPointArray.h>
#include <maya/MStringArray.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaAttrUtil
{
public:
    /// test if an attribute exists
    static bool HasAttr(const MObject& depNode, const MString& attrId);
    /// get type of an attribute
    static MayaAttrType::Code AttrType(const MObject& depNode, const MString& attrId);
    /// get all attributes of a Maya node
    static MStringArray ListAttrs(const MObject& depNode);
    /// get generic MPlug object of given attribute
    static MPlug GetPlug(const MObject& depNode, const MString& attrId);

    /// get int value
    static int GetInt(const MObject& depNode, const MString& attrId);
    /// get float value
    static float GetFloat(const MObject& depNode, const MString& attrId);
    /// get bool value
    static bool GetBool(const MObject& depNode, const MString& attrId);
    /// get vector value (number of valid components depends on Maya attr)
    static Math::float4 GetVector(const MObject& depNode, const MString& attrId);
    /// get string value
    static Util::String GetString(const MObject& depNode, const MString& attrId);
    /// get enum value
    static Util::String GetEnum(const MObject& depNode, const MString& attrId);
    /// get int array 
    static Util::Array<int> GetIntArray(const MObject& depNode, const MString& attrId);
    /// get double array
    static Util::Array<float> GetFloatArray(const MObject& depNode, const MString& attrId);
    /// get vector array
    static Util::Array<Math::float4> GetVectorArray(const MObject& depNode, const MString& attrId);
    /// get string array
    static Util::Array<Util::String> GetStringArray(const MObject& depNode, const MString& attrId);
    
private:
    /// internal helper method to read a float4 directly from a plug
    static Math::float4 GetFloat4FromPlug(const MPlug& plug);
};

} // namespace Maya
//------------------------------------------------------------------------------

