//------------------------------------------------------------------------------
//  physxbody.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/physx/physxbody.h"
#include "physics/collider.h"
#include "PxRigidDynamic.h"
#include "physxutils.h"
#include "PxPhysics.h"
#include "extensions/PxRigidBodyExt.h"
#include "physxphysicsserver.h"

using namespace Physics;
using namespace physx;

namespace PhysX
{
__ImplementClass(PhysX::PhysXBody, 'PXRB', Physics::BaseRigidBody);


//------------------------------------------------------------------------------
/**
*/
PhysXBody::PhysXBody()
{
	
}

//------------------------------------------------------------------------------
/**
*/
PhysXBody::~PhysXBody()
{

}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXBody::SetupFromTemplate(const PhysicsCommon & templ)
{
	BaseRigidBody::SetupFromTemplate(templ);
	Math::quaternion startRotation = Math::quaternion::rotationmatrix(templ.startTransform);
	startRotation = Math::quaternion::normalize(startRotation);
	Math::vector startPosition = templ.startTransform.get_position();
	PxTransform pxStartTrans(Neb2PxVec(startPosition), Neb2PxQuat(startRotation));
	this->body = PhysXServer::Instance()->physics->createRigidDynamic(pxStartTrans);
	Ptr<PhysXCollider> coll = templ.collider.cast<PhysXCollider>();
	for (int i = 0;i < coll->geometry.Size();i++)
	{
		
	}
	
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::RenderDebug()
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXBody::SetLinearVelocity(const Math::vector& v)
{
	this->body->setLinearVelocity(Neb2PxVec(v));
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
PhysXBody::GetLinearVelocity() const
{		
	return Px2NebVec(this->body->getLinearVelocity());	
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXBody::SetAngularVelocity(const Math::vector& v)
{
	this->body->setAngularVelocity(Neb2PxVec(v));
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
PhysXBody::GetAngularVelocity() const
{
	return Px2NebVec(this->body->getAngularVelocity());
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
PhysXBody::GetCenterOfMassLocal()
{
	return Px2NebVec(this->body->getCMassLocalPose().p);	
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
PhysXBody::GetCenterOfMassWorld()
{
	n_error("BaseRigidBody::GetCenterOfMassWorld: Not implemented");
	return -1;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::Attach(Physics::BaseScene * world)
{
	this->scene = ((PhysXScene*)world)->scene;
	this->scene->addActor(*this->body);
	this->attached = true;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::Detach()
{
	n_assert(this->attached);
	this->scene->removeActor(*this->body);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::SetMass(float m)
{
	PxRigidBodyExt::setMassAndUpdateInertia(*this->body, m);	
}

//------------------------------------------------------------------------------
/**
*/
float
PhysXBody::GetMass() const
{
	return this->body->getMass();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::SetEnableGravity(bool enable)
{
	this->body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !enable);
}

//------------------------------------------------------------------------------
/**
*/
bool
PhysXBody::GetEnableGravity() const
{
	return !this->body->getActorFlags().isSet(PxActorFlag::eDISABLE_GRAVITY);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::SetKinematic(bool enable)
{
	this->body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, enable);
}

//------------------------------------------------------------------------------
/**
*/
bool
PhysXBody::GetKinematic()
{
	return this->body->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::ApplyImpulseAtPos(const Math::vector& impulse, const Math::point& pos, bool multByMass /*= false*/)
{
	PxRigidBodyExt::addForceAtPos(*this->body, Neb2PxVec(impulse), Neb2PxVec(pos), PxForceMode::eIMPULSE);	
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::SetAngularDamping(float f)
{
	this->body->setAngularDamping(f);
}

//------------------------------------------------------------------------------
/**
*/
float
PhysXBody::GetAngularDamping() const
{
	return this->body->getAngularDamping();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXBody::SetLinearDamping(float f)
{
	this->body->setLinearDamping(f);
}

//------------------------------------------------------------------------------
/**
*/
float
PhysXBody::GetLinearDamping() const
{
	return this->body->getLinearDamping();
}

} // namespace PhysX