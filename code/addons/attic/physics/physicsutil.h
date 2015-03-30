#ifndef PHYSICS_PHYSICSUTIL_H
#define PHYSICS_PHYSICSUTIL_H
//------------------------------------------------------------------------------
/**
    @class Physics::PhysicsUtil

    Implements some static physics utility methods.
    
    (C) 2004 RadonLabs GmbH
*/
#include "core/types.h"
#include "math/vector.h"
#include "physics/filterset.h"

//------------------------------------------------------------------------------
namespace Physics
{
class ContactPoint;

class PhysicsUtil
{
public:
    /// do a normal stabbing test and return closest contact
    static bool RayCheck(const Math::vector& from, const Math::vector& to, const FilterSet& excludeSet, ContactPoint& outContact);
    /// do a stabbing test into the world with a ray bundle, return distance to intersection
    static bool RayBundleCheck(const Math::vector& from, const Math::vector& to, const Math::vector& upVec, const Math::vector& leftVec, float bundleRadius, const FilterSet& excludeSet, float& outContactDist);
};

}; // namespace Physics

//------------------------------------------------------------------------------
#endif
