//------------------------------------------------------------------------------
//  physics/shape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/shape.h"
#include "physics/rigidbody.h"
#include "physics/filterset.h"
#include "physics/level.h"

namespace Physics
{
__ImplementClass(Physics::Shape, 'SHAP', Core::RefCounted);

Util::Array<ContactPoint>* Shape::CollideContacts = 0;
const FilterSet* Shape::CollideFilterSet = 0;

//------------------------------------------------------------------------------
/**
*/
Shape::Shape() :
    type(InvalidShapeType),
    rigidBody(0),
    entity(0),
    categoryBits(Dynamic),
    collideBits((uint)All),
    odeGeomId(0),
    odeSpaceId(0),
    transform(Math::matrix44::identity())
{
    materialType = MaterialTable::StringToMaterialType("Soil");
    dMassSetZero(&(this->odeMass));
}

//------------------------------------------------------------------------------
/**
*/
Shape::Shape(Type t) :
    type(t),
    rigidBody(0),
    entity(0),
    categoryBits(Dynamic),
    collideBits((uint)All),
    odeGeomId(0),
    odeSpaceId(0)
{
    materialType = MaterialTable::StringToMaterialType("Metal");
    dMassSetZero(&(this->odeMass));
}

//------------------------------------------------------------------------------
/**
*/
Shape::~Shape()
{
    if (this->IsAttached())
    {
        this->Detach();
    }
}

//------------------------------------------------------------------------------
/**
    This attaches the shape to a new collide space. This method is more
    lightweight then Attach() (in fact, it's called by Attach(). This method
    together with RemoveFromSpace() can be used to move the shape
    quickly between collide spaces.
*/
void
Shape::AttachToSpace(dSpaceID spaceId)
{
    n_assert(0 == this->odeSpaceId);
    n_assert(0 != this->odeGeomId);
    this->odeSpaceId = spaceId;
    dSpaceAdd(spaceId, this->odeGeomId);
}

//------------------------------------------------------------------------------
/**
    This removes the shape from its current collide space, but leaves
    everything intact.
*/
void
Shape::RemoveFromSpace()
{
    n_assert(0 != this->odeSpaceId);
    n_assert(0 != this->odeGeomId);
    dSpaceRemove(this->odeSpaceId, this->odeGeomId);
    this->odeSpaceId = 0;
}

//------------------------------------------------------------------------------
/**
    The Open() method is usually overwritten in a subclass to create
    a shape of a specific type.
*/
bool
Shape::Attach(dSpaceID spaceId)
{
    n_assert(!this->IsAttached());
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Shape::Detach()
{
    n_assert(this->IsAttached());
    n_assert(0 != this->odeGeomId);
    dGeomDestroy(this->odeGeomId);
    this->odeGeomId = 0;
    this->odeSpaceId = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
Shape::OnCollide(Shape* shape)
{
    n_assert(shape);
    if (this->entity && (shape->entity != this->entity))
    {
        // let the entity decide if the collision is valid
        return this->entity->OnCollide(shape);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Universal method for all specific ODE geom types, which add the
    geom to the collide space, using an ODE proxy geom to offset the
    geom by the provided transformation matrix. The geom will also
    be attached to the rigid body, if any is set.
*/
void
Shape::AttachGeom(dGeomID geomId, dSpaceID spaceId)
{
    n_assert(0 != geomId);
    n_assert(!this->IsAttached());

    // extract position and rotation
    dVector3 odePos;
    dMatrix3 odeRot;
    PhysicsServer::Vector3ToOde(this->transform.get_position(), odePos);
    PhysicsServer::Matrix44ToOde(this->transform, odeRot);

    // set the geom's local transform
    dGeomSetPosition(geomId, odePos[0], odePos[1], odePos[2]);
    dGeomSetRotation(geomId, odeRot);

    // if attached to rigid body, create a geom transform "proxy" object
    if (0 != this->rigidBody)
    {
        this->odeGeomId = dCreateGeomTransform(0);
        dGeomTransformSetCleanup(this->odeGeomId, 1);
        dGeomTransformSetGeom(this->odeGeomId, geomId);

        // attach the proxy to the rigid body
        dGeomSetBody(this->odeGeomId, this->rigidBody->GetOdeBodyId());
    }
    else
    {
        // not attached to rigid body, directly set transform and rotation
        this->odeGeomId = geomId;
    }

    // set collide category bits
    dGeomSetCategoryBits(this->odeGeomId, this->categoryBits);
    dGeomSetCollideBits(this->odeGeomId, this->collideBits);

    // set user data pointer of proxy to point to ourselvs
    dGeomSetData(this->odeGeomId, this);

    // add geom to space
    this->AttachToSpace(spaceId);
}

//------------------------------------------------------------------------------
/**
    Transform the own dMass structure by the own transform matrix.
*/
void
Shape::TransformMass()
{
    dVector3 odePos;
    dMatrix3 odeRot;
    PhysicsServer::Vector3ToOde(this->transform.get_position(), odePos);
    PhysicsServer::Matrix44ToOde(this->transform, odeRot);
    dMassRotate(&(this->odeMass), odeRot);
    dMassTranslate(&(this->odeMass), odePos[0], odePos[1], odePos[2]);
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of this shape. Override this method
    in a subclass.

    @param  t   transform matrix of rigid body this shape is attached to
*/
void
Shape::RenderDebug(const Math::matrix44& t)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Shape::SetTransform(const Math::matrix44& m)
{
    this->transform = m;

    // if not attached to rigid body, directly update shape's transformation
    if ((0 == this->rigidBody) && (this->odeGeomId != 0))
    {
        dVector3 odePos;
        dMatrix3 odeRot;
        PhysicsServer::Vector3ToOde(this->transform.get_position(), odePos);
        PhysicsServer::Matrix44ToOde(this->transform, odeRot);
        dGeomSetPosition(this->odeGeomId, odePos[0], odePos[1], odePos[2]);
        dGeomSetRotation(this->odeGeomId, odeRot);
    }
}

//------------------------------------------------------------------------------
/**
    Returns the debug visualization color which is used in RenderDebug().
    This depends on the state of the rigid body which owns this state (if any):
    yellow:     no rigid body attached to shape
    green:   rigid body is enabled
    blue:    ridig body is disabled
*/
Math::float4
Shape::GetDebugVisualizationColor() const
{
    RigidBody* rigidBody = this->GetRigidBody();
    if (rigidBody)
    {
        if (rigidBody->IsEnabled())
        {
            return Math::float4(0.0f, 1.0f, 0.0f, 0.35f);
        }
        else
        {
            return Math::float4(0.0f, 0.0f, 1.0f, 0.35f);
        }
    }
    else
    {
        IndexT index;
        for(index = 0; index < this->contactPoints.Size(); index++)
        {
            if(this->contactPoints[index].GetType() == ContactPoint::DynamicType)
            {
                return Math::float4(1.0f, 0.0f, 0.0f, 0.3f);
            }
        }
        return Math::float4(1.0f, 1.0f, 0.0f, 0.3f);
    }
}

//------------------------------------------------------------------------------
/**
    The ODENearCallback for Shape::Collide.

    31-May-05   floh    invert contact normal if necessary, the contact normal
                        in ODE always points into shape1, however, we want
                        the contact normal always to point away from the
                        "other" object
*/
void
Shape::OdeNearCallback(void* data, dGeomID o1, dGeomID o2)
{
    Shape* self = (Shape*) data;

    // handle sub-space
    if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
    {
        // collide geom with sub-space
        dSpaceCollide2(o1, o2, data, &OdeNearCallback);
        return;
    }

    // check for exclusion
    Shape* shape1 = Shape::GetShapeFromGeom(o1);
    Shape* shape2 = Shape::GetShapeFromGeom(o2);
    Shape* otherShape = 0;
    if (shape1 == self)
    {
        otherShape = shape2;
    }
    else if (shape2 == self)
    {
        otherShape = shape1;
    }
    n_assert(0 != otherShape);

    if (Shape::CollideFilterSet->CheckShape(otherShape))
    {
        return;
    }

    // do collision detection, only check for one contact
    dContactGeom odeContact;
    int numColls = 0;
    if (shape1 == self)
    {
        numColls = dCollide(o1, o2, 1, &(odeContact), sizeof(dContactGeom));
    }
    else
    {
        numColls = dCollide(o2, o1, 1, &(odeContact), sizeof(dContactGeom));
    }
    ContactPoint contactPoint;
    static Math::vector contactPos;
    static Math::vector contactNormal;
    if (numColls > 0)
    {
        contactPos.set(odeContact.pos[0], odeContact.pos[1], odeContact.pos[2]);
        contactNormal.set(odeContact.normal[0], odeContact.normal[1], odeContact.normal[2]);
        contactPoint.SetPosition(contactPos);
        contactPoint.SetUpVector(contactNormal);
        contactPoint.SetMaterial(otherShape->GetMaterialType());
        contactPoint.SetDepth(odeContact.depth);        
        
        // set type depending on spaces
        const dSpaceID s1 = dGeomGetSpace(o1);
        const dSpaceID s2 = dGeomGetSpace(o2);            
        if (s1 == s2 && s2 == PhysicsServer::Instance()->GetLevel()->GetOdeDynamicSpaceId())
        {
            contactPoint.SetType(ContactPoint::DynamicType);
        }
        else if (s1 != s2)
        {
            contactPoint.SetType(ContactPoint::StaticType);
        }
        else
        {
            n_error("Collision between two static objects detected! Should NEVER happen!!!");
        }


        PhysicsEntity* entity = otherShape->GetEntity();
        if (entity)
        {
            contactPoint.SetPhysicsEntityId(entity->GetUniqueId());            
        }
        RigidBody* rigidBody = otherShape->GetRigidBody();
        if (rigidBody)
        {
            contactPoint.SetRigidBodyId(rigidBody->GetUniqueId());
        }
        Shape::CollideContacts->Append(contactPoint);
    }
}

//------------------------------------------------------------------------------
/**
    Collides this shape against the rest of the world and return all found
    contacts. Note this method will not return exact contacts, since there
    will only be one contact point returned per collided geom. Contact positions
    will be set to the midpoint of the geom, normals will be computed so that
    they point away from the Shape's midpoint.
*/
void
Shape::Collide(const FilterSet& filterSet, Util::Array<ContactPoint>& contacts)
{
    contacts.Reset();
    dSpaceID odeSpaceId = PhysicsServer::Instance()->GetOdeCommonSpaceId();
    Shape::CollideContacts = &contacts;
    Shape::CollideFilterSet = &filterSet;
    dSpaceCollide2((dGeomID) odeSpaceId, this->odeGeomId, this, &OdeNearCallback);
}

} // namespace Physics