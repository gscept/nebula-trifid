//------------------------------------------------------------------------------
//  physics/ray.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/ray.h"
#include "physics/shape.h"
#include "physics/rigidbody.h"
#include "physics/physicsserver.h"
#include "physics/level.h"
#include "physics/meshshape.h"
#include "physics/composite.h"

namespace Physics
{
__ImplementClass(Physics::Ray, 'PRAY',  Core::RefCounted);

using namespace Math;

Util::Array<ContactPoint>* Ray::contactsArray = 0;

//------------------------------------------------------------------------------
/**
*/
Ray::Ray() : 
    vec(0.0f, 1.0f, 0.0f)
{
    // create an ODE ray
    this->odeRayId = dCreateRay(0, 1.0f);
}

//------------------------------------------------------------------------------
/**
*/
Ray::~Ray()
{
    n_assert(0 == contactsArray);
    dGeomDestroy(this->odeRayId);
}

//------------------------------------------------------------------------------
/**
    The near callback for the ray check.
*/
void
Ray::OdeRayCallback(void* data, dGeomID o1, dGeomID o2)
{
    n_assert(data != 0);
    n_assert(o1 != o2);
    Ray* self = (Ray*) data;
    n_assert(Ray::contactsArray);
    n_assert(self->IsInstanceOf(Ray::RTTI));

    // handle sub-space
    if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
    {
        dSpaceCollide2(o1, o2, data, &OdeRayCallback);
        return;
    }

    // check for exclusion
    Shape* otherShape = 0;
    if (o1 == self->GetGeomId())
    {
        otherShape = Shape::GetShapeFromGeom(o2);
    }
    else if (o2 == self->GetGeomId())
    {
        otherShape = Shape::GetShapeFromGeom(o1);
    }

    const FilterSet excludeSet = self->GetExcludeFilterSet();
    if (excludeSet.CheckShape(otherShape))
    {
        return;
    }

    dContactGeom contact[MaxContacts];
    int numColls = 0;
    if (o1 == self->GetGeomId())
    {
        numColls = dCollide(o2, o1, MaxContacts, &(contact[0]), sizeof(dContactGeom));
    }
    else
    {
        numColls = dCollide(o1, o2, MaxContacts, &(contact[0]), sizeof(dContactGeom));
    }
    static Math::vector contactPos;
    static Math::vector contactNormal;
    ContactPoint contactPoint;
    int i;
    for (i = 0; i < numColls; i++)
    {
        // FIXME: hmm, contact[x].geom.pos[] doesn't seem to be correct with mesh
        // shapes which are not at the origin. Computing the intersection pos from
        // the stabbing depth and the ray's original vector
        contactPos = self->GetOrigin() + self->GetNormVector() * contact[i].depth;
        contactNormal.set(contact[i].normal[0], contact[i].normal[1], contact[i].normal[2]);
        contactPoint.SetPosition(contactPos);
        contactPoint.SetUpVector(contactNormal);
        contactPoint.SetDepth(contact[i].depth);
        contactPoint.SetType(ContactPoint::RayCheck);
        PhysicsEntity* otherEntity = otherShape->GetEntity();
        if (otherEntity)
        {
            contactPoint.SetPhysicsEntityId(otherEntity->GetUniqueId());
        }
        else
        {
            contactPoint.SetPhysicsEntityId(0);
        }
        RigidBody* otherRigidBody = otherShape->GetRigidBody();
        if (otherRigidBody)
        {
            contactPoint.SetRigidBodyId(otherRigidBody->GetUniqueId());
        }
        else
        {
            contactPoint.SetRigidBodyId(0);
        }
        contactPoint.SetMaterial(otherShape->GetMaterialType());
        Ray::contactsArray->Append(contactPoint);
    }
}

//------------------------------------------------------------------------------
/**
    Do a complete ray check, and append(!) all contacts to the provided
    Math::vector array. Returns number of intersections encountered.

    - 24-Nov-03     floh    dGeomRaySet() was expecting a normalized direction
*/
int
Ray::DoRayCheckAllContacts(const Math::matrix44& transform, Util::Array<ContactPoint>& contacts)
{
    int origNumContacts = contacts.Size();

    // transform position and ray
    Math::vector globOrig = matrix44::transform(this->orig, transform);
    Math::vector globVec  = (matrix44::transform(this->orig + this->vec, transform)) - globOrig;
    float rayLen = globVec.length();
    globVec = vector::normalize(globVec);

    // update ODE ray properties
    dGeomRaySet(this->odeRayId, globOrig.x(), globOrig.y(), globOrig.z(), globVec.x(), globVec.y(), globVec.z());
    dGeomRaySetLength(this->odeRayId, rayLen);

    // do collision detection
    dSpaceID odeSpaceId = PhysicsServer::Instance()->GetOdeCommonSpaceId();
    Ray::contactsArray = &contacts;
    dSpaceCollide2((dGeomID) odeSpaceId, this->odeRayId, this, &OdeRayCallback);
    Ray::contactsArray = 0;
    return (contacts.Size() - origNumContacts);
}

} // namespace Physics