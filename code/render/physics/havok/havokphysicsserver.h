#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokPhysicsServer
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/physicsserverbase.h"

namespace Physics
{
	class Scene;
	class ContactListener;
}

namespace Havok
{
class HavokPhysicsServer : public Physics::BasePhysicsServer
{
	__DeclareClass(HavokPhysicsServer);
	__DeclareSingleton(HavokPhysicsServer);
public:
	/// default constructor
	HavokPhysicsServer();
	/// destructor
	~HavokPhysicsServer();

	/// initialize the physics subsystem
	virtual bool Open();
	/// close the physics subsystem
	virtual void Close();
	/// perform simulation step(s)
	virtual void Trigger();
	/// render a debug visualization of the level
	virtual void RenderDebug();
	
	/// set the current physics level object
	void SetScene(Physics::Scene* level);

	/// (havok-specific) register a contact listener to be used in HandleCollisions()
	void RegisterContactListener(const Ptr<Physics::ContactListener>& listener);

	/// get all contact listeners that are attached to the object
	Util::Array<Ptr<Physics::ContactListener>> GetObjectContactListeners(const Ptr<Physics::PhysicsObject>& physObj);
	/// get the first found listener attached to the object
	Ptr<Physics::ContactListener> GetObjectContactListener(const Ptr<Physics::PhysicsObject>& physObj);

	/// handle collisions
	virtual void HandleCollisions();

private:

	Util::Array<Ptr<Physics::ContactListener>> contactListeners;
};

}