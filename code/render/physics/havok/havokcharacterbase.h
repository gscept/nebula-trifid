#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokCharacter

	Base havok-character class, can be represented using a phantom or a normal 
	rigidbody. 
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/basecharacter.h"

#include <Common/Base/Types/hkRefPtr.h>
#include <Physics/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxyCinfo.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>

class hkpWorld;
class hkpCharacterContext;

namespace Physics
{
	class BaseScene;
}

namespace Havok
{
class HavokCharacterBase : public Physics::BaseCharacter
{
	__DeclareAbstractClass(HavokCharacterBase);
public:
	/// default constructor
	HavokCharacterBase();
	/// destructor
	~HavokCharacterBase();

	/// do debug rendering
	virtual void RenderDebug();
	/// called before simulation step is taken
	virtual void OnStepBefore();

	/// set transform, updates forward vector
	virtual void SetTransform(const Math::matrix44& trans);

	/// sets the height of the character, makes the shape dirty
	virtual void SetHeight(float height);
	/// sets the radius of the character, makes the shape dirty
	virtual void SetRadius(float radius);
	/// sets the shape of the character, makes the shape dirty
	virtual void SetShape(CharacterShape shape);

	/// sets movement direction
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

	/// set movement speed
	virtual void SetMovementSpeed(float speed);
	/// set linear acceleration
	virtual void SetMaxLinearAcceleration(float acceleration);
	/// set acceleration gain, between 0 and 1
	virtual void SetVelocityGain(float gain, float airGain);

	/// get world object (hkpPhantom or hkpEntity)
	virtual hkRefPtr<hkpWorldObject> GetWorldObject() = 0;

	/// get the forward direction
	Math::vector GetForwardDirection() const;
	/// get the rightwards direction
	Math::vector GetRightwardDirection() const;

	/// set crouching
	virtual void SetCrouching(bool enable);
	/// handle any changes concerning crouching
	virtual void HandleCrouching() = 0;

	// ----- Havok-specific methods ----- 

	/// return the current character state
	hkpCharacterStateType GetCharacterState() const;
	/// get character state as string
	static Util::String GetCharacterStateAsString(hkpCharacterStateType state);

protected:

	//FIXME: when looking up or down and walking diagonally, the character will move more sideways than forwards/backwards
	/// summarize the input
	virtual void GetInput(hkpCharacterInput& outInput) = 0;

	/// set up default character states
	virtual void SetupCharacterStates();

	/// initialize the stand- and crouchshape members
	void InitShapes();

	hkRefPtr<hkpWorld> world;

	bool shapeDirty;	//< if height, radius, or the shape type has been changed after setting up
	hkRefPtr<hkpShape> defaultShape, crouchingShape;
	hkRefPtr<hkpCharacterStateManager> stateManager;
	hkRefPtr<hkpCharacterContext> characterContext;	//< holds the state manager, and calculates the resulting velocity after input

	bool isCrouching, wantToCrouch;

	float maxSlopeAngle;
	Math::vector motionVector;
	Math::vector forward, right;
	bool wantJump;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokCharacterBase::SetHeight(float h)
{
	this->shapeDirty = true;
	BaseCharacter::SetHeight(h);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokCharacterBase::SetRadius(float r)
{
	this->shapeDirty = true;
	BaseCharacter::SetRadius(r);
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokCharacterBase::SetShape(CharacterShape shape)
{
	this->shapeDirty = true;
	BaseCharacter::SetShape(shape);
}

//------------------------------------------------------------------------------
/**
*/
inline hkpCharacterStateType 
HavokCharacterBase::GetCharacterState() const
{
	return this->characterContext->getState();
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokCharacterBase::SetMaxTraversableSlopeAngle(float angle)
{
	this->maxSlopeAngle = angle;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
HavokCharacterBase::GetForwardDirection() const
{
	return this->forward;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
HavokCharacterBase::GetRightwardDirection() const
{
	return this->right;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokCharacterBase::SetCrouching(bool flag)
{
	this->wantToCrouch = flag;
}

}