//------------------------------------------------------------------------------
//  mayatransformutil.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayatransformutil.h"
#include "mayautil/mayanodeutil.h"
#include "mayautil/mayatype.h"
#include <maya/MFnTransform.h>
#include <maya/MQuaternion.h>

namespace Maya
{
using namespace Math;

//------------------------------------------------------------------------------
/**
    This gets the inclusive matrix of the dag path split
    into its components (position, rotation, scale, rotatePivot, scalePivot).
    The dag path must be a transform node.
*/
transform44
MayaTransformUtil::GetTransform(const MDagPath& dagPath)
{
    MStatus status;
    
    // get inclusive matrix
    MMatrix inclMatrix = dagPath.inclusiveMatrix(&status);
    n_assert(MS::kSuccess == status);
    
    // get rotate and scale pivot from transform node
    MFnTransform fnTransform(dagPath, &status);
    n_assert(MS::kSuccess == status);
    MPoint rotatePivot = fnTransform.rotatePivot(MSpace::kTransform, &status);
    n_assert(MS::kSuccess == status);
    MPoint scalePivot = fnTransform.scalePivot(MSpace::kTransform, &status);
    n_assert(MS::kSuccess == status);

    // split matrix into components
    transform44 tform = MayaTransformUtil::GetTransformComponents(inclMatrix, rotatePivot, scalePivot);
    return tform;
}

//------------------------------------------------------------------------------
/**
*/
transform44
MayaTransformUtil::GetTransformComponents(const MMatrix& m, const MPoint& rotatePivot, const MPoint& scalePivot)
{
    MStatus status;

    // split matrix into components
    MTransformationMatrix mayaTransform(m);
    MVector translate = mayaTransform.getTranslation(MSpace::kTransform, &status) * MayaTransformUtil::LinearUnitScale();
    n_assert(MS::kSuccess == status);
    MQuaternion rotate = mayaTransform.rotation();
    double scale[3];
    status = mayaTransform.getScale(scale, MSpace::kTransform);
    n_assert(MS::kSuccess == status);

    // cast to Nebula3 datatypes
    point n3Translate(float(translate.x), float(translate.y), float(translate.z));
    quaternion n3Rotate(MayaType::Cast<MQuaternion, quaternion>(rotate));
    vector n3Scale((float)scale[0], (float)scale[1], (float)scale[2]);
    point n3RotatePivot(MayaType::Cast<MPoint, point>(rotatePivot));
    point n3ScalePivot(MayaType::Cast<MPoint, point>(scalePivot));

    // fill Nebula transform object
    transform44 tform;
    tform.setposition(n3Translate);
    tform.setrotate(n3Rotate);
    tform.setscale(n3Scale);
    tform.setrotatepivot(n3RotatePivot);
    tform.setscalepivot(n3ScalePivot);
    
    return tform;
}

//------------------------------------------------------------------------------
/**
    Returns the relative transform to the next hierarchy node in the
    transform hierarchy. The provided dag path must point to 
    a transform node, not a shape node!
*/
transform44
MayaTransformUtil::GetHierarchyNodeRelativeTransform(const MDagPath& dagPath)
{
    MStatus status;
    float linearUnitScale = MayaTransformUtil::LinearUnitScale();

    // must be a transform node! (not a shape node etc...)
    MFnTransform fnTransform(dagPath, &status);
    n_assert(MS::kSuccess == status);

    // get transform of dag path itself
    MMatrix inclMatrix = dagPath.inclusiveMatrix(&status);
    n_assert(MS::kSuccess == status);

    // find the first hierarchy node towards the root
    MDagPath parentHierarchyNode;
    if (MayaNodeUtil::LookupParentHierarchyNode(dagPath, parentHierarchyNode))
    {
        // get inclusive, inverted matrix from hierarchy node parent
        MMatrix parentInclMatrixInv = parentHierarchyNode.inclusiveMatrixInverse(&status);

        // compute parent-relative matrix
        inclMatrix *= parentInclMatrixInv;
    }

    // get rotate and scale pivot from transform node
    MPoint rotatePivot = fnTransform.rotatePivot(MSpace::kTransform, &status) * linearUnitScale;
    n_assert(MS::kSuccess == status);
    MPoint scalePivot = fnTransform.scalePivot(MSpace::kTransform, &status) * linearUnitScale;
    n_assert(MS::kSuccess == status);

    // split the matrix into its components
    transform44 tform = MayaTransformUtil::GetTransformComponents(inclMatrix, rotatePivot, scalePivot);

    return tform;
}

//------------------------------------------------------------------------------
/**
*/
float
MayaTransformUtil::LinearUnitScale()
{
    // FIXME: get unit setting from Maya!
    return 0.01f;
}

} // namespace Maya
