#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokContactListener
    
    Havok-specific contact listener, these are used for collision feedback (in
	comparison to checking all contacts that happen in the world in the bullet
	implementation, that is not even possible in havok)
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "physics/base/basecontactlistener.h"

#include <Common/Base/hkBase.h>
#include <Common/Base/Object/hkReferencedObject.h>
#include <Physics2012/Dynamics/Collide/ContactListener/hkpContactListener.h>
//------------------------------------------------------------------------------

namespace Physics
{
	class PhysicsObject;
}

namespace Havok
{
class HavokContactListener : public Physics::BaseContactListener
{
	__DeclareClass(HavokContactListener);
public:

	struct HavokCollisionData : public BaseContactListener::CollisionData
	{
		/// default constructor
		HavokCollisionData():
			separatingVelocity(0),
			hasUnhandledInfo(false),
			collisionId(idCounter++)
		{ /* empty */ }

		/// constructor
		HavokCollisionData(Physics::PhysicsObject* obj1, Physics::PhysicsObject* obj2):
			CollisionData(obj1, obj2),
			separatingVelocity(0),
			hasUnhandledInfo(false),
			collisionId(idCounter++)
		{ /* empty */ }

		float separatingVelocity;
		hkpContactPointEvent::Type contactEventType;
		
		// keep an id to distinguish collisions, since the same collision can be reported several times if new data is added (see HavokPhysicsServer::HandleCollisions)
		const uint collisionId; 
		bool hasUnhandledInfo;

	private:
		static uint idCounter /*= 0*/;
	};

	/// constructor
	HavokContactListener();
	/// destructor
	virtual ~HavokContactListener();

	/// attach the listener to a physicsbody, and register it with the physicsserver
	void AttachToObject(const Ptr<Physics::PhysicsObject>& object);

	/// get the number of expired collisions
	SizeT GetNumExpiredCollisions();
	/// get expired collision data at index
	CollisionData* GetExpiredCollisionDataAtIndex(IndexT i) const;
	/// clear all expired collisions
	void ClearExpiredCollisions();

	/// traverse current collisions and release object references
	virtual void Clear();

protected:
	friend class InternalHavokContactListener;

	/// make a real havok contactlistener
	class InternalHavokContactListener : public hkReferencedObject, public hkpContactListener
	{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

		/// constructor
		InternalHavokContactListener() { /*empty*/ };

		// note: do not make this a Ptr<>, it creates a reference loop and results in memleaks
		HavokContactListener* parent;

		void collisionAddedCallback(const hkpCollisionEvent& event);
		void collisionRemovedCallback(const hkpCollisionEvent& event);

		void contactPointCallback(const hkpContactPointEvent& e);
	};

	/// a new collision has started with another physics object
	virtual void OnCollisionAdded(const Ptr<Physics::PhysicsObject>& object);
	/// a collision has ended
	virtual void OnCollisionRemoved(const Ptr<Physics::PhysicsObject>& object);
	/// when there's a new contact during a collision. this also adds separating velocity compared to the base class
	void OnNewContactPoint(const Ptr<Physics::PhysicsObject>& object, const Math::vector& point, const Math::vector& normal, float distance, float separatingVelocity, const hkpContactPointEvent::Type& eventType);

	/// utility method, returns the object which is not the owner (asserts that the other actually is the owner)
	Physics::PhysicsObject* GetOtherObject(Physics::PhysicsObject* obj1, Physics::PhysicsObject* obj2);

	hkRefPtr<InternalHavokContactListener> internalContactListener;
	Util::Dictionary<Physics::PhysicsObject::Id, CollisionData*> expiredCollisions;	//< keep collision data when a collision is removed until checked by the physserver or a new collision with the same object occurs (otherwise collisions between two very fast-moving objects will not be handled)
}; 


//------------------------------------------------------------------------------
/**
*/
inline SizeT 
HavokContactListener::GetNumExpiredCollisions()
{
	return this->expiredCollisions.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline Physics::BaseContactListener::CollisionData* 
HavokContactListener::GetExpiredCollisionDataAtIndex(IndexT i) const
{
	n_assert(this->expiredCollisions.Size() > i);

	return this->expiredCollisions.ValueAtIndex(i);
}


} 
// namespace Havok
//------------------------------------------------------------------------------