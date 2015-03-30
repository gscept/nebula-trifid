//------------------------------------------------------------------------------
//  game/PhysicsFeatureUnit.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsfeatureunit.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "game/gameserver.h"
#include "basegametiming/systemtimesource.h"
#include "physics/level.h"
#include "physics/meshcache.h"
#include "io/ioserver.h"

// include all properties for known by managers::factorymanager
#include "properties/physicsproperty.h"
#include "properties/actorphysicsproperty.h"
#include "properties/mousegripperproperty.h"
#include "properties/environmentcollideproperty.h"
#include "properties/triggerproperty.h"

namespace PhysicsFeature
{
__ImplementClass(PhysicsFeatureUnit, 'FPHY' , Game::FeatureUnit);
__ImplementSingleton(PhysicsFeatureUnit);

using namespace Physics;
using namespace Game;

//------------------------------------------------------------------------------
/**
*/
PhysicsFeatureUnit::PhysicsFeatureUnit()
{
    __ConstructSingleton;
    _setup_timer(PhysicsFeatureUpdatePhysics);
}

//------------------------------------------------------------------------------
/**
*/
PhysicsFeatureUnit::~PhysicsFeatureUnit()
{
    _discard_timer(PhysicsFeatureUpdatePhysics);
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnActivate()
{
    FeatureUnit::OnActivate();

    IO::AssignRegistry::Instance()->SetAssign(IO::Assign("physics", "export:physics"));

    this->physicsServer = PhysicsServer::Create();
    this->physicsServer->Open();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnDeactivate()
{
    this->physicsServer->Close();
    this->physicsServer = 0;

    FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnLoad()
{
    Physics::MeshCache::Instance()->Open();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnFrame()
{    
    _start_timer(PhysicsFeatureUpdatePhysics);

    // update time in physics system
    Timing::Time time = BaseGameFeature::SystemTimeSource::Instance()->GetTime();
    this->physicsServer->SetTime(time);
    // trigger physics system, steps physics world etc.
    this->physicsServer->Trigger();

    _stop_timer(PhysicsFeatureUpdatePhysics);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::OnRenderDebug()
{
    Physics::PhysicsServer::Instance()->RenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::CreateDefaultPhysicsWorld()
{
    Physics::MeshCache::Instance()->Open();
    Ptr<Level> physicsLevel = Level::Create();
    PhysicsServer::Instance()->SetLevel(physicsLevel);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsFeatureUnit::CleanupPhysicsWorld()
{
    PhysicsServer::Instance()->SetLevel(0);
    Physics::MeshCache::Instance()->Close();
}

}; // namespace Game
