//------------------------------------------------------------------------------
//  havokplayertestapplication.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokplayertestapplication.h"
#include "havokplayerteststate.h"

#include "util/commandlineargs.h"
#include "io/assignregistry.h"
#include "io/assign.h"
#include "resources/resource.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "physics/model/templates.h"
#include "physics/havok/havokcollider.h"
#include "physics/physicsbody.h"
#include "statehandlers/gamestatehandler.h"

// note: many of these includes aren't necessary
// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

// Dynamics includes
#include <Physics/Collide/hkpCollide.h>										
#include <Physics/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>	
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>				
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>		

#include <Physics/Dynamics/Collide/Filter/Constraint/hkpConstraintCollisionFilter.h>
#include <Physics/Dynamics/Constraint/Bilateral/Ragdoll/hkpRagdollConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/Prismatic/hkpPrismaticConstraintData.h>
#include <Physics/Dynamics/Constraint/Motor/Position/hkpPositionConstraintMotor.h>

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics/Dynamics/World/hkpWorld.h>								
#include <Physics/Dynamics/Entity/hkpRigidBody.h>		
#include <Physics/Dynamics/Constraint/hkpConstraintInstance.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	

#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpConstraintViewer.h>

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>	

#undef HK_FEATURE_PRODUCT_AI
#define HK_FEATURE_PRODUCT_PHYSICS
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#define HK_FEATURE_REFLECTION_PHYSICS
//#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#define HK_EXCLUDE_FEATURE_MemoryTracker
#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches
#define HK_EXCLUDE_LIBRARY_hkGeometryUtilities

using namespace Physics;
using namespace App;
using namespace Havok;
using namespace Math;
using namespace BaseGameFeature;

namespace Tests
{
__ImplementSingleton(HavokPlayerTestApplication);

//------------------------------------------------------------------------------
/**
*/
HavokPlayerTestApplication::HavokPlayerTestApplication()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
HavokPlayerTestApplication::~HavokPlayerTestApplication()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokPlayerTestApplication::Open()
{
	n_assert(!this->IsOpen());

	if (!GameApplication::Open())
	{
		return false;
	} 

	return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestApplication::Close()
{
	GameApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestApplication::SetupStateHandlers()
{
	this->havokState = HavokPlayerTestState::Create();
	this->havokState->SetSetupMode(GameStateHandler::NewGame);
	this->havokState->SetName("HavokPlayerTestState");
	this->havokState->SetLevelName("empty");

	this->AddStateHandler(this->havokState.get());

	this->SetState("HavokPlayerTestState");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestApplication::CleanupStateHandlers()
{
	GameApplication::CleanupStateHandlers();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestApplication::SetupGameFeatures()
{
	GameApplication::SetupGameFeatures();

	// create and attach default graphic features
	this->graphicsFeature = GraphicsFeature::GraphicsFeatureUnit::Create();
	this->gameServer->AttachGameFeature(this->graphicsFeature.upcast<Game::FeatureUnit>());

	// setup base game feature
	this->baseFeature = BaseGameFeatureUnit::Create();
	this->gameServer->AttachGameFeature(this->baseFeature.upcast<Game::FeatureUnit>());

	// create and attach core features
	this->physicsFeature = PhysicsFeature::PhysicsFeatureUnit::Create();    
	this->physicsFeature->SetRenderDebug(true);
	this->gameServer->AttachGameFeature(this->physicsFeature.upcast<Game::FeatureUnit>());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPlayerTestApplication::CleanupGameFeatures()
{
	GameApplication::CleanupGameFeatures();
}

}