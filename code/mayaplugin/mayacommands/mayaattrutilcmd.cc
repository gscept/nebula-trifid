//------------------------------------------------------------------------------
//  mayaattrutilcmd.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaattrutilcmd.h"
#include "mayautil/mayaattrutil.h"
#include "mayautil/mayanodeutil.h"
#include "mayamain/mayalogger.h"
#include "mayautil/mayatype.h"
#include <maya/MArgDatabase.h>
#include <maya/MDagPathArray.h>



namespace Maya
{
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
bool
MayaAttrUtilCmd::hasSyntax() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaAttrUtilCmd::isUndoable() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void*
MayaAttrUtilCmd::creator()
{
    return new MayaAttrUtilCmd;
}

//------------------------------------------------------------------------------
/**
*/
MSyntax
MayaAttrUtilCmd::getSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-nd", "-node", MSyntax::kString);
    syntax.addFlag("-ha", "-hasattr", MSyntax::kString);
    syntax.addFlag("-at", "-attrtype", MSyntax::kString);
    syntax.addFlag("-l", "-listattrs");
    syntax.addFlag("-gi", "-getint", MSyntax::kString);
    syntax.addFlag("-gf", "-getfloat", MSyntax::kString);
    syntax.addFlag("-gb", "-getbool", MSyntax::kString);
    syntax.addFlag("-gv", "-getvector", MSyntax::kString);
    syntax.addFlag("-gs", "-getstring", MSyntax::kString);
    syntax.addFlag("-ge", "-getenum", MSyntax::kString);
    syntax.addFlag("-gia", "-getintarray", MSyntax::kString);
    syntax.addFlag("-gfa", "-getfloatarray", MSyntax::kString);
    syntax.addFlag("-gva", "-getvectorarray", MSyntax::kString);
    syntax.addFlag("-gsa", "-getstringarray", MSyntax::kString);
    return syntax;
}

//------------------------------------------------------------------------------
/**
    Check if an attribute exists on a node, and (optionally) has the
    expected type.
*/
bool
MayaAttrUtilCmd::CheckAttr(const MObject& node, const MString& attrName, MayaAttrType::Code type)
{
    if (MayaAttrUtil::HasAttr(node, attrName))
    {
        if (MayaAttrType::InvalidAttrType != type)
        {
            if (MayaAttrUtil::AttrType(node, attrName) == type)
            {
                return true;
            }
            else
            {
                MayaLogger::Instance()->Warning("Attr '%s' on node '%s' has invalid type (type is '%s', expected is '%s'!\n",
                    attrName.asChar(),
                    MayaNodeUtil::GetNodeName(node).asChar(),
                    MayaAttrType::ToString(MayaAttrUtil::AttrType(node, attrName)).AsCharPtr(),
                    MayaAttrType::ToString(type).AsCharPtr());
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    else
    {
        MayaLogger::Instance()->Warning("Attr '%s' not found on node '%s'!\n", 
            MayaNodeUtil::GetNodeName(node).asChar(),
            attrName.asChar());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
MStatus
MayaAttrUtilCmd::doIt(const MArgList& argList)
{
    MStatus status;
    MArgDatabase args(syntax(), argList, &status);
    if (MS::kSuccess == status)
    {
        MDagPathArray dagPath;
        MString nodePath;
        MString attrName;

        if (args.isFlagSet("-nd"))
        {
            args.getFlagArgument("-nd", 0, nodePath);
            dagPath = MayaNodeUtil::LookupNode(nodePath);
            if (dagPath.length() == 0)
            {
                MayaLogger::Instance()->Warning("Object doesn't exist: %s\n", nodePath.asChar());
                return MS::kFailure;
            }
        }
        else
        {
            MayaLogger::Instance()->Warning("-nd argument expected!\n");
            return MS::kFailure;
        }
        if (args.isFlagSet("-ha"))
        {
            args.getFlagArgument("-ha", 0, attrName);
            if (attrName.length() > 0)
            {
                this->setResult(MayaAttrUtil::HasAttr(dagPath[0].node(), attrName));
                return MS::kSuccess;
            }
            else
            {
                MayaLogger::Instance()->Warning("-a argument expected!\n");
                return MS::kFailure;
            }
        }
        if (args.isFlagSet("-at"))
        {
            args.getFlagArgument("-at", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName))
            {
                this->setResult(MayaType::Cast<String,MString>(MayaAttrType::ToString(MayaAttrUtil::AttrType(dagPath[0].node(), attrName))));
                return MS::kSuccess;             
            }
        }
        if (args.isFlagSet("-l"))
        {
            this->setResult(MayaAttrUtil::ListAttrs(dagPath[0].node()));
            return MS::kSuccess;
        }
        if (args.isFlagSet("-gi"))
        {
            args.getFlagArgument("-gi", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::Int))
            {
                this->setResult(MayaAttrUtil::GetInt(dagPath[0].node(), attrName));
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gf"))
        {
            args.getFlagArgument("-gf", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::Float))
            {
                this->setResult(MayaAttrUtil::GetFloat(dagPath[0].node(), attrName));
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gb"))
        {
            args.getFlagArgument("-gb", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::Bool))
            {
                this->setResult(MayaAttrUtil::GetBool(dagPath[0].node(), attrName));
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gv"))
        {
            args.getFlagArgument("-gv", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::Vector))
            {
                float4 v = MayaAttrUtil::GetVector(dagPath[0].node(), attrName);
                this->appendToResult(v.x());
                this->appendToResult(v.y());
                this->appendToResult(v.z());
                this->appendToResult(v.w());
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gs"))
        {
            args.getFlagArgument("-gs", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::String))
            {
                this->setResult(MayaType::Cast<String,MString>(MayaAttrUtil::GetString(dagPath[0].node(), attrName)));
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-ge"))
        {
            args.getFlagArgument("-ge", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::Enum))
            {
                this->setResult(MayaType::Cast<String,MString>(MayaAttrUtil::GetEnum(dagPath[0].node(), attrName)));
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gia"))
        {
            args.getFlagArgument("-gia", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::IntArray))
            {
                Array<int> intArray = MayaAttrUtil::GetIntArray(dagPath[0].node(), attrName);
                this->setResult(MayaType::Cast<Array<int>,MIntArray>(intArray));
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gfa"))
        {
            args.getFlagArgument("-gfa", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::FloatArray))
            {
                Array<float> floatArray = MayaAttrUtil::GetFloatArray(dagPath[0].node(), attrName);
                this->setResult(MayaType::Cast<Array<float>,MDoubleArray>(floatArray));
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gva"))
        {
            args.getFlagArgument("-gva", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::VectorArray))
            {
                Array<float4> float4Array = MayaAttrUtil::GetVectorArray(dagPath[0].node(), attrName);
                SizeT i;
                for (i = 0; i < float4Array.Size(); i++)
                {
                    this->appendToResult(float4Array[i].x());
                    this->appendToResult(float4Array[i].y());
                    this->appendToResult(float4Array[i].z());
                    this->appendToResult(float4Array[i].w());
                }
                return MS::kSuccess;
            }
        }
        if (args.isFlagSet("-gsa"))
        {
            args.getFlagArgument("-gsa", 0, attrName);
            if (this->CheckAttr(dagPath[0].node(), attrName, MayaAttrType::StringArray))
            {
                Array<String> stringArray = MayaAttrUtil::GetStringArray(dagPath[0].node(), attrName);
                this->setResult(MayaType::Cast<Array<String>,MStringArray>(stringArray));
                return MS::kSuccess;
            }
        }
    }
    // fallthrough: an error occured
    return MS::kFailure;
}

} // namespace Maya
