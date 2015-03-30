#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokLoadAssetState
    
    Dummy state to run a havok application.
    
    (C) 2013 gscept
*/
#include "basegamefeature/statehandlers/gamestatehandler.h"


namespace Physics
{
	class PhysicsServer;
	class PhysicsBody;
	class Scene;
}

namespace Game
{
	class Entity;
}

//------------------------------------------------------------------------------
namespace Tests
{
class HavokLoadAssetState : public BaseGameFeature::GameStateHandler
{
	__DeclareClass(HavokLoadAssetState);
public:
	/// constructor
	HavokLoadAssetState();
	/// destructor
	virtual ~HavokLoadAssetState();

	/// called when the state represented by this state handler is entered
	void OnStateEnter(const Util::String& prevState);
	/// called when the state represented by this state handler is left
	void OnStateLeave(const Util::String& nextState);
	/// called each frame as long as state is current, return new state
	Util::String OnFrame();

private:

	/// initialize physics stuff
	void InitPhysicsStuff();

	Ptr<Physics::PhysicsServer> physicsServer;	//< handles the simulation
	Physics::Scene* physicsWorld;				//< the physics world

	Ptr<Game::Entity> camera;
	Ptr<Game::Entity> sphere;	// test sphere
}; 
} 
// namespace Havok
//------------------------------------------------------------------------------