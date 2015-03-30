//------------------------------------------------------------------------------
//  mayalodnodeextractor.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaextractors/mayalodnodeextractor.h"
#include "mayautil/mayanodeutil.h"
#include "mayautil/mayatype.h"
#include "mayautil/mayaattrutil.h"
#include "mayautil/mayatransformutil.h"
#include <maya/MDagPathArray.h>
#include <maya/MFnDagNode.h>

namespace Maya
{
__ImplementClass(Maya::MayaLodNodeExtractor, 'MLOX', Maya::MayaExtractorBase);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
MayaStatus::Code
MayaLodNodeExtractor::Extract(const Ptr<MayaScene>& mayaScene)
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
    Check if the provided node is a LOD node and recurse. If it is 
    an LOD node, extract LOD information and add to existing MayaScene node.
*/
void
MayaLodNodeExtractor::ExtractNode(const Ptr<MayaScene>& mayaScene, const MDagPath& nodePath)
{
    MStatus status;
    MFnDagNode fnDagNode(nodePath, &status);
    n_assert(MS::kSuccess == status);

    // check if the current node is a valid LOD node
    MObject node = nodePath.node();
    if (MayaNodeUtil::IsExportNode(nodePath) && 
        MayaNodeUtil::IsHierarchyNode(nodePath) &&
        node.hasFn(MFn::kLodGroup))
    {
        // get the name of the node
        StringAtom name = MayaType::Cast<MString,StringAtom>(MayaNodeUtil::GetNodeName(node));
        if (mayaScene->HasNode(name))
        {
            // if minMaxDistance is not set, use default values for
            // min and max visible distance
            float minDist = 0.0f;
            float maxDist = 10000.0f;
            if (MayaAttrUtil::GetBool(node, "minMaxDistance"))
            {
                minDist = MayaAttrUtil::GetFloat(node, "minDistance");
                maxDist = MayaAttrUtil::GetFloat(node, "maxDistance");
            }

            // convert to meters
            Array<float> threshold = MayaAttrUtil::GetFloatArray(node, "threshold");
            float linearUnitScale = MayaTransformUtil::LinearUnitScale();
            IndexT i;
            for (i = 0; i < threshold.Size(); i++)
            {
                threshold[i] *= linearUnitScale;
            }

            // update the MayaScene with the extracted LOD information
            const Ptr<MayaNode>& mayaNode = mayaScene->GetNodeByName(name);
            mayaNode->SetSubType(StringAtom("LodNode"));
            mayaNode->AddAttr(StringAtom("lodmindist"), Variant(minDist));
            mayaNode->AddAttr(StringAtom("lodmaxdist"), Variant(maxDist));
            mayaNode->AddAttr(StringAtom("loddistances"), Variant(threshold));
        }
    }

    // recurse into children
    uint i;
    uint num = fnDagNode.childCount();
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