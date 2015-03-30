#ifndef PHYSICS_EXPLOSIONAREAIMPULSE_H
#define PHYSICS_EXPLOSIONAREAIMPULSE_H
//------------------------------------------------------------------------------
/**
    @class Physics::ExplosionAreaImpulse

    Implements an area impulse for a typical explosion. Applies an impulse
    with exponentail falloff to all rigid bodies within the range of the
    explosion witch satisfy a line-of-sight test. After Apply() is called,
    the object can be asked about all physics entities which have been
    affected.
    
    (C) 2005 Radon Labs GmbH
*/
#include "physics/areaimpulse.h"
#include "util/array.h"
#include "physics/physicsentity.h"

//------------------------------------------------------------------------------
namespace Physics
{
class ContactPoint;
class RigidBody;

class ExplosionAreaImpulse : public AreaImpulse
{
	__DeclareClass(ExplosionAreaImpulse);
public:
    /// constructor
    ExplosionAreaImpulse();
    /// destructor
    virtual ~ExplosionAreaImpulse();
    /// apply impulse to the world
    void Apply();
    /// set position
    void SetPosition(const Math::vector& p);
    /// get position
    const Math::vector& GetPosition() const;
    /// set radius
    void SetRadius(float r);
    /// get radius
    float GetRadius() const;
    /// set max impulse
    void SetImpulse(float i);
    /// get max impulse
    float GetImpulse() const;

private:
    /// apply impulse on single rigid body
    bool HandleRigidBody(RigidBody* rigidBody, const Math::vector& pos);

    static Util::Array<ContactPoint> CollideContacts;
    Math::vector pos;
    float radius;
    float impulse;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
ExplosionAreaImpulse::SetPosition(const Math::vector& p)
{
    this->pos = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
ExplosionAreaImpulse::GetPosition() const
{
    return this->pos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ExplosionAreaImpulse::SetRadius(float r)
{
    this->radius = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ExplosionAreaImpulse::GetRadius() const
{
    return this->radius;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ExplosionAreaImpulse::SetImpulse(float i)
{
    this->impulse = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ExplosionAreaImpulse::GetImpulse() const
{
    return this->impulse;
}

};
//------------------------------------------------------------------------------
#endif
