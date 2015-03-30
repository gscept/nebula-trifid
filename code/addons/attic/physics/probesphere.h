#ifndef PHYSICS_PROBESPHERE_H
#define PHYSICS_PROBESPHERE_H
//------------------------------------------------------------------------------
/**
    @class Physics::ProbeSphere

    A sphere shape in the Physics subsystem.

    (C) 2003 RadonLabs GmbH
*/
#include "physics/sphereshape.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Physics
{
class ProbeSphere : public SphereShape
{
    __DeclareClass(ProbeSphere);

public:
    /// constructor
    ProbeSphere();
    /// destructor
    virtual ~ProbeSphere();
    /// render debug visualization
    virtual void RenderDebug(const Math::matrix44& t);
	/// perform collision test
	void Collide();
	/// had any static collisions last time ?
	bool HasStaticCollisions() const;
	/// has any collisions with game entities ? 
	bool HasDynamicCollisions() const;
	/// get collided entities
	const Util::Array<uint>& GetCollidedEntityIDs() const;
	/// set physics entity to ignore on collision
	void SetPhysicsEntityToIgnore(Physics::PhysicsEntity* ignore);
    /// get contact points of last collision
    const Util::Array<ContactPoint>& GetContactPoints() const;
    /// distanceToGround
    static float DistanceToGround(const Math::vector& pos, float upOffset, float downOffset);
    /// get maximal static collide depth
    float GetStaticCollideDepth() const;
    /// get static collide dir of deepest collide
    const Math::vector& GetStaticCollideDir() const;

private:
    /// called on collision; decide if it is valid
    virtual bool OnCollide(Shape* collidee);

	bool dynamicCollision;
	bool staticCollision;
	Util::Array<ContactPoint> contactPoints;
	Util::Array<uint> collidedEntityIDs;
	PhysicsEntity* toIgnore;
    float staticDepth;
    Math::vector staticCollideDir;
};

//------------------------------------------------------------------------------
/**
*/
inline
float
ProbeSphere::GetStaticCollideDepth() const
{
	return this->staticDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
ProbeSphere::GetStaticCollideDir() const
{
	return this->staticCollideDir;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ProbeSphere::HasStaticCollisions() const
{
	return this->staticCollision;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ProbeSphere::HasDynamicCollisions() const
{
	return this->dynamicCollision;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<uint>&
ProbeSphere::GetCollidedEntityIDs() const
{
	return this->collidedEntityIDs;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<ContactPoint>&
ProbeSphere::GetContactPoints() const
{
    return this->contactPoints;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ProbeSphere::SetPhysicsEntityToIgnore(Physics::PhysicsEntity* toIgnore)
{
	this->toIgnore = toIgnore;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
