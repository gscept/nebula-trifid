#ifndef PHYSICS_CONTACTPOINT_H
#define PHYSICS_CONTACTPOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::ContactPoint

    Contact points with position and Face-Up vector.

    (C) 2003 RadonLabs GmbH
*/
#include "math/vector.h"
#include "physics/materialtable.h"

//------------------------------------------------------------------------------
namespace Physics
{
class PhysicsEntity;
class RigidBody;
class Composite;

class ContactPoint
{
public:

    enum Type
    {
        StaticType,             // contactpoint between static and dynamic objects
        DynamicType,            // contactpoint between dynamic and dynamic objects

        RayCheck,               // contactpoint caused through a raycheck

        NumTypes,
        InvalidType
    };

    /// Construct.
    ContactPoint();

    /// clear content
    void Clear();
    /// set position to `v'
    void SetPosition(const Math::vector& v);
    /// get position
    const Math::vector& GetPosition() const;

    /// set up vector to `v'
    void SetUpVector(const Math::vector& v);
    /// get up vector
    const Math::vector& GetUpVector() const;

    /// get penetration depth
    float GetDepth() const;
    /// set penetration depth
    void SetDepth(float d);

    /// set optional entity id
    void SetPhysicsEntityId(uint id);
    /// get entity id
    uint GetPhysicsEntityId() const;
    /// find physics entity of contact, slow, may return 0
    PhysicsEntity* GetPhysicsEntity() const;

    /// set optional rigid body index in composite of entity
    void SetRigidBodyId(uint i);
    /// get rigid body index
    uint GetRigidBodyId() const;
    /// find rigid body of contact, slow, may return 0
    RigidBody* GetRigidBody() const;

    /// Set material of contact
    void SetMaterial(MaterialType material);
    /// get material of contact
    MaterialType GetMaterial() const;   

    /// set the tpye of the contact point
    void SetType(Type type);
    /// get the type
    const Type& GetType() const;

private:
    Math::vector position;
    Math::vector upVector;
    float depth;
    uint physicsEntityId;
    uint rigidBodyId;
    MaterialType material;
    Type type;
};

//------------------------------------------------------------------------------
/**
*/
inline
ContactPoint::ContactPoint() :
    upVector(0.0f, 1.0f, 0.0f),
    depth(0.f),
    physicsEntityId(0),
    rigidBodyId(0),
    material(InvalidMaterial),
    type(InvalidType)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::Clear()
{
    this->position.set(0.0f, 0.0f, 0.0f);
    this->upVector.set(0.0f, 1.0f, 0.0f);
    this->physicsEntityId = 0;
    this->rigidBodyId = 0;
    this->material = InvalidMaterial;
    this->type = InvalidType;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetPosition(const Math::vector& v)
{
    position = v;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Math::vector& 
ContactPoint::GetPosition() const
{
    return position;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetType(Type type)
{
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const ContactPoint::Type& 
ContactPoint::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetUpVector(const Math::vector& v)
{
    upVector = v;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Math::vector& 
ContactPoint::GetUpVector() const
{
    return upVector;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ContactPoint::GetDepth() const
{
    return this->depth;    
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetDepth(float d)
{
    this->depth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetPhysicsEntityId(uint id)
{
    this->physicsEntityId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
ContactPoint::GetPhysicsEntityId() const
{
    return this->physicsEntityId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetRigidBodyId(uint id)
{
    this->rigidBodyId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
ContactPoint::GetRigidBodyId() const
{
    return this->rigidBodyId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetMaterial(MaterialType mat)
{
    this->material = mat;
}

//------------------------------------------------------------------------------
/**
*/
inline
MaterialType
ContactPoint::GetMaterial() const
{
    return this->material;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
