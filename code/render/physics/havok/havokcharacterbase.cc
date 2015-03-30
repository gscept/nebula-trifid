//------------------------------------------------------------------------------
//  havokcharacter.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokcharacterbase.h"
#include "conversion.h"
#include "havokdebugdrawer.h"
#include "havokphysicsserver.h"
#include "havokvisualdebuggerserver.h"
#include "physics/scene.h"
#include "debugrender/debugrender.h"

#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>

#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterStateManager.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterState.h>
#include <Physics/Utilities/CharacterControl/StateMachine/OnGround/hkpCharacterStateOnGround.h>
#include <Physics/Utilities/CharacterControl/StateMachine/InAir/hkpCharacterStateInAir.h>
#include <Physics/Utilities/CharacterControl/StateMachine/Jumping/hkpCharacterStateJumping.h>

namespace Havok
{
using namespace Physics;
using namespace Math;

__ImplementAbstractClass(Havok::HavokCharacterBase,'HKCH', Physics::BaseCharacter);

//------------------------------------------------------------------------------
/**
*/
HavokCharacterBase::HavokCharacterBase():
	shapeDirty(false),
	defaultShape(HK_NULL),
	stateManager(HK_NULL),
	wantJump(false),
	maxSlopeAngle(PI * 0.5f * 0.75f),
	isCrouching(false),
	wantToCrouch(isCrouching)
{
	this->motionVector = vector::nullvec();
	this->forward = vector::nullvec();
	this->right= vector::nullvec();
}

//------------------------------------------------------------------------------
/**
*/
HavokCharacterBase::~HavokCharacterBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetFallSpeed(float fallSpeed)
{
	//FIXME: Fall and jump speed is specified by player gravity
	n_error("HavokCharacter::SetFallSpeed: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetMotionVector(const Math::vector& movement)
{
	this->motionVector = movement;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::RenderDebug()
{
	PhysicsObject::RenderDebug();

	if (HK_NULL != this->GetWorldObject())
	{
		// draw shape in the application
		hkQsTransform transform = NebMatrix442HkQsTransform(this->GetTransform());
		hkVector4 translation = transform.getTranslation();
		translation(1) += this->height*0.5f;
		transform.setTranslation(translation);
		HavokDebugDrawer::DrawShape(this->defaultShape, transform);
	}

	// display the state of the character
	vector position = this->GetTransform().get_position();
	position.y() += this->height + 2;
	Util::String text;
	text = GetCharacterStateAsString(this->GetCharacterState());

	_debug_text3D(text, position, Math::float4(1, 1, 1, 1));
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::OnStepBefore()
{
	this->HandleCrouching();

	// display an AABB of the character in the visual debugger
	vector position = this->GetTransform().get_position();
	vector halfExtents(this->radius, this->height*0.5f, this->radius);
	position.y() += halfExtents.y();
	HavokVisualDebuggerServer::Instance()->DrawAABB(Math::bbox(position, halfExtents), float4(0, 1, 0, 1));

	// also forward vector
	HavokVisualDebuggerServer::Instance()->DrawArrow(position, this->forward);

	// plus the current state
	Util::String text;
	text = GetCharacterStateAsString(this->GetCharacterState());
	position.y() += this->height + 2;

	HavokVisualDebuggerServer::Instance()->DrawText(position, text, float4(0, 1, 0, 1));
}


//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetupCharacterStates()
{
	hkpCharacterState* state;
	this->stateManager = n_new(hkpCharacterStateManager());

	state = n_new(hkpCharacterStateOnGround());
	this->stateManager->registerState(state,	HK_CHARACTER_ON_GROUND);
	state->removeReference();

	state = n_new(hkpCharacterStateInAir());
	this->stateManager->registerState(state,	HK_CHARACTER_IN_AIR);
	state->removeReference();

	state = n_new(hkpCharacterStateJumping());
	this->stateManager->registerState(state,	HK_CHARACTER_JUMPING);
	state->removeReference();

	this->characterContext = n_new(hkpCharacterContext(stateManager, HK_CHARACTER_ON_GROUND));
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::Jump()
{
	this->wantJump = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetJumpSpeed(float jumpSpeed)
{
	//FIXME: Fall and jump speed is specified by player gravity
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokCharacterBase::OnGround()
{
	return HK_CHARACTER_ON_GROUND == this->characterContext->getState();
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
HavokCharacterBase::GetCharacterStateAsString(hkpCharacterStateType s)
{
	Util::String state;

	switch(s)
	{
		case HK_CHARACTER_ON_GROUND:
			state = "On ground";
			break;
		case HK_CHARACTER_JUMPING:
			state = "Jumping";
			break;
		case HK_CHARACTER_IN_AIR:
			state = "In air";
			break;
		case HK_CHARACTER_CLIMBING:
			state = "Climbing";
			break;
		case HK_CHARACTER_FLYING:
			state = "Flying";
			break;
		case HK_CHARACTER_USER_STATE_0:
			state = "User state 0";
			break;
		case HK_CHARACTER_USER_STATE_1:
			state = "User state 1";
			break;
		case HK_CHARACTER_USER_STATE_2:
			state = "User state 2";
			break;
		case HK_CHARACTER_USER_STATE_3:
			state = "User state 3";
			break;
		case HK_CHARACTER_USER_STATE_4:
			state = "User state 4";
			break;
		case HK_CHARACTER_USER_STATE_5:
			state = "User state 5";
			break;
		default:
			state.Format("Unknown state enum '%d'!", (int)s);
			break;
	}

	return state;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetMaxJumpHeight(float maxJumpHeight)
{
	n_assert(HK_NULL != this->stateManager);

	hkpCharacterState* state = this->stateManager->getState(HK_CHARACTER_JUMPING);
	((hkpCharacterStateJumping*)state)->setJumpHeight(maxJumpHeight);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetMovementSpeed(float speed)
{
	n_assert(HK_NULL != this->stateManager);

	// set movement speed for both on-ground and in-air states
	hkpCharacterState* state = this->stateManager->getState(HK_CHARACTER_ON_GROUND);
	((hkpCharacterStateOnGround*)state)->setSpeed(speed);

	state = this->stateManager->getState(HK_CHARACTER_IN_AIR);
	((hkpCharacterStateInAir*)state)->setSpeed(speed);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetMaxLinearAcceleration(float acceleration)
{
	n_assert(HK_NULL != this->stateManager);

	// set acceleration for both on-ground and in-air states
	hkpCharacterState* state = this->stateManager->getState(HK_CHARACTER_ON_GROUND);
	((hkpCharacterStateOnGround*)state)->setMaxLinearAcceleration(acceleration);

	state = this->stateManager->getState(HK_CHARACTER_IN_AIR);
	((hkpCharacterStateInAir*)state)->setMaxLinearAcceleration(acceleration);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetVelocityGain(float gain, float airGain)
{
	n_assert(HK_NULL != this->stateManager);
	n_assert(gain >= 0.0f && gain <= 1.0f);
	n_assert(airGain >= 0.0f && airGain <= 1.0f);

	// set velocity gain for both on-ground and in-air states
	hkpCharacterState* state = this->stateManager->getState(HK_CHARACTER_ON_GROUND);
	((hkpCharacterStateOnGround*)state)->setGain(gain);

	state = this->stateManager->getState(HK_CHARACTER_IN_AIR);
	((hkpCharacterStateInAir*)state)->setGain(airGain);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterBase::SetTransform(const Math::matrix44& trans)
{
	// get the forward vector from the transform
	matrix44 rotationMatrix = trans;
	rotationMatrix.set_position(float4(0, 0, 0, 1));
	this->forward = float4::transform(vector(0, 0, 1), rotationMatrix);

	// remove the y component and normalize
	this->forward = float4::normalize(vector(this->forward.x(), 0, this->forward.z()));

	// also update the sideways vector
	this->right = float4::cross3(this->forward, vector::upvec());

	PhysicsObject::SetTransform(trans);
}

//------------------------------------------------------------------------------
/**
*/
void
HavokCharacterBase::InitShapes()
{
	// create the shape with origo at the bottom
	switch (this->shape)
	{
	case BaseCharacter::Capsule:
		n_assert(this->height > this->radius*2);
		n_assert(this->crouchingHeight > this->radius*2);
		this->defaultShape = n_new(hkpCapsuleShape(hkVector4(0, this->radius, 0), hkVector4(0, this->height - this->radius, 0), this->radius));
		this->crouchingShape = n_new(hkpCapsuleShape(hkVector4(0, this->radius, 0), hkVector4(0, this->crouchingHeight - this->radius, 0), this->radius));
		break;
	case BaseCharacter::Cylinder:
		this->defaultShape = n_new(hkpCylinderShape(hkVector4(0, 0, 0), hkVector4(0, this->height, 0), this->radius));
		this->crouchingShape = n_new(hkpCylinderShape(hkVector4(0, 0, 0), hkVector4(0, this->crouchingHeight, 0), this->radius));
		break;
	default:
		n_error("Shape '%d' currently not supported for a character!", (int)this->shape);
	}
}

}