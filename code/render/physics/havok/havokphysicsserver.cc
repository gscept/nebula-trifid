//------------------------------------------------------------------------------
//  havokphysicsserver.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokphysicsserver.h"
#include "havokutil.h"
#include "physics/scene.h"
#include "physics/visualdebuggerserver.h"
#include "physics/contactlistener.h"

// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Memory/Allocator/hkMemoryAllocator.h>

//FIXME: all of these includes are not necessary, clean
// Dynamics includes
#include <Physics2012/Collide/hkpCollide.h>										
#include <Physics2012/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>	
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics2012/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics2012/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics2012/Collide/Shape/Convex/Sphere/hkpSphereShape.h>				
#include <Physics2012/Collide/Dispatch/hkpAgentRegisterUtil.h>		

#include <Physics2012/Dynamics/Collide/Filter/Constraint/hkpConstraintCollisionFilter.h>
#include <Physics/Constraint/Data/Ragdoll/hkpRagdollConstraintData.h>
#include <Physics/Constraint/Data/Prismatic/hkpPrismaticConstraintData.h>
#include <Physics/Constraint/Motor/Position/hkpPositionConstraintMotor.h>

#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics2012/Dynamics/World/hkpWorld.h>								
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>		
#include <Physics2012/Dynamics/Constraint/hkpConstraintInstance.h>
#include <Physics2012/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	
#include "physics/havok/havokcontactlistener.h"

#include <Physics2012/Utilities/VisualDebugger/Viewer/Dynamics/hkpConstraintViewer.h>

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
//#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

// for som reason this caused compiler errors (not anymore): Error	1	error C3861: 'hkpHeightFieldAgent_registerSelf': identifier not found	c:\havok\source\common\base\config\hkproductfeatures.cxx	105
// Keycode
#include <Common/Base/keycode.cxx>

#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
//#define HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
//#define HK_FEATURE_REFLECTION_PHYSICS

//TODO. remove?
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
// productfeatures must be added to one .cc file in a project (do not include this to projects using render lib)
#include <Common/Base/Config/hkProductFeatures.cxx>

static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
{
	n_warning(" ## Havok error report: ## %s\n", msg);
}

namespace Havok
{
using namespace Physics;

__ImplementClass(Havok::HavokPhysicsServer,'HKPS', Physics::BasePhysicsServer);
__ImplementSingleton(Havok::HavokPhysicsServer);

//------------------------------------------------------------------------------
/**
*/
HavokPhysicsServer::HavokPhysicsServer()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
HavokPhysicsServer::~HavokPhysicsServer()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokPhysicsServer::Open()
{
	if (BasePhysicsServer::Open())
	{
#ifdef _DEBUG
		//hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initChecking(
		hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(
			hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(1024*1024) );
#else
		hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(
			hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(1024*1024) );
#endif
		hkBaseSystem::init(memoryRouter, errorReport);		

		if (this->initVisualDebugger)
		{
			this->visualDebuggerServer = Physics::VisualDebuggerServer::Create();
			this->visualDebuggerServer->SetSimulationFrameTime(this->simulationFrameTime);
		}

		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsServer::Close()
{
	BasePhysicsServer::Close();

	this->visualDebuggerServer = 0;

	IndexT i;
	for (i = 0; i < this->contactListeners.Size(); i++)
	{
		const Ptr<ContactListener>& listener = this->contactListeners[i];
		listener->Clear();
	}

	this->contactListeners.Clear();

	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsServer::Trigger()
{
	BasePhysicsServer::Trigger();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsServer::RenderDebug()
{
	BasePhysicsServer::RenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsServer::HandleCollisions()
{
	// first the data of active collisions are checked, then the data of expired collisions - the 
	// data of expired collisions is saved in order to make sure collisions between very fast 
	// moving objects are handled (otherwise the collision might be unregistered before the 
	// physserver has a chance to process it)
	IndexT c, i, j;
	for (c = 0; c < 2; c++)
	{
		// c = 0 -> active collisions, c = 1 -> expired collisions

		for (i = 0; i < this->contactListeners.Size(); i++)
		{
			const Ptr<ContactListener>& listener = this->contactListeners[i];

			SizeT startNumCols = listener->GetNumCollisions();	//< use this to handle if a collision is suddenly removed, see end of loop (used for active collisions)
			for (j = 0; j < (0 == c? listener->GetNumCollisions(): listener->GetNumExpiredCollisions()) ; j++)
			{
				ContactListener::HavokCollisionData* data = 
					0 == c?
					static_cast<ContactListener::HavokCollisionData*>(listener->GetCollisionDataAtIndex(j)):
					static_cast<ContactListener::HavokCollisionData*>(listener->GetExpiredCollisionDataAtIndex(j));

				if (c == 0)
				{
					// avoid reporting contacts repeatedly unless there is new data
					if (!data->hasUnhandledInfo)
					{
						continue;
					}

					data->hasUnhandledInfo = false;
				}
				else
				{
					n_assert(data->hasUnhandledInfo);
				}

				// report contact data to receivers
				Ptr<Contact> contact = Contact::Create();
				
				float depth = 0;
				const Util::Array<float>& contactDistances = data->distances;
				IndexT k;
				for (k = 0; k < contactDistances.Size(); k++)
				{
					const float& dist = contactDistances[k];

					if (dist > depth)
					{
						depth = dist;
					}
				}

				contact->SetSeparatingVelocity(data->separatingVelocity);
				contact->SetContactEventType(data->contactEventType);
				contact->SetDepth(depth);
				contact->SetNormalVectors(data->normals);
				contact->SetPoints(data->points);

				Ptr<PhysicsObject> object1 = data->object1;
				Ptr<PhysicsObject> object2 = data->object2;

				if (object1->GetUserData()->enableCollisionCallback 
					&& (object1->GetCollideFilter() & object2->GetCollideCategory()) == 0)
				{
					contact->SetOwnerObject(object1);
					contact->SetCollisionObject(object2);
					contact->SetMaterial(object2->GetMaterialType());

					for(Util::Array<CollisionReceiver*>::Iterator iter = this->receivers.Begin();iter != this->receivers.End();iter++)
					{
						(*iter)->OnCollision(object1, object2, contact);
					}
				}

				if (object2->GetUserData()->enableCollisionCallback 
					&& (object2->GetCollideFilter() & object1->GetCollideCategory()) == 0)
				{
					contact->SetOwnerObject(object2);
					contact->SetCollisionObject(object1);
					contact->SetMaterial(object1->GetMaterialType());

					for(Util::Array<CollisionReceiver*>::Iterator iter = this->receivers.Begin();iter != this->receivers.End();iter++)
					{
						(*iter)->OnCollision(object2, object1, contact);
					}
				}

				if (0 == c)	//< only do this for active collisions
				{	
					/* if any of the OnCollison calls resulted in the world to ignore collisions between one or more pair of objects, 
						then there would possible be fewer collisions during this loop - check and decrease j if necessary
					*/
					if (startNumCols > listener->GetNumCollisions())
					{
						int diff = startNumCols - listener->GetNumCollisions();

						startNumCols -= diff;
						j -= diff;

						n_assert(j >= 0);
					}
				}
			}

			if (1 == c)
			{
				// done traversing the expired collisions of this listener
				listener->ClearExpiredCollisions();
			}
		}
	}

}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsServer::SetScene(Physics::Scene* level)
{
	BasePhysicsServer::SetScene(level);

	if (this->visualDebuggerServer.isvalid() && 0 != level)
	{
		hkpWorld* world = level->GetWorld();

		this->visualDebuggerServer->Initialize(level);
	}
}
//------------------------------------------------------------------------------
/**
*/
void 
HavokPhysicsServer::RegisterContactListener(const Ptr<Physics::ContactListener>& listener)
{
	//TODO: when should they be unregistered? what happens on level change?
	n_assert(InvalidIndex == this->contactListeners.FindIndex(listener));

	this->contactListeners.Append(listener);
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Physics::ContactListener>> 
HavokPhysicsServer::GetObjectContactListeners(const Ptr<Physics::PhysicsObject>& physObj)
{
	// the given object must be eligible to have a contactlistener
	n_assert(HavokUtil::HasHavokRigidBody(physObj));

	Util::Array<Ptr<Physics::ContactListener>> listeners;

	IndexT i;
	for (i = 0; i < this->contactListeners.Size(); i++)
	{
		const Ptr<ContactListener>& listener = this->contactListeners[i];

		if (listener->GetOwner() == physObj)
		{
			listeners.Append(listener);
		}
	}

	return listeners;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Physics::ContactListener> 
HavokPhysicsServer::GetObjectContactListener(const Ptr<Physics::PhysicsObject>& physObj)
{
	// the given object must be eligible to have a contactlistener
	n_assert(HavokUtil::HasHavokRigidBody(physObj));

	IndexT i;
	for (i = 0; i < this->contactListeners.Size(); i++)
	{
		const Ptr<ContactListener>& listener = this->contactListeners[i];

		if (listener->GetOwner() == physObj)
		{
			return listener;
		}
	}

	return 0;
}

}