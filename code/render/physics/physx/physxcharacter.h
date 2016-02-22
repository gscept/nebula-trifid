#pragma once
//------------------------------------------------------------------------------
/**
	@class PhysX::PhysXCharacter

    A physics entity for a controllable actor.

    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "physics/base/basecharacter.h"

namespace physx
{
	class PxController;
}

//------------------------------------------------------------------------------
namespace PhysX
{
class PhysXCharacter : public Physics::BaseCharacter
{
	__DeclareClass(PhysXCharacter);
public:

    /// constructor
	PhysXCharacter();
    /// destructor
    virtual ~PhysXCharacter();
	
	/// set the height of the character
	virtual void SetHeight(float height);
    /// Sets crouching height
    virtual void SetCrouchingHeight(float height);	
	/// set the radius of the character
	virtual void SetRadius(float radius);
	
	// set the shape of the character
	virtual void SetShape(CharacterShape shape);

	/// set movement direction
	virtual void SetMotionVector(const Math::vector& movement);
	/// sets biggest angle allowed for traversal
	virtual void SetMaxTraversableSlopeAngle(float angle);
	/// sets fall speed
	virtual void SetFallSpeed(float fallSpeed);
	/// jumps character
	virtual void Jump();
	/// sets speed of jumping
	virtual void SetJumpSpeed(float jumpSpeed);
	/// sets maximal jumping height
	virtual void SetMaxJumpHeight(float maxJumpHeight);
	/// returns true if character is on the ground
	virtual bool OnGround();
	/// returns linear velocity
	virtual Math::vector GetLinearVelocity();

    /// set crouching
    virtual void SetCrouching(bool enable);

	/// set movement speed
	virtual void SetMovementSpeed(float speed);
	/// set linear acceleration
	virtual void SetMaxLinearAcceleration(float acceleration);
	/// set velocity gain, between 0 and 1
	virtual void SetVelocityGain(float gain, float airGain);

protected:
	/// called when character gets attached
	void Attach(Physics::BaseScene* world);
	/// called when character gets detached
	void Detach();

	physx::PxController* controller;
	float jumpHeight;
};
}