//------------------------------------------------------------------------------
//  havokcharacterphantom.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokcharacterphantom.h"
#include "conversion.h"
#include "havokdebugdrawer.h"
#include "havokphysicsserver.h"
#include "havokvisualdebuggerserver.h"
#include "physics/scene.h"
#include "debugrender/debugrender.h"

#include <Physics2012/Dynamics/Phantom/hkpSimpleShapePhantom.h>
#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include "conversion.h"

using namespace Math;

namespace Havok
{
__ImplementClass(Havok::HavokCharacterPhantom,'HKCP', Havok::HavokCharacterBase);

//------------------------------------------------------------------------------
/**
*/
HavokCharacterPhantom::HavokCharacterPhantom():
	phantom(HK_NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
HavokCharacterPhantom::~HavokCharacterPhantom()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::Attach(Physics::BaseScene* w)
{
	if (this->shapeDirty || HK_NULL == this->phantom)
	{
		this->InitShapes();

		if (HK_NULL == this->phantom)
		{
			// this is the first time this is attached
			this->SetupCharacterStates();
			this->phantom = n_new(hkpSimpleShapePhantom(this->defaultShape, hkTransform::getIdentity(), /*FIXME*/(int)n_log2(Physics::Characters)));
			this->phantom->setUserData((hkUlong)this);
		}
		else
		{
			this->phantom->setShape(this->defaultShape);
		}
	}

	this->world = ((HavokScene*)w)->GetWorld();
	this->world->addPhantom(this->phantom);

	this->charInfo.m_maxSlope = this->maxSlopeAngle;
	this->charInfo.m_shapePhantom = this->phantom;
	this->charInfo.m_position = Neb2HkFloat4(this->transform.get_position());
	this->characterProxy = n_new(hkpCharacterProxy(this->charInfo));

	this->shapeDirty = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::Detach()
{
	if (HK_NULL != this->world)
	{
		n_assert(HK_NULL != this->phantom);

		this->world->removePhantom(this->phantom);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::SetTransform(const Math::matrix44& trans)
{
	if (HK_NULL != this->characterProxy)
	{
		this->characterProxy->setPosition(Neb2HkFloat4(trans.get_position()));
	}

	HavokCharacterBase::SetTransform(trans);
}

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44& 
HavokCharacterPhantom::GetTransform()
{
	if (HK_NULL == this->characterProxy)
	{
		return PhysicsObject::GetTransform();
	}

	matrix44 transform = PhysicsObject::GetTransform();

	transform.set_position(Hk2NebFloat4(this->characterProxy->getPosition()));

	// call the method of PhysicsObject since HavokCharacterBase::SetTransform updates forward vector
	PhysicsObject::SetTransform(transform);

	return PhysicsObject::GetTransform();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::OnStepBefore()
{
	HavokCharacterBase::OnStepBefore();

	// calculate the final velocity
	hkpCharacterInput input;
	this->GetInput(input);

	hkpCharacterOutput output;
	this->characterContext->update(input, output);

	this->characterProxy->setLinearVelocity(output.m_velocity);

	// simulate the character one step
	const float& simulationFrameTime = HavokPhysicsServer::Instance()->GetSimulationFrameTime();

	hkStepInfo stepInfo;
	stepInfo.m_deltaTime = simulationFrameTime;
	stepInfo.m_invDeltaTime = 1.0f/simulationFrameTime;
	this->characterProxy->integrate(stepInfo, Neb2HkFloat4(HavokPhysicsServer::Instance()->GetScene()->GetGravity()));
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::GetInput(hkpCharacterInput& outInput)
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
	outInput.m_stepInfo.m_deltaTime = simulationFrameTime;
	outInput.m_stepInfo.m_invDeltaTime = 1.0f / simulationFrameTime;
	outInput.m_characterGravity = Neb2HkFloat4(HavokPhysicsServer::Instance()->GetScene()->GetGravity());
	outInput.m_velocity = this->characterProxy->getLinearVelocity();
	outInput.m_position = this->characterProxy->getPosition();

	hkVector4 down;	
	down.setNeg4(outInput.m_up);
	this->characterProxy->checkSupport(down, outInput.m_surfaceInfo);

	n_assert(outInput.isValid());
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpWorldObject> 
HavokCharacterPhantom::GetWorldObject()
{
	return (hkpWorldObject*)this->phantom;
}
//------------------------------------------------------------------------------
/**
*/
Math::vector
HavokCharacterPhantom::GetLinearVelocity()
{
	return Hk2NebFloat4(this->characterProxy->getLinearVelocity());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::SetCollideFilter(uint mask)
{
#if _DEBUG
	n_error(
#else
	n_warning(
#endif
		"HavokCharacterPhantom::SetCollideFilter: Can not set collide filter directly on a havok-physobject! Instead you must update the collidefilter in the scene");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::SetCollideCategory(Physics::CollideCategory coll)
{
	n_assert(this->phantom);

	PhysicsObject::SetCollideCategory(coll);
	n_error("FIXME: Cannot set the collide category directly to the phantom, only when it is created! Must remember the collidecategory-param and recreate the phantom");

	//this->phantom->getCollidable()->setCollisionFilterInfo(
	//	//TODO: blä, fix CollideCategory
	//	(int)(((int)coll > 1)? n_log2((Math::scalar)coll): coll));

	if (this->world)
	{
		this->world->updateCollisionFilterOnPhantom(this->phantom, 
			HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokCharacterPhantom::HandleCrouching()
{
	n_error("HavokCharacterPhantom::HandleCrouching: Not implemented");
}

}