#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXScene
        
	PhysX scenes (worlds)

    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "math/matrix44.h"
#include "math/vector.h"
#include "physics/physicsobject.h"
#include "physics/collider.h"
#include "physics/contact.h"
#include "timing/time.h"
#include "physics/base/basescene.h"
#include "physxprobe.h"

namespace physx
{
	class PxScene;
	class PxDefaultCpuDispatcher;
	class PxControllerManager;
}

namespace Physics
{
	class PhysicsBody;
	class FilterSet;
	class PhysicsProbe;
	class AreaImpulse;
}

namespace PhysX
{


class PhysXScene: public Physics::BaseScene
{
	__DeclareClass(PhysXScene);
public:

	 /// constructor
	PhysXScene();
	 /// destructor
	~PhysXScene();

	/// called when the scene is attached to the physics server
	void OnActivate();
	/// called when the scene is detached from the physics server
	void OnDeactivate();
	/// perform one (or more) simulation steps depending on current time
	void Trigger();

    /// Render debug shapes of colliders and other objects
	virtual void RenderDebug();

		
	/// set gravity vector
	virtual void SetGravity(const Math::vector& v);
	

    /// attach a physics object to scene
	virtual void Attach(const Ptr<Physics::PhysicsObject> & obj);
    /// attach static physics object to scene (cant be moved after, can be optimized by underlying physics engine if applicable)
    virtual void AttachStatic(const Ptr<Physics::PhysicsObject> &obj);
    /// detach physics object from scene
	virtual void Detach(const Ptr<Physics::PhysicsObject> & obj);
	
	/// return all entities within a spherical area
	virtual int GetObjectsInSphere(const Math::vector& pos, float radius, const Physics::FilterSet& excludeSet, Util::Array<Ptr<Physics::PhysicsObject> >& result) {return 0;}
	/// return all entities within a box 
	virtual int GetObjectsInBox(const Math::vector& scale, const Math::matrix44& m, const Physics::FilterSet& excludeSet, Util::Array<Ptr<Physics::PhysicsObject> >& result) {return 0;}

	/// Do a ray check starting from position `pos' along ray `dir'.
    virtual Util::Array<Ptr<Physics::Contact>> RayCheck(const Math::vector& pos, const Math::vector& dir, const Physics::FilterSet& excludeSet, RayTestType rayType);
			

	/// disable collision between two bodies
	virtual void AddIgnoreCollisionPair(const Ptr<Physics::PhysicsBody>& bodyA, const Ptr<Physics::PhysicsBody>& bodyB);
	
	class physx::PxScene * scene;
	class physx::PxDefaultCpuDispatcher* dispatcher;
	class physx::PxControllerManager * controllerManager;
private:
	Util::Array<Ptr<PhysX::PhysXProbe>> triggers;
};



}
