//------------------------------------------------------------------------------
//  havokbody.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokbody.h"
#include "conversion.h"
#include "havokscene.h"
#include "havokcontactlistener.h"
#include "debugrender/debugshaperenderer.h"
#include "coregraphics/rendershape.h"

#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>
#include <Physics2012/Collide/Shape/Compound/Collection/List/hkpListShape.h>
#include <Physics2012/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

namespace Havok
{
using namespace Physics;
using namespace Math;

__ImplementClass(Havok::HavokBody,'HKBY', Physics::BaseRigidBody);

//------------------------------------------------------------------------------
/**
*/
HavokBody::HavokBody()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
HavokBody::~HavokBody()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetKinematic(bool enable)
{
    n_assert(this->rigidBody);
    if(enable)
    {
	    this->rigidBody->setMotionType(hkpMotion::MOTION_KEYFRAMED);
    }
    else
    {
        this->rigidBody->setMotionType(hkpMotion::MOTION_DYNAMIC);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetupFromTemplate(const PhysicsCommon& templ)
{
	n_assert(HK_NULL == this->rigidBody);
	n_assert(0 != templ.collider);
	
	BaseRigidBody::SetupFromTemplate(templ);

	Math::vector scale;
	templ.startTransform.get_scale(scale);

	if(scale.x() != 1 || scale.y() != 1 || scale.z() != 1)
	{
		// need scaled collider. check if supported by shape
		n_assert2(templ.collider->GetDescriptions().Size() == 1, "only singular box shapes support scale");
		n_assert2(templ.collider->GetDescriptions()[0].type == ColliderCube, "only box shapes support scale");
		Ptr<HavokCollider> newbox = HavokCollider::Create();
		newbox->SetScale(scale);
		newbox->AddFromDescription(templ.collider->GetDescriptions()[0]);
		this->common.collider = 0;
		this->common.collider = newbox.cast<Collider>();
        this->scale.scale(scale);
	}	

	// set up the compound shape
	const hkArray<hkpShape*>& shapes = this->common.collider->GetShapes();
	n_assert2(!shapes.isEmpty(), "HavokBody::SetupFromTemplate: Collider is empty!");
	hkRefPtr<hkpListShape> listShape = n_new(hkpListShape(shapes.begin(), shapes.getSize()));

	hkpRigidBodyCinfo bodyInfo;
	bodyInfo.m_shape = listShape;

	// make sure to set correct motion type if it is kinematic
	if (Physics::Kinematic & templ.bodyFlags)
	{
		bodyInfo.m_motionType = hkpMotion::MOTION_KEYFRAMED;
	}

	Math::quaternion startRotation = Math::quaternion::rotationmatrix(templ.startTransform);
	startRotation = Math::quaternion::normalize(startRotation);
	Math::vector startPosition = templ.startTransform.get_position();

	bodyInfo.m_rotation = Neb2HkQuaternion(startRotation);
	bodyInfo.m_position = Neb2HkFloat4(startPosition);
	bodyInfo.m_mass = templ.mass;
	if (-1 != templ.friction) bodyInfo.m_friction = templ.friction;
	if (-1 != templ.restitution) bodyInfo.m_restitution = templ.restitution;

	//FIXME: This can be optimized so it the same shapelist isn't computed more than once
	hkMassProperties massResult;
	this->ComputeMassProperties(bodyInfo.m_shape, templ.mass, massResult);
	bodyInfo.m_centerOfMass = massResult.m_centerOfMass;
	bodyInfo.m_inertiaTensor = massResult.m_inertiaTensor;

	bodyInfo.m_collisionFilterInfo = Physics::Default;

	this->rigidBody = n_new(hkpRigidBody(bodyInfo));

	this->rigidBody->setUserData((hkUlong)this);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::Attach(Physics::BaseScene* inWorld)
{
	n_assert(HK_NULL != this->rigidBody);

	this->world = ((HavokScene*)inWorld)->GetWorld();
	this->world->addEntity(this->rigidBody);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::Detach()
{
	n_assert(HK_NULL != this->rigidBody);
	n_assert(HK_NULL != this->world);

	// note: dependant on the world settings the bodies might be removed when they move out of broadphase
	if (this->rigidBody->isAddedToWorld())
	{
		this->world->removeEntity(this->rigidBody);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetMass(float m)
{
	n_assert(HK_NULL != this->rigidBody);

	this->rigidBody->setMass(m);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokBody::GetMass() const
{
	n_assert(HK_NULL != this->rigidBody);

	return this->rigidBody->getMass();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetTransform(const Math::matrix44& trans)
{
	hkTransform havokTransform = NebMatrix442HkTransform(trans);
	this->rigidBody->setTransform(havokTransform);
}

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44& 
HavokBody::GetTransform()
{
	hkTransform unscaledHavokTransform = this->rigidBody->getTransform();
    Math::matrix44 uns = HkTransform2NebMatrix44(this->rigidBody->getTransform());        
    this->transform = Math::matrix44::multiply(this->scale,uns);    
	return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
HavokBody::GetCenterOfMassLocal()
{
	n_assert(HK_NULL != this->rigidBody);

	return Hk2NebFloat4(this->rigidBody->getCenterOfMassLocal());
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
HavokBody::GetCenterOfMassWorld()
{
	n_assert(HK_NULL != this->rigidBody);

	return Hk2NebFloat4(this->rigidBody->getCenterOfMassInWorld());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::ComputeMassProperties(const hkpShape* shape, float mass, hkMassProperties& outResult)
{
	n_assert(HK_NULL != shape);
	hkpInertiaTensorComputer::computeShapeVolumeMassProperties(shape, mass, outResult);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetAngularFactor(const Math::vector& v)
{
	n_error("HavokBody::SetAngularFactor: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
HavokBody::GetAngularFactor() const
{
	n_error("HavokBody::GetAngularFactor: Not implemented");
	return vector();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetEnableCollisionCallback(bool enable)
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
void 
HavokBody::ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multByMass)
{
	if(this->rigidBody)
	{
		this->rigidBody->applyPointImpulse(Neb2HkFloat4(impulse),Neb2HkFloat4(pos));
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetCollideFilter(uint mask)
{
#if _DEBUG
	n_error(
#else
	n_warning(
#endif
		"HavokBody::SetCollideFilter: Can not set collide filter directly on a havok-physobject! Instead you must update the collidefilter in the scene");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokBody::SetCollideCategory(Physics::CollideCategory coll)
{
	n_assert(this->rigidBody);

	PhysicsObject::SetCollideCategory(coll);
	/* "setCollisionFilterInfo" is used as collision-category/group, the configuration of which 
	   categories that should collide with which is configured in the collision filter of the scene
	*/
	this->rigidBody->setCollisionFilterInfo(	
		//TODO: blä, fix CollideCategory
		(int)(((int)coll > 1)? n_log2((Math::scalar)coll): coll));

	if (this->world)
	{
		this->world->updateCollisionFilterOnEntity(this->rigidBody, 
			HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, 
			HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS);
	}
}

}