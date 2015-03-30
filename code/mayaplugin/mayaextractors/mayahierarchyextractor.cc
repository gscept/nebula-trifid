//------------------------------------------------------------------------------
//  mayahierarchyextractor.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaextractors/mayahierarchyextractor.h"
#include "mayautil/mayanodeutil.h"
#include "mayautil/mayatransformutil.h"
#include "mayautil/mayatype.h"
#include "mayacore/mayascene.h"
#include <maya/MDagPathArray.h>
#include <maya/MFnDagNode.h>

namespace Maya
{
__ImplementClass(Maya::MayaHierarchyExtractor, 'MHYX', Maya::MayaExtractorBase);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
MayaStatus::Code
MayaHierarchyExtractor::Extract(const Ptr<MayaScene>& mayaScene)
{
    // start at the "|model" node and recurse
    MDagPathArray modelNode = MayaNodeUtil::LookupNode("|model");
    if (modelNode.length() > 0)
    {
        this->ExtractNode(mayaScene, modelNode[0]);
        return MayaStatus::Success;
    }
    else
    {
        // no "|model" node in scene
        return MayaStatus::NothingTodo;
    }
}

//------------------------------------------------------------------------------
/**
    Check if the provided node is a hierarchy node, if yes add it to the
    MayaScene object and recurse.
*/
void
MayaHierarchyExtractor::ExtractNode(const Ptr<MayaScene>& mayaScene, const MDagPath& nodePath)
{
    MStatus status;
    MFnDagNode fnDagNode(nodePath, &status);
    n_assert(MS::kSuccess == status);

    // check if the current node is a hierarchy node
    if (MayaNodeUtil::IsExportNode(nodePath) && MayaNodeUtil::IsHierarchyNode(nodePath))
    {
        // extract name transform from the node
        MString name = MayaNodeUtil::GetNodeName(nodePath.node());
        transform44 tform = MayaTransformUtil::GetHierarchyNodeRelativeTransform(nodePath);

        // lookup parent node
        Ptr<MayaNode> parentNode;
        MDagPath parentPath;
        if (MayaNodeUtil::LookupParentHierarchyNode(nodePath, parentPath))
        {
            String parentName = MayaType::Cast<MString,String>(MayaNodeUtil::GetNodeName(parentPath.node()));
            n_assert(mayaScene->HasNode(parentName));
            parentNode = mayaScene->GetNodeByName(parentName);
        }

        // setup a new MayaNode and add it to the MayaScene
        Ptr<MayaNode> mayaNode = MayaNode::Create();
        mayaNode->Setup(MayaType::Cast<MString,StringAtom>(name), StringAtom("transform"), StringAtom(""), parentNode);
        mayaNode->AddAttr(StringAtom("position"), Variant(tform.getposition()));
        mayaNode->AddAttr(StringAtom("rotation"), Variant(MayaType::Cast<quaternion, float4>(tform.getrotate())));
        mayaNode->AddAttr(StringAtom("scale"), Variant(tform.getscale()));
        mayaNode->AddAttr(StringAtom("rotatepivot"), Variant(tform.getrotatepivot()));
        mayaNode->AddAttr(StringAtom("scalepivot"), Variant(tform.getscalepivot()));
        mayaScene->AddNode(mayaNode);
    }
    
    // recurse into children
    uint num = fnDagNode.childCount();
    uint i;
    for (i = 0; i < num; i++)
    {
        MObject child = fnDagNode.child(i);
        if (child.hasFn(MFn::kTransform))
        {
            MFnDagNode fnChildDagNode(child);
            MDagPath childDagPath;
            status = fnChildDagNode.getPath(childDagPath);
            n_assert(MS::kSuccess == status);
            this->ExtractNode(mayaScene, childDagPath);
        }
    }
}

} // namespace Maya