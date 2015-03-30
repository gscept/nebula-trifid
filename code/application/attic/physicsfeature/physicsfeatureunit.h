#ifndef PHYSICSFEATURE_PHYSICSFEATUREUNIT_H
#define PHYSICSFEATURE_PHYSICSFEATUREUNIT_H
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
*/
#include "game/featureunit.h"
#include "physics/physicsserver.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{

class PhysicsFeatureUnit : public Game::FeatureUnit    
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
    
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();      

    /// called when game debug visualization is on
    virtual void OnRenderDebug();

    /// create default physics world
    void CreateDefaultPhysicsWorld();
    /// cleanup physics world
    void CleanupPhysicsWorld();

protected:
    Ptr<Physics::PhysicsServer> physicsServer;
    _declare_timer(PhysicsFeatureUpdatePhysics);
};

}; // namespace PhysicsFeature
//------------------------------------------------------------------------------
#endif