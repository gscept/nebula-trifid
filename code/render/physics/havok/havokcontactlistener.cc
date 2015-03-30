//------------------------------------------------------------------------------
//  havokcontactlistener.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/contactlistener.h"
#include "conversion.h"
#include "../physicsbody.h"
#include "havokbody.h"
#include "havokcharacterrigidbody.h"
#include "havokstatic.h"
#include "havokphysicsserver.h"
#include "havokutil.h"

#include "Physics/Dynamics/Collide/ContactListener/hkpContactPointEvent.h"

namespace Havok
{
uint HavokContactListener::HavokCollisionData::idCounter = 0;

using namespace Physics;

__ImplementClass(Havok::HavokContactListener,'HKCL', Physics::BaseContactListener);

//------------------------------------------------------------------------------
/**
*/
HavokContactListener::HavokContactListener()
{
	this->internalContactListener = HK_NULL;
}

//------------------------------------------------------------------------------
/**
*/
HavokContactListener::~HavokContactListener()
{
	this->internalContactListener = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::AttachToObject(const Ptr<Physics::PhysicsObject>& object)
{
	BaseContactListener::AttachToObject(object);

	// attaching this to several bodies might mess things up
	n_assert(HK_NULL == this->internalContactListener);

	// retrieve the havok-rigidbody from the object
	n_assert(HavokUtil::HasHavokRigidBody(object));

	hkRefPtr<hkpRigidBody> rigidBody = HavokUtil::GetHavokRigidBody(object);

	this->internalContactListener = n_new(InternalHavokContactListener());
	rigidBody->addContactListener(this->internalContactListener);
	this->internalContactListener->parent = this;

	const Ptr<HavokContactListener>& listener = this;
	Havok::HavokPhysicsServer::Instance()->RegisterContactListener(listener.cast<Physics::ContactListener>());
}

//------------------------------------------------------------------------------
/**
*/
PhysicsObject* 
HavokContactListener::GetOtherObject(PhysicsObject* obj1, PhysicsObject* obj2)
{
	if (obj1 == this->owner.get())
	{
		return obj2;
	}
	else
	{
		n_assert(obj2 == this->owner.get());
		return obj1;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::OnCollisionAdded(const Ptr<Physics::PhysicsObject>& object)
{
	const PhysicsObject::Id& id = object->GetUniqueId();
	
	n_assert(!this->collisions.Contains(id));

	this->collisions.Add(id, n_new(HavokCollisionData(this->owner, object)));

	if (this->expiredCollisions.Contains(id))
	{
		CollisionData* collision = this->expiredCollisions[id];
		n_delete(collision);
		this->expiredCollisions.Erase(id);
	}

}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::OnCollisionRemoved(const Ptr<Physics::PhysicsObject>& object)
{
	const PhysicsObject::Id& id = object->GetUniqueId();
	n_assert(this->collisions.Contains(id));
	n_assert(!this->expiredCollisions.Contains(id));

	HavokCollisionData* collision = static_cast<HavokCollisionData*>(this->collisions[id]);
	this->collisions.Erase(id);

	if (collision->hasUnhandledInfo)
	{
		// collision is being unregistered before it had a chance to be processed, store it to make sure it gets processed
		this->expiredCollisions.Add(id, collision);
	}
	else
	{
		n_delete(collision);
	}
}


//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::OnNewContactPoint(const Ptr<Physics::PhysicsObject>& object, const Math::vector& point, const Math::vector& normal, float distance, float separatingVelocity, const hkpContactPointEvent::Type& eventType)
{
	BaseContactListener::OnNewContactPoint(object, point, normal, distance);

	if (!this->collisions.Contains(object->GetUniqueId()))
	{
		//FIXME: see BaseContactListener::OnNewContactPoint
		return;
	}

	HavokCollisionData* collision = static_cast<HavokCollisionData*>(this->collisions[object->GetUniqueId()]);
	collision->separatingVelocity = separatingVelocity;
	// depending on the event type the separating velocity will be different 
	collision->contactEventType = eventType;

	collision->hasUnhandledInfo = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::Clear()
{
	BaseContactListener::Clear();

	this->ClearExpiredCollisions();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::ClearExpiredCollisions()
{
	IndexT i;
	for (i = 0; i < this->expiredCollisions.Size(); i++)
	{
		CollisionData* collisionData = this->expiredCollisions.ValueAtIndex(i);
		collisionData->Clear();
		n_delete(collisionData);
	}

	this->expiredCollisions.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::InternalHavokContactListener::collisionAddedCallback(const hkpCollisionEvent& event)
{
	PhysicsObject* physObject1 = (PhysicsObject*)event.m_bodies[0]->getUserData();	
	PhysicsObject* physObject2 = (PhysicsObject*)event.m_bodies[1]->getUserData();	

	// the order in which the objects are given are not consistent, get the one which is not the owner of this listener
	PhysicsObject* otherObject = this->parent->GetOtherObject(physObject1, physObject2);

	this->parent->OnCollisionAdded(otherObject);

	registerForEndOfStepContactPointCallbacks(event);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::InternalHavokContactListener::collisionRemovedCallback(const hkpCollisionEvent& event)
{
	PhysicsObject* physObject1 = (PhysicsObject*)event.m_bodies[0]->getUserData();	
	PhysicsObject* physObject2 = (PhysicsObject*)event.m_bodies[1]->getUserData();	

	// the order in which the objects are given are not consistent, get the one which is not the owner of this listener
	PhysicsObject* otherObject = this->parent->GetOtherObject(physObject1, physObject2);

	this->parent->OnCollisionRemoved(otherObject);

	unregisterForEndOfStepContactPointCallbacks(event);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokContactListener::InternalHavokContactListener::contactPointCallback(const hkpContactPointEvent& e)
{
	PhysicsObject* physObject1 = (PhysicsObject*)e.m_bodies[0]->getUserData();	
	PhysicsObject* physObject2 = (PhysicsObject*)e.m_bodies[1]->getUserData();

	float separatingVelocity = e.getSeparatingVelocity();

	// the order in which the objects are given is not consistent, get the one which is not the owner of this listener
	PhysicsObject* otherObject = this->parent->GetOtherObject(physObject1, physObject2);

	this->parent->OnNewContactPoint(otherObject, Hk2NebFloat4(e.m_contactPoint->getPosition()), Hk2NebFloat4(e.m_contactPoint->getNormal()), e.m_contactPoint->getDistance(), separatingVelocity, e.m_type);
}

}