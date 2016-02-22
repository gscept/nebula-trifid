//------------------------------------------------------------------------------
//  physxphysicsserver.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "physics/physicsserver.h"
#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "extensions/PxDefaultAllocator.h"
#include "physxprofilesdk/PxProfileZoneManager.h"
#include "common/PxTolerancesScale.h"

#define PVD_HOST "127.0.0.1"

using namespace physx;

static PhysX::NebulaAllocatorCallback gDefaultAllocatorCallback;

class NebulaErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file,
		int line)
	{
		n_error("PhysXError: type: %d, %s, \nfile: %s\nline: %d\n", code, message, file, line);
	}
};

static NebulaErrorCallback gDefaultErrorCallback;

namespace PhysX
{
__ImplementAbstractClass(PhysX::PhysXServer, 'PXSR', Physics::BasePhysicsServer);
__ImplementInterfaceSingleton(PhysX::PhysXServer);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
PhysXServer::PhysXServer()
{
    __ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
PhysXServer::~PhysXServer()
{
    __DestructInterfaceSingleton;
}


//------------------------------------------------------------------------------
/**
    Set the current physics level. The refcount of the level will be 
    incremented, the refcount of the previous level will be decremented
    (if exists). A 0 pointer is valid and will just release the
    previous level.

    @param  level   pointer to a Physics::Level object
*/
void
PhysXServer::SetScene(Physics::Scene * level)
{
	BasePhysicsServer::SetScene(level);    
}

//------------------------------------------------------------------------------
/**
    Initialize the physics subsystem.

    - 25-May-05 jo   Don't create default physics level.

    @return     true if physics subsystem initialized successfully
*/
bool
PhysXServer::Open()
{
	if (BasePhysicsServer::Open())
	{
		
		this->foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		n_assert2(this->foundation, "PxCreateFoundation failed!");
		
		bool recordMemoryAllocations = true;
		this->profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(this->foundation);
		n_assert2(this->profileZoneManager, "PxProfileZoneManager::createProfileZoneManager failed!");

		this->physics = PxCreatePhysics(PX_PHYSICS_VERSION, *this->foundation, PxTolerancesScale(), recordMemoryAllocations, this->profileZoneManager);
		n_assert2(this->physics, "PxCreatePhysics failed!");

		if (this->physics->getPvdConnectionManager())
		{
			this->physics->getVisualDebugger()->setVisualizeConstraints(true);
			this->physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
			this->physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
			this->pvd = PxVisualDebuggerExt::createConnection(this->physics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
		}

		this->cooking = PxCreateCooking(PX_PHYSICS_VERSION, *this->foundation, PxCookingParams(PxTolerancesScale()));
		n_assert2(this->cooking, "PxCreateCooking failed!");

		if (!PxInitExtensions(*this->physics))
		{
			n_error("PxInitExtensions failed!");
		}
			


	}
    return false;
}

//------------------------------------------------------------------------------
/**
    Close the physics subsystem.
*/
void
PhysXServer::Close()
{
	BasePhysicsServer::Close();
	this->cooking->release();	
	this->physics->release();
	this->profileZoneManager->release();
	this->foundation->release();
}

//------------------------------------------------------------------------------
/**
    Perform one or more simulation steps. The number of simulation steps
    performed depends on the time of the last call to Trigger().
*/
void
PhysXServer::Trigger()
{
	BasePhysicsServer::Trigger();
}

//------------------------------------------------------------------------------
/**
    Renders the debug visualization of the level.
*/
void
PhysXServer::RenderDebug()
{
	BasePhysicsServer::RenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXServer::HandleCollisions()
{
	n_error("not implemented");
}

} // namespace PhysX