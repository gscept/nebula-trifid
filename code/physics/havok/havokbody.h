#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokBody
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/basebody.h"
#include "conversion.h"

#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>
#include <Common/GeometryUtilities/Inertia/hkInertiaTensorComputer.h>
#include <Physics2012/Collide/Agent/Collidable/hkpCollidableQualityType.h>


namespace Physics
{
	class PhysicsCommon;
	class BaseScene;
}

class hkpWorld;
class hkpRigidBodyInfo;

namespace Havok
{
class HavokBody : public Physics::BaseRigidBody
{
	__DeclareClass(HavokBody);
public:
	/// default constructor
	HavokBody();
	/// destructor
	~HavokBody();
	/// set mass of the body
	void SetMass(float m);
	/// get the mass of the body
	float GetMass() const;
	/// get the center of mass in local space
	Math::vector GetCenterOfMassLocal();
	/// get the center of mass in world space
	Math::vector GetCenterOfMassWorld();

	/// set wether this object is kinematic
	virtual void SetKinematic(bool);

	/// set transform
	void SetTransform(const Math::matrix44& trans);
	/// get transform
	const Math::matrix44& GetTransform();

	/// set the body's linear velocity
	void SetLinearVelocity(const Math::vector& v);
	/// get the body's linear velocity
	Math::vector GetLinearVelocity() const;
	/// set the body's angular velocity
	void SetAngularVelocity(const Math::vector& v);
	/// get the body's angular velocity
	Math::vector GetAngularVelocity() const;

	/// set the angular factor on the rigid body
	void SetAngularFactor(const Math::vector& v);
	/// get the angular factor on the rigid body
	Math::vector GetAngularFactor() const;

	/// if enabling collision callback then this creates and attaches a contact listener to this object
	void SetEnableCollisionCallback(bool enable);

    /// deactivate body
    void SetSleeping(bool sleeping);
    /// return if body is non active
    bool GetSleeping();

	/// apply a global impulse vector at the next time step at a global position
	void ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multByMass = false);

	/// set collide category
	virtual void SetCollideCategory(Physics::CollideCategory coll);
	/// will deliberately throw an error! You must instead update the filter in the havok scene
	virtual void SetCollideFilter(uint mask);

	// ----- Havok-specific methods ----- 

	/// get the rigidbody pointer
	hkRefPtr<hkpRigidBody> GetRigidBody();
	/// get collidable quality type
	hkpCollidableQualityType GetCollidableQualityType() const;
	/// set collidable quality type
	void SetCollidableQualityType(hkpCollidableQualityType qualityType);

protected:   
	friend class Physics::PhysicsObject;

	/// attach to scene
	void Attach(Physics::BaseScene * world);
	/// detach from scene
	void Detach();
	
	/// set up body from template
	virtual void SetupFromTemplate(const Physics::PhysicsCommon& templ);

	/// compute mass properties
	static void ComputeMassProperties(const hkpShape* shape, float mass, hkMassProperties& outResult);

	hkRefPtr<hkpRigidBody> rigidBody;
	hkRefPtr<hkpWorld> world;
    Math::matrix44 scale;
};

//------------------------------------------------------------------------------
/**
*/
inline hkRefPtr<hkpRigidBody>
HavokBody::GetRigidBody()
{
	return this->rigidBody;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokBody::SetLinearVelocity(const Math::vector& v)
{
	n_assert(HK_NULL != this->rigidBody);
	this->rigidBody->setLinearVelocity(Neb2HkFloat4(v));
}

//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
HavokBody::GetLinearVelocity() const
{
	n_assert(HK_NULL != this->rigidBody);
	return Hk2NebFloat4(this->rigidBody->getLinearVelocity());
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokBody::SetAngularVelocity(const Math::vector& v)
{
	n_assert(HK_NULL != this->rigidBody);
	this->rigidBody->setAngularVelocity(Neb2HkFloat4(v));
}

//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
HavokBody::GetAngularVelocity() const
{
	n_assert(HK_NULL != this->rigidBody);
	return Hk2NebFloat4(this->rigidBody->getAngularVelocity());
}

//------------------------------------------------------------------------------
/**
*/
inline hkpCollidableQualityType 
HavokBody::GetCollidableQualityType() const
{
	n_assert(HK_NULL != this->rigidBody);

	return this->rigidBody->getQualityType();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokBody::SetCollidableQualityType(hkpCollidableQualityType qualityType)
{
	n_assert(HK_NULL != this->rigidBody);

	this->rigidBody->setQualityType(qualityType);
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
HavokBody::GetSleeping()
{
    n_assert(HK_NULL != this->rigidBody);
    return !this->rigidBody->isActive();    
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokBody::SetSleeping(bool sleeping)
{
    n_assert(HK_NULL != this->rigidBody);
    if(sleeping)
    {
        this->rigidBody->requestDeactivation();
    }
    else
    {
        this->rigidBody->activate();
    }
}

}