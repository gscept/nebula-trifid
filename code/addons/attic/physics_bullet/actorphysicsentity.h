#pragma once
//------------------------------------------------------------------------------
/**
    @class Physics::ActorPhysicsEntity

    A physics entity for a controllable actor.
*/
#include "physics/physicsentity.h"

//------------------------------------------------------------------------------
class btKinematicCharacterController;
class btPairCachingGhostObject;

namespace Physics
{

class Level;

class ActorPhysicsEntity : public Physics::PhysicsEntity
{
	__DeclareClass(ActorPhysicsEntity);
public:
    /// constructor
    ActorPhysicsEntity();
    /// destructor
    virtual ~ActorPhysicsEntity();

    /// called when attached to game entity
    virtual void OnActivate();
    /// called when attached to physics level
    virtual void OnAttachedToLevel(Level* level);
	/// invoked after a stepping the simulation
	virtual void OnMoveAfter();

	/// set the current world space transformation
	virtual void SetTransform(const Math::matrix44& m);
	/// get the current world space transformation
	virtual Math::matrix44 GetTransform() const;

	/// gets height
	float GetHeight() const { return height; }
	/// sets height
	void SetHeight(float height) { this->height = height; }
	/// gets radius
	float GetRadius() const { return radius; }
	/// sets radius
	void SetRadius(float radius) { this->radius = radius; }

	void SetFallSpeed(float fallSpeed);
	void SetMotionVector(const Math::vector& movement);
	void Jump();
	void SetJumpSpeed(float jumpSpeed);
	void SetMaxJumpHeight(float maxJumpHeight);
	void SetMaxTraversableSlopeAngle(float angle);

	bool OnGround();

protected:
	void CreateCharacterController();

	float radius;
	float height;

	btKinematicCharacterController* characterController;
	btPairCachingGhostObject* ghostObject;
};

}