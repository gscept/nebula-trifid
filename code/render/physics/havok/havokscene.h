#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokScene
	
	A havok physics world.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "physics/base/basescene.h"
#include "timing/time.h"
#include "../filterset.h"

#include <Physics/Dynamics/World/hkpWorld.h>

class hkpRigidBody;
class hkpGroupFilter;

namespace Physics
{
	class PhysicsBody;
	class PhysicsObject;
	class FilterSet;
	class RayTestType;
	class Contact;
}

namespace Havok
{
class HavokScene : public Physics::BaseScene
{
	__DeclareClass(HavokScene);
public:
	/// default constructor
	HavokScene();
	/// destructor
	~HavokScene();

	/// called when the scene is attached to the physics server
	void OnActivate();
	/// called when the scene is detached from the physics server
	void OnDeactivate();

	/// set gravity vector
	void SetGravity(const Math::vector& v);

	/// perform one (or more) simulation steps depending on current time
	virtual void Trigger();
	/// render debug representation of the world
	virtual void RenderDebug();

	/// return all entities within a spherical area
	virtual int GetObjectsInSphere(const Math::vector& pos, float radius, const Physics::FilterSet& excludeSet, Util::Array<Ptr<Physics::PhysicsObject>>& result);
	/// return all entities within a box 
	virtual int GetObjectsInBox(const Math::vector& scale, const Math::matrix44& m, const Physics::FilterSet& excludeSet, Util::Array<Ptr<Physics::PhysicsObject>>& result);

	/// Do a ray check starting from position `pos' along ray `dir'.
	virtual Util::Array<Ptr<Physics::Contact>> RayCheck(const Math::vector& pos, const Math::vector& dir, const Physics::FilterSet& excludeSet, Physics::BaseScene::RayTestType rayType);
	/// do a ray check through the mouse pointer and return closest contact
	virtual Ptr<Physics::Contact> GetClosestContactUnderMouse(const Math::line& worldMouseRay, const Physics::FilterSet& excludeSet);
	/// get closest contact along ray
	virtual Ptr<Physics::Contact> GetClosestContactAlongRay(const Math::vector& pos, const Math::vector& dir, const Physics::FilterSet& excludeSet);
	/// apply an impulse along a ray into the world onto the first object which the ray hits
	virtual bool ApplyImpulseAlongRay(const Math::vector& pos, const Math::vector& dir, const Physics::FilterSet& excludeSet, float impulse);

	// ----- Havok-specific methods -----
	/// disable collision between two categories	(requires groupfilter)
	void DisableCollisionBetweenCategories(int categoryA, int categoryB);
	/// enable collision between two categories	(requires groupfilter)
	void EnableCollisionBetweenCategories(int categoryA, int categoryB);
	/// check if collision is enabled between two categories	(requires groupfilter)
	bool IsCollisionEnabledBetweenCategories(int categoryA, int categoryB);

	/// get the physics world
	hkRefPtr<hkpWorld> GetWorld();
	/// get the active collision filter
	hkRefPtr<hkpCollisionFilter> GetCollisionFilter();

protected:

	/// set up the collision filter (creates a group filter by default)
	virtual void SetupCollisionFilter();

	/// simulate the world one step, also call OnStepBefore() and OnStepAfter() on all physobjects
	virtual void StepWorld(float time);
	/// returns true if world is set up
	bool IsInitialized() const;

	/// get objects colliding with shape
	int GetObjectsInShape(hkRefPtr<hkpShape> shape, const Math::matrix44& m, const Physics::FilterSet& excludeSet, Util::Array<Ptr<Physics::PhysicsObject>>& result);

	bool initialized;
	hkRefPtr<hkpWorld> world;
	hkRefPtr<hkpCollisionFilter> filter;

	Timing::Time sinceLastTrigger, lastFrame;
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
HavokScene::IsInitialized() const
{
	return this->initialized;
}

//------------------------------------------------------------------------------
/**
*/
inline hkRefPtr<hkpWorld>
HavokScene::GetWorld()
{
	return this->world;
}

//------------------------------------------------------------------------------
/**
*/
inline hkRefPtr<hkpCollisionFilter> 
HavokScene::GetCollisionFilter()
{
	return this->filter;
}

}