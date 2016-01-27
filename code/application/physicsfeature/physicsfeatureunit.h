#pragma once
//------------------------------------------------------------------------------
/**
    @class Game::PhysicsFeatureUnit
    
    The physicsfeature offers the use of collision and 
    physical representation of game objects.

    It uses the physics addon with use of a third party physics system (like ODE).

    It offers a collide property for static environment objects and a physicsproperty 
    for  passiv moving dynamic objects and an actorphysicsproperty for active dynamic moving
    objects.

    Additional there is a mousegripperproperty for grabbing and throwing
    physics object around.

    IMPORTANT: the physics server and so the physics simulation system is triggered in 
    OnFrame of the feature so

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/featureunit.h"
#include "physics/physicsserver.h"
#include "physics/physicsobject.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{

class PhysicsFeatureUnit : public Game::FeatureUnit , public Physics::CollisionReceiver   
{
    __DeclareClass(PhysicsFeatureUnit);
    __DeclareSingleton(PhysicsFeatureUnit);   

public:

    /// constructor
    PhysicsFeatureUnit();
    /// destructor
    virtual ~PhysicsFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();
               
    /// called after loading game state
    virtual void OnLoad();
    
	/// called from within GameServer::OnStart() after OnLoad when the complete world exist
	virtual void OnStart(){}      

	/// called from within GameServer::NotifySetupDefault() before the database is loaded
	virtual void OnBeforeLoad();
	/// called from within GameServer::NotifyCleanup() before shutting down a level
	virtual void OnBeforeCleanup();

	/// called on begin of frame
	virtual void OnBeginFrame(){}
	/// called in the middle of the feature trigger cycle
	virtual void OnFrame();        
	/// called at the end of the feature trigger cycle
	virtual void OnEndFrame(){}   

    /// called when game debug visualization is on
    virtual void OnRenderDebug();

	///
	virtual bool OnCollision(const Ptr<Physics::PhysicsObject> & receiver, const Ptr<Physics::PhysicsObject> & collidedWith, const Ptr<Physics::Contact> & c);

	/// returns whether the physics-vdb camera is being synced with the camera 
	bool IsSyncingVisualDebuggerCamera() const;
	/// toggle vdb-camerasync
	void ToggleVisualDebuggerCameraSync();
	/// set whether the physics-vdb camera should be synced with the camera
	void SetVisualDebuggerCameraSyncFlag(bool flag);

	/// (must be called before OnActivate!) set whether the physicsserver is supposed to initiate its visualdebugger
	void SetInitVisualDebuggerFlag(bool flag);
	/// get whether the physicsserver is supposed to initiate its visualdebugger
	bool GetInitVisualDebuggerFlag() const;

protected:	
	/// create default physics world
	void CreateDefaultPhysicsWorld();
	/// cleanup physics world
	void CleanupPhysicsWorld();

	bool syncVDBCamera;		// whether the physics-visualdebugger camera should be synced with the view camera
	bool initVisualDebugger; // whether the physicsserver is supposed to initiate its visualdebugger (defaults to true)

    Ptr<Physics::PhysicsServer> physicsServer;
    _declare_timer(PhysicsFeatureUpdatePhysics);


};

//------------------------------------------------------------------------------
/**
*/
inline bool 
PhysicsFeatureUnit::IsSyncingVisualDebuggerCamera() const
{
	return this->syncVDBCamera;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PhysicsFeatureUnit::ToggleVisualDebuggerCameraSync()
{
	this->SetVisualDebuggerCameraSyncFlag(!this->IsSyncingVisualDebuggerCamera());
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PhysicsFeatureUnit::SetVisualDebuggerCameraSyncFlag(bool flag)
{
	this->syncVDBCamera = flag;
}

}; // namespace PhysicsFeature
//------------------------------------------------------------------------------