//------------------------------------------------------------------------------
//  havokcharacterrigidbody.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokcharacterrigidbody.h"
#include "conversion.h"
#include "havokdebugdrawer.h"
#include "havokphysicsserver.h"
#include "havokvisualdebuggerserver.h"
#include "havokcontactlistener.h"
#include "havokscene.h"
#include "debugrender/debugrender.h"

#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>

using namespace Math;

namespace Havok
{
__ImplementClass(Havok::HavokCharacterRigidBody,'HRBC', Havok::HavokCharacterBase);

//------------------------------------------------------------------------------
/**
*/
HavokCharacterRigidBody::HavokCharacterRigidBody()
{
	this->charInfo.m_mass = 100.0f;
	this->charInfo.m_friction = 0.5f;
}

//------------------------------------------------------------------------------
/**
*/
HavokCharacterRigidBody::~HavokCharacterRigidBody()
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::SetupCharacterStates()
{
	HavokCharacterBase::SetupCharacterStates();

	this->characterContext->setCharacterType(hkpCharacterContext::HK_CHARACTER_RIGIDBODY);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::OnStepBefore()
{
	HavokCharacterBase::OnStepBefore();

	// should the rigid body for whatever reason be fixed then calling set-linear/angular-velocity
	// will only result in warnings being printed and spam the console
	bool bodyIsFixed = hkpMotion::MOTION_FIXED == this->characterRigidBody->getRigidBody()->getMotionType();

	if (bodyIsFixed)
	{
		// then there's not much to do
		return;
	}

	// calculate the final velocity
	hkpCharacterInput input;
	this->GetInput(input);

	hkpCharacterOutput output;

	this->characterContext->update(input, output);

	// simulate the character one step
	const float& simulationFrameTime = HavokPhysicsServer::Instance()->GetSimulationFrameTime();

	this->characterRigidBody->setLinearVelocity(output.m_velocity, simulationFrameTime);

	// update the rotation of the rigidbody
	float yAngle = float4::angle(this->forward, vector(1, 0, 0));
	if (this->forward.z() > 0)
	{
		yAngle *= -1.0f;
	}

	hkQuaternion targetRotation;
	targetRotation.setAxisAngle(Neb2HkFloat4(vector(0, 1, 0)), yAngle);

	const hkQuaternion& currentOrient = this->characterRigidBody->getRigidBody()->getRotation();

	hkVector4 angle; 
	currentOrient.estimateAngleTo(targetRotation, angle);

	//FIXME: make configurable
	const hkReal gain = 0.1f;

	hkVector4 angularVelocity; 
	angularVelocity.setMul4(gain/simulationFrameTime, angle);
	this->characterRigidBody->setAngularVelocity(angularVelocity);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::SetTransform(const Math::matrix44& trans)
{
	if (HK_NULL != this->characterRigidBody)
	{
		// update the position
		hkVector4 targetPosition = Neb2HkFloat4(trans.get_position());
		const hkVector4& currentPosition = this->characterRigidBody->getRigidBody()->getPosition();

		// if the rigid body has been still for a duration then havok will temporarily deactivate 
		// the body so it won't spend unnecessary computations for the body - therefore avoid 
		// updating the position if it's no different from the previous position
		// (constantly updating the transform of a deactivated body will also result in the 
		// console getting spammed)
		if (!targetPosition.equals4(currentPosition))
		{
			this->characterRigidBody->getRigidBody()->setPosition(targetPosition);
		}
	}

	// update the rotation vector
	HavokCharacterBase::SetTransform(trans);
}


//------------------------------------------------------------------------------
/**
*/
const Math::matrix44& 
HavokCharacterRigidBody::GetTransform()
{
	if (HK_NULL == this->characterRigidBody)
	{
		return PhysicsObject::GetTransform();
	}

	matrix44 transform = PhysicsObject::GetTransform();

	transform.set_position(Hk2NebFloat4(this->characterRigidBody->getRigidBody()->getPosition()));

	// call the method of PhysicsObject since HavokCharacterBase::SetTransform updates forward vector
	PhysicsObject::SetTransform(transform);

	return PhysicsObject::GetTransform();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::Attach(Physics::BaseScene * w)
{
	if (this->shapeDirty || HK_NULL == this->characterRigidBody)
	{
		if (HK_NULL == this->characterRigidBody)
		{
			// this is the first time this is attached
			this->SetupCharacterStates();
		}

		this->InitShapes();

		this->charInfo.m_shape = this->defaultShape;
		this->charInfo.m_maxSlope = this->maxSlopeAngle;
		this->charInfo.m_position = Neb2HkFloat4(this->transform.get_position());

		this->charInfo.m_collisionFilterInfo = (int)n_log2(Physics::Characters); /*//TODO: log2 is used until collide categories are changed to 1, 2, 3... instead of 1, 2, 4, 8...*/

		this->characterRigidBody = n_new(hkpCharacterRigidBody(this->charInfo));
		this->characterRigidBody->getRigidBody()->setUserData((hkUlong)this);

		// set the max linear velocity and acceleration to sufficiently high levels
		// to make sure very fast moving characters won't start ignoring gravity
		this->characterRigidBody->getRigidBody()->setMaxLinearVelocity(1000);
		this->characterContext->setFilterParameters(1.0f, 1000, 1000);
	}

	this->world = ((HavokScene*)w)->GetWorld();
	this->world->addEntity(this->GetRigidBody());

	this->shapeDirty = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::Detach()
{
	if (HK_NULL != this->world)
	{
		n_assert(HK_NULL != this->characterRigidBody);

		this->world->removeEntity(this->GetRigidBody());
	}
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpRigidBody> 
HavokCharacterRigidBody::GetRigidBody()
{
	if (HK_NULL == this->characterRigidBody)
	{
		return HK_NULL;
	}

	return this->characterRigidBody->getRigidBody();
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpWorldObject> 
HavokCharacterRigidBody::GetWorldObject()
{
	return (hkpWorldObject*)this->GetRigidBody();
}
//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::GetInput(hkpCharacterInput& outInput)
{
	outInput.m_inputUD = -float4::dot3(this->motionVector, this->forward);
	outInput.m_inputLR = -float4::dot3(this->motionVector, this->right);

	outInput.m_wantJump =  this->wantJump;
	this->wantJump = false;

	outInput.m_up.set(0, 1, 0);
	// must keep a forward vector if there is no movement
	outInput.m_forward = Neb2HkFloat4(this->forward);
	outInput.m_atLadder = false;

	const float& simulationFrameTime = HavokPhysicsServer::Instance()->GetSimulationFrameTime();

	hkStepInfo stepInfo;
	stepInfo.m_deltaTime = simulationFrameTime;
	stepInfo.m_invDeltaTime = 1.0f/simulationFrameTime;

	outInput.m_stepInfo = stepInfo;

	outInput.m_characterGravity = Neb2HkFloat4(HavokPhysicsServer::Instance()->GetScene()->GetGravity());
	outInput.m_velocity = this->characterRigidBody->getLinearVelocity();
	outInput.m_position = this->characterRigidBody->getPosition();

	this->characterRigidBody->checkSupport(stepInfo, outInput.m_surfaceInfo);

	n_assert(outInput.isValid());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::SetEnableCollisionCallback(bool enable)
{
	PhysicsObject::SetEnableCollisionCallback(enable);

	if (enable)
	{
		Ptr<HavokContactListener> contactListener = HavokContactListener::Create();
		contactListener->AttachToObject(this);
	}
}

//------------------------------------------------------------------------------
/**
*/
Math::vector
HavokCharacterRigidBody::GetLinearVelocity()
{
	return Hk2NebFloat4(this->characterRigidBody->getLinearVelocity());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::SetCollideFilter(uint mask)
{
#if _DEBUG
	n_error(
#else
	n_warning(
#endif
		"HavokCharacterRigidBody::SetCollideFilter: Can not set collide filter directly on a havok-physobject! Instead you must update the collidefilter in the scene");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::SetCollideCategory(Physics::CollideCategory coll)
{
	n_assert(this->characterRigidBody);

	PhysicsObject::SetCollideCategory(coll);
	this->characterRigidBody->getRigidBody()->setCollisionFilterInfo(
		//TODO: blä, fix CollideCategory
		(int)(((int)coll > 1)? n_log2((Math::scalar)coll): coll));

	if (this->world)
	{
		this->world->updateCollisionFilterOnEntity(this->characterRigidBody->getRigidBody(), 
			HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, 
			HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterRigidBody::HandleCrouching()
{
	if (this->isCrouching == this->wantToCrouch)
	{
		return;
	}

	n_assert(this->defaultShape);
	n_assert(this->crouchingShape);
	n_assert(this->characterRigidBody);

	if (!this->isCrouching && this->wantToCrouch)
	{
		this->characterRigidBody->getRigidBody()->setShape(this->crouchingShape);
		this->isCrouching = true;
	}
	else if (this->isCrouching && !this->wantToCrouch)
	{
		this->characterRigidBody->getRigidBody()->setShape(this->defaultShape);
		this->isCrouching = false;
	}
}

}
