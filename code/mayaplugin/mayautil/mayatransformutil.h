#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaTransformUtil
    
    Get transformation from Maya nodes as Nebula3 data types.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include "math/matrix44.h"
#include "math/transform44.h"
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaTransformUtil
{
public:
    /// get transform components from dag path (pos, rot, scale, rotPivot, scalePivot)
    static Math::transform44 GetTransform(const MDagPath& dagPath);
    /// get transform relative to the next hierarchy node toward the root
    static Math::transform44 GetHierarchyNodeRelativeTransform(const MDagPath& dagPath);
    /// split a Maya matrix into its transform components (pos, rotate, scale)
    static Math::transform44 GetTransformComponents(const MMatrix& m, const MPoint& rotatePivot, const MPoint& scalePivot);
    /// get the scale factor for linear units
    static float LinearUnitScale();
};

} // namespace Maya
//------------------------------------------------------------------------------
    