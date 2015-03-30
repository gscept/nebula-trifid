//------------------------------------------------------------------------------
//  mayanodeinspector.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayanodeutil.h"
#include "mayautil/mayaattrutil.h"
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlugArray.h>
#include <maya/MFnTransform.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MDagPathArray.h>
#include <maya/MSelectionList.h>

namespace Maya
{

//------------------------------------------------------------------------------
/**
    Test method tests if a node is valid for export. The most important aspect is whether a
    node is visible (invisible nodes are never exported).
    This takes a lot of things into account:
        - if any of the parents is invisible, the node itself is invisible
        - the following special case node names area always treated as visible:
            - collide
            - physics
        - if an rlExportHidden attribute exists and is true, this specific node
          will be treated as visible
        - if the object is part of an invisible layer, the object is treated as invisible
        - if the visibility attribute is set to false, the object is treated as invisible
        - if the template attribute is set to true, the object is treated as invisible
*/
bool
MayaNodeUtil::IsExportNode(const MDagPath& dagPath)
{
    MStatus status;
    bool isVisible = false;

    MFnDagNode fnDagNode(dagPath, &status);
    n_assert(MS::kSuccess == status);
    
    // is an intermediate object?
    if (fnDagNode.isIntermediateObject())
    {
        return false;
    }

    // check object and every parent object up to the root path
    MDagPath curDagPath(dagPath);
    while (curDagPath.length() > 0)
    {
        MObject curNode = curDagPath.node();
        MFnDependencyNode fnDepNode(curNode);

        // check for special cases which override visibility check
        bool forceVisible = false;
        if (MayaAttrUtil::HasAttr(curNode, "rlExportHidden") && 
            MayaAttrUtil::GetBool(curNode, "rlExportHidden"))
        {   
            forceVisible = true;
        }
        MString name = fnDepNode.name();
        if ((name == "collide") ||
            (name == "physics"))
        {
            forceVisible = true;
        }

        // check for invisibility
        if (!forceVisible)
        {
            // check visibility attribute
            if (!MayaAttrUtil::GetBool(curNode, "visibility"))
            {
                return false;
            }

            // check templated attribute
            if (MayaAttrUtil::GetBool(curNode, "template"))
            {
                return false;
            }

            // check if the node is part of an invisible layer
            MPlug drawOverride = MayaAttrUtil::GetPlug(curNode, "drawOverride");
            MPlugArray inputs;
            drawOverride.connectedTo(inputs, true, false, &status);
            if (MS::kSuccess == status)
            {
                IndexT i;
                SizeT num = inputs.length();
                for (i = 0; i < num; i++)
                {
                    const MPlug& inPlug = inputs[i];
                    if (MFn::kDisplayLayer == inPlug.node().apiType())
                    {
                        if (!MayaAttrUtil::GetBool(inPlug.node(), "visibility"))
                        {
                            return false;
                        }
                    }

                    // check if the layer's display type is "Normal"
                    // (alternatives are Template or Reference)
                    if (1 == MayaAttrUtil::GetInt(inPlug.node(), "displayType"))
                    {
                        return false;
                    }
                }
            }
        }
        curDagPath.pop();
    }

    // fallthrough: node is visible
    return true;
}

//------------------------------------------------------------------------------
/**
    This method checks whether the node is marked as a hierarchy node. 
    Hierarchy nodes preserve their hierarchy into the exported
    node structure.
*/
bool
MayaNodeUtil::IsHierarchyNode(const MDagPath& dagPath)
{
    MStatus status;
    MObject node = dagPath.node();

    // check if node is a transform node (only transform nodes can be hierarchy nodes)
    MFnTransform fnTransform(node, &status);
    if (MS::kSuccess != status)
    {
        return false;
    }

    // check the special rlPreserveHierarchy attribute
    if (MayaAttrUtil::HasAttr(node, "rlPreserveHierarchy"))
    {
        return true;
    }

    // check special group names
    MString nodeName = fnTransform.name();
    if (nodeName == "model")
    {
        return true;
    }

    // lod nodes are hierarchy nodes
    if (node.hasFn(MFn::kLodGroup))
    {
        return true;
    }

    // direct children of lod nodes are hierarchy nodes
    MObject parent = fnTransform.parent(0, &status);
    if (MS::kSuccess == status)
    {
        if (parent.hasFn(MFn::kLodGroup))
        {
            return true;
        }
    }

    // animated nodes are hierarchy nodes
    // NOTE: we CANNOT simple MAnimUtil::isAnimated() here!!!
    // since this would also trigger on driven keys in skeletons
    const SizeT numPlugs = 9;
    const char plugArray[numPlugs][32] = 
       {"translateX", "translateY", "translateZ",
        "rotateX", "rotateY", "rotateZ",
        "scaleX", "scaleY", "scaleZ"};
    IndexT i;
    for (i = 0; i < numPlugs; ++i)
    {
        MPlug plug = fnTransform.findPlug(plugArray[i], &status);
        n_assert(MS::kSuccess == status);
        MFnAnimCurve animCurve(plug, &status);
        if (MS::kSuccess == status)
        {
            if (animCurve.numKeys() > 1)
            {
                return true;
            }
        }
    }

    // fallthrough: not a hierarchy node
    return false;
}

//------------------------------------------------------------------------------
/**
    Find one or several dag nodes by name. The name may contain path
    components or wildcards, or it may simply be the same of a single node.
*/
MDagPathArray
MayaNodeUtil::LookupNode(const MString& path)
{
    MStatus status;
    MDagPathArray result;
    MSelectionList selectionList;
    status = selectionList.add(path);
    if (MS::kSuccess == status)
    {
        unsigned int i;
        for (i = 0; i < selectionList.length(); i++)
        {
            MDagPath curDagPath;
            status = selectionList.getDagPath(i, curDagPath);
            if (MS::kSuccess == status)
            {
                result.append(curDagPath);
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    This finds the closest parent which is a hierarchy node. Returns true
    if a hierarchy node has been found. This will NOT take the actual input
    dag path into account.
*/
bool
MayaNodeUtil::LookupParentHierarchyNode(const MDagPath& dagPath, MDagPath& outDagPath)
{
    outDagPath = dagPath;
    while (true)
    {
        if (MS::kSuccess != outDagPath.pop())
        {
            return false;
        }
        if (MayaNodeUtil::IsHierarchyNode(outDagPath))
        {
            return true;
        }
    }
    // can't happen
}

//------------------------------------------------------------------------------
/**
*/
MString
MayaNodeUtil::GetNodeName(const MObject& node)
{
    MStatus status;
    MFnDependencyNode fnDepNode(node, &status);
    n_assert(MS::kSuccess == status);
    return fnDepNode.name();
}

} // namespace Maya