#ifndef PHYSICS_SHAPE_H
#define PHYSICS_SHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::Shape

    Shapes are used in the physics subsystem for collision detection.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "physics/materialtable.h"
#include "physics/collidecategory.h"
#include "ode/ode.h"
#include "physics/contactpoint.h"

//------------------------------------------------------------------------------
namespace Physics
{
class RigidBody;
class PhysicsEntity;
class FilterSet;

class Shape : public Core::RefCounted
{
    __DeclareClass(Shape);
public:
    /// shape types
    enum Type
    {
        InvalidShapeType = 0,
        Box,
        Sphere,
        Plane,
        Mesh,
        Capsule,

        NumShapeTypes,
    };

    /// default constructor
    Shape();
    /// destructor
    virtual ~Shape();
    /// render debug visualization
    virtual void RenderDebug(const Math::matrix44& t);
    /// collision notifier 
    virtual bool OnCollide(Shape* shape);
    /// attach the shape to the world
    virtual bool Attach(dSpaceID spaceId);
    /// detach the shape from the world
    virtual void Detach();
    /// is shape attached to world?
    bool IsAttached() const;
    /// set the shape type
    void SetType(Type t);
    /// get the shape type
    Type GetType() const;
    /// set the shape's transform
    void SetTransform(const Math::matrix44& m);
    /// get the shape's transform
    const Math::matrix44& GetTransform() const;
    /// set the shape's material type
    void SetMaterialType(MaterialType t);
    /// get the shape's material type
    MaterialType GetMaterialType() const;
    /// set the shape's collide category bits (of type Physics::CollideCategory)
    void SetCategoryBits(uint cat);
    /// get the shape's collide category
    uint GetCategoryBits() const;
    /// set the shape's collide bits (contains a category bit for each category to check against)
    void SetCollideBits(uint bits);
    /// get collide bits
    uint GetCollideBits() const;
    /// set optional rigid body object this shape is attached to
    void SetRigidBody(RigidBody* body);
    /// get optional rigid body object
    RigidBody* GetRigidBody() const;
    /// set optional physics entity pointer
    void SetEntity(PhysicsEntity* e);
    /// get optional entity pointer
    PhysicsEntity* GetEntity() const;
    /// generic collide check of shape against rest of world
    void Collide(const FilterSet& excludeSet, Util::Array<ContactPoint>& contacts);
    /// get the ODE geom id
    dGeomID GetGeomId() const;
    /// get the space id we're currently attached to
    dSpaceID GetSpaceId() const;
    /// attach the shape to a collide space, this is more lightweight then Attach()
    void AttachToSpace(dSpaceID spaceId);
    /// remove the shape from its current collide space
    void RemoveFromSpace();
    /// set contact points
    void SetContactPoints(const Util::Array<Physics::ContactPoint>& contacts);
    /// get contact points
    const Util::Array<Physics::ContactPoint>&  GetContactPoints() const;
    /// clear contact points
    void ClearContactPoints();

protected:
    friend class RigidBody;
    friend class Level;
    friend class Ray;

    /// constructor
    Shape(Type t);
    /// add geom shape to collide space using a proxy geom
    void AttachGeom(dGeomID geomId, dSpaceID spaceId);
    /// transform ODE own mass structure by own 4x4 matrix
    void TransformMass();
    /// get initialized ODE mass structure (after Attach()!)
    const dMass& GetOdeMass() const;
    /// get shape pointer from ODE geometry
    static Shape* GetShapeFromGeom(dGeomID geom);
    /// return debug visualization color (depends on state of rigid body)
    Math::float4 GetDebugVisualizationColor() const;
    /// ode collision callback
    static void OdeNearCallback(void* data, dGeomID o1, dGeomID o2);

    enum
    {
        MaxContacts = 64,
    };

    static Util::Array<ContactPoint>* CollideContacts;
    static const FilterSet* CollideFilterSet;

    Type type;
    Math::matrix44 transform;
    MaterialType materialType;
    RigidBody* rigidBody;
    PhysicsEntity* entity;
    uint categoryBits;
    uint collideBits;
    dGeomID odeGeomId;      // the proxy geom id
    dMass odeMass;          // the mass structure
    dSpaceID odeSpaceId;    // the collide space we're currently attached to
    Util::Array<Physics::ContactPoint> contactPoints;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetEntity(PhysicsEntity* e)
{
    this->entity = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
PhysicsEntity*
Shape::GetEntity() const
{
    return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
const dMass&
Shape::GetOdeMass() const
{
    return this->odeMass;
}

//------------------------------------------------------------------------------
/**
*/
inline
dGeomID
Shape::GetGeomId() const
{
    return this->odeGeomId;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Shape::IsAttached() const
{
    return (0 != this->odeGeomId);
}

//------------------------------------------------------------------------------
/**
*/
inline
dSpaceID
Shape::GetSpaceId() const
{
    return this->odeSpaceId;
}

//------------------------------------------------------------------------------
/**
*/
inline
Shape::Type
Shape::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44&
Shape::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetMaterialType(MaterialType t)
{
    this->materialType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
MaterialType
Shape::GetMaterialType() const
{
    return this->materialType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetCategoryBits(uint cat)
{
    this->categoryBits = cat;
    if (0 != this->odeGeomId)
    {
        dGeomSetCategoryBits(this->odeGeomId, this->categoryBits);
    }        
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
Shape::GetCategoryBits() const
{
    return this->categoryBits;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetCollideBits(uint bits)
{
    this->collideBits = bits;
    if (0 != this->odeGeomId)
    {
        dGeomSetCollideBits(this->odeGeomId, this->collideBits);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
Shape::GetCollideBits() const
{
    return this->collideBits;
}

//------------------------------------------------------------------------------
/**
*/
inline
RigidBody*
Shape::GetRigidBody() const
{
    return this->rigidBody;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Shape::SetRigidBody(RigidBody* body)
{
    n_assert(!this->IsAttached());
    this->rigidBody = body;
}

//------------------------------------------------------------------------------
/**
*/
inline
Shape*
Shape::GetShapeFromGeom(dGeomID geom)
{
    return (Shape*) dGeomGetData(geom);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Shape::SetContactPoints(const Util::Array<Physics::ContactPoint>& contacts)
{
    this->contactPoints =  contacts;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Physics::ContactPoint>&  
Shape::GetContactPoints() const
{
    return this->contactPoints;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Shape::ClearContactPoints()
{
    this->contactPoints.Clear();
}
}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    