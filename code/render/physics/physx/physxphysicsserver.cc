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
#include "../materialtable.h"

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
   
*/
void
PhysXServer::SetScene(Physics::Scene * level)
{
	BasePhysicsServer::SetScene(level);    
}

//------------------------------------------------------------------------------
/**
   
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
			
        Util::Array<Physics::MaterialTable::Material> mats = Physics::MaterialTable::GetMaterialTable();
        this->materials.SetSize(mats.Size()+1);
        for (int i = 0; i < mats.Size(); i++)
        {
            PxMaterial * newMat = this->physics->createMaterial(mats[i].friction, mats[i].friction, mats[i].restitution);            
            this->materials[i + 1] = newMat;
        }
        PxMaterial * invalidMat = this->physics->createMaterial(0.5, 0.5, 0.5);
        this->materials[0] = invalidMat;
	}
    return false;
}

//------------------------------------------------------------------------------
/**
    
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
    
*/
void
PhysXServer::Trigger()
{
	BasePhysicsServer::Trigger();
}

//------------------------------------------------------------------------------
/**
    
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