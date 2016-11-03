//------------------------------------------------------------------------------
//  havokscene.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/scene.h"
#include "physics/filterset.h"
#include "physics/physicsserver.h"
#include "physics/physicsbody.h"
#include "physics/visualdebuggerserver.h"
#include "physics/character.h"
#include "physics/model/templates.h"
#include "havokcharacterrigidbody.h"
#include "conversion.h"
#include "havokutil.h"

#include <Physics2012/Dynamics/World/hkpWorld.h>
#include <Physics2012/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics2012/Collide/Shape/hkpShapeBase.h>
#include <Physics2012/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>
#include <Physics2012/Dynamics/Collide/Filter/Pair/hkpPairCollisionFilter.h>
#include <Physics2012/Collide/Query/Collector/RayCollector/hkpAllRayHitCollector.h>
#include <Physics2012/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Collide/Agent/Query/hkpCdBodyPairCollector.h>
#include <Physics2012/Collide/Agent/Collidable/hkpCollidable.h>
#include <Physics2012/Collide/Query/Collector/BodyPairCollector/hkpAllCdBodyPairCollector.h>
#include <Physics2012/Collide/Agent/hkpProcessCollisionInput.h>
#include <Physics2012/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics2012/Collide/Filter/Group/hkpGroupFilterSetup.h>

using namespace Physics;
using namespace Math;

namespace Havok
{

__ImplementClass(Havok::HavokScene,'HKSC', Physics::BaseScene);

//------------------------------------------------------------------------------
/**
*/
HavokScene::HavokScene():
	initialized(false),
	lastFrame(-1)
{
	this->world = 0;
}

//------------------------------------------------------------------------------
/**
*/
HavokScene::~HavokScene()
{
	this->world = 0;
	this->filter = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::RenderDebug()
{
	IndexT i;
	for (i = 0; i < this->objects.Size() ; i++)
	{
		this->objects[i]->RenderDebug();
	}
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Contact>> 
HavokScene::RayCheck(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, BaseScene::RayTestType rayType)
{
	n_assert(this->IsInitialized());

    hkVector4 position = Neb2HkFloat4(pos);
	hkVector4 direction = Neb2HkFloat4(dir);
	hkVector4 targetPoint;
	targetPoint.setAdd(position, direction);

	hkpWorldRayCastInput rayCastInput;
	rayCastInput.m_enableShapeCollectionFilter = true;
	rayCastInput.m_filterInfo = (int)Math::n_log2(Physics::Picking) /*FIXME*/;
	rayCastInput.m_from = position;
	rayCastInput.m_to = targetPoint;

	hkArray<hkpWorldRayCastOutput> hits;

	if (Test_Closest == rayType)
	{
		hkpClosestRayHitCollector collector;
		this->world->castRay(rayCastInput, collector);
        if(collector.hasHit())
        {
            hkpWorldRayCastOutput hit = collector.getHit();
            hits.pushBack(hit);
        }        
	}
	else
	{
		hkpAllRayHitCollector collector;
		this->world->castRay(rayCastInput, collector);
		hits = collector.getHits();
	}

	Util::Array<Ptr<Contact>> contacts;

	IndexT i;
	for (i = 0; i < hits.getSize(); i++)
	{
		const hkpWorldRayCastOutput& hit = hits[i];

		if (HK_NULL == hit.m_rootCollidable)
		{
			continue;
		}

		// get the nebula physicsobject from the entity
		void* owner = hit.m_rootCollidable->getOwner();
		hkpWorldObject* entity = (hkpWorldObject*)owner;
		n_assert(0 != entity);

		hkUlong userData = entity->getUserData();
		HavokUtil::CheckWorldObjectUserData(userData);

		Ptr<PhysicsObject> physObject = (PhysicsObject*)userData;

		if (excludeSet.CheckObject(physObject))
		{
			continue;
		}

		Ptr<Contact> contact = Contact::Create();

		contact->SetCollisionObject(physObject);
		contact->SetMaterial(physObject->GetMaterialType());
		contact->SetType(Contact::RayCheck);
		contact->SetNormalVector(Hk2NebFloat4(hit.m_normal));
		contact->SetPoint(pos + (dir * hit.m_hitFraction));

		contacts.Append(contact);

		if (Test_Closest == rayType)
		{
			break;
		}
	}

	return contacts;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Contact> 
HavokScene::GetClosestContactUnderMouse(const Math::line& worldMouseRay, const FilterSet& excludeSet)
{
	Util::Array<Ptr<Contact>> contacts = this->RayCheck(worldMouseRay.b, worldMouseRay.m, excludeSet, Test_Closest);

	if (contacts.IsEmpty())
	{
		return 0;
	}

	return contacts[0];
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Contact> 
HavokScene::GetClosestContactAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet)
{
	Util::Array<Ptr<Contact>> contacts = this->RayCheck(pos, dir, excludeSet, Test_Closest);

	if (contacts.IsEmpty())
	{
		return 0;
	}

	return contacts[0];
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokScene::ApplyImpulseAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, float impulse)
{
	Util::Array<Ptr<Contact>> contacts = this->RayCheck(pos, dir, excludeSet, Test_All);

	bool hitValidTarget = false;

	IndexT i;
	for (i = 0; i < contacts.Size(); i++)
	{
		const Ptr<Contact> contact = contacts[i];
		const Ptr<PhysicsObject>& physObject = contact->GetCollisionObject();

		hkRefPtr<hkpRigidBody> rigidBody = HK_NULL;

		if (physObject->IsA(PhysicsBody::RTTI))
		{
			Ptr<HavokBody> havokBody = physObject.downcast<HavokBody>();
			rigidBody = havokBody->GetRigidBody();
		}

		if (HK_NULL != rigidBody)
		{
			hitValidTarget = true;

			vector impulseVector = float4::normalize(dir) * impulse;

			rigidBody->applyPointImpulse(Neb2HkFloat4(impulseVector), Neb2HkFloat4(contact->GetPoint()));
		}
	}

	return hitValidTarget;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::OnActivate()
{
	n_assert(!this->IsInitialized());

	// set up default behaviour for world
	hkpWorldCinfo worldInfo;	
	worldInfo.m_broadPhaseBorderBehaviour = 
#if _DEBUG
		//TODO: Revert when broadphase borders can be defined in the leveleditor
		hkpWorldCinfo::BROADPHASE_BORDER_DO_NOTHING; 
		//hkpWorldCinfo::BROADPHASE_BORDER_ASSERT;
#else
		hkpWorldCinfo::BROADPHASE_BORDER_DO_NOTHING; 
#endif

	worldInfo.m_fireCollisionCallbacks = true;	//< this is needed for custom contact listeners
	
	this->world = n_new(hkpWorld(worldInfo));

	// set default gravity
	this->SetGravity(Math::vector(0, -9.8f, 0));
	
	// this is needed to detect collisions
	hkpAgentRegisterUtil::registerAllAgents(this->world->getCollisionDispatcher());

	this->SetupCollisionFilter();

	this->initialized = true;

}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::OnDeactivate()
{
	n_assert(this->IsInitialized());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::SetGravity(const Math::vector& v)
{
	hkVector4 hkGravity = Neb2HkFloat4(v);
	this->world->setGravity(hkGravity);

	BaseScene::SetGravity(v);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::Trigger()
{
	// to have a consistent simulation 
	const float& simulationFrameTime = HavokPhysicsServer::Instance()->GetSimulationFrameTime();

	if (-1 == this->lastFrame)
	{
		// first frame, simulate one step
		this->lastFrame = PhysicsServer::Instance()->GetTime();
		this->StepWorld(simulationFrameTime);
		this->sinceLastTrigger = 0;

		return;
	}
	// else default behaviour:
	this->sinceLastTrigger += this->simulationSpeed * (PhysicsServer::Instance()->GetTime() - this->lastFrame);
	this->lastFrame = PhysicsServer::Instance()->GetTime();

	if (this->sinceLastTrigger > simulationFrameTime)
	{
		//for (; this->sinceLastTrigger > simulationFrameTime; this->sinceLastTrigger -= simulationFrameTime)	//< may cause noticable minilags!
		{
			this->StepWorld(simulationFrameTime);
			this->sinceLastTrigger -= simulationFrameTime;

			this->time += simulationFrameTime;
		}
	}

	// important note: it is tempting to just step the world using sinceLastTrigger but it will make the simulation inconsistent and
	// will likely mess up the behaviour of the character - for example jump height might vary between different jumps
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::StepWorld(float time)
{
	IndexT i;
	for (i = 0; i < this->objects.Size(); i++)
	{
		this->objects[i]->OnStepBefore();
	}
	
	this->world->stepDeltaTime(time);

	if (HavokVisualDebuggerServer::HasInstance())
	{
		// also perform a step for the visualdebugger
		HavokVisualDebuggerServer::Instance()->OnStep();
	}

	HavokPhysicsServer::Instance()->HandleCollisions();

	for (i = 0; i < this->objects.Size(); i++)
	{
		this->objects[i]->OnStepAfter();
	}
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokScene::GetObjectsInSphere(const Math::vector& pos, float radius, const Physics::FilterSet& excludeSet, Util::Array<Ptr<PhysicsObject>>& result)
{
	hkRefPtr<hkpShape> sphere = n_new(hkpSphereShape(radius));

	return this->GetObjectsInShape(sphere, matrix44::translation(pos), excludeSet, result);
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokScene::GetObjectsInBox(const Math::vector& scale, const Math::matrix44& m, const Physics::FilterSet& excludeSet, Util::Array<Ptr<PhysicsObject>>& result)
{
	hkRefPtr<hkpShape> box = n_new(hkpBoxShape(Neb2HkFloat4(scale)));

	return this->GetObjectsInShape(box, m, excludeSet, result);
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokScene::GetObjectsInShape(hkRefPtr<hkpShape> shape, const Math::matrix44& m, const Physics::FilterSet& excludeSet, Util::Array<Ptr<PhysicsObject>>& result)
{
	n_assert(result.IsEmpty());

	hkpCollidable* collidable = n_new(hkpCollidable(shape, &NebMatrix442HkTransform(m)));

	hkpAllCdBodyPairCollector collector;
	const hkpProcessCollisionInput* input = this->world->getCollisionInput();
	this->world->getPenetrations(collidable, *input, collector);

	const hkArray<hkpRootCdBodyPair>& hits = collector.getHits();

	IndexT i;
	for (i = 0; i < hits.getSize(); i++)
	{
		void* owner = hits[i].m_rootCollidableB->getOwner();

		hkpWorldObject* entity = (hkpWorldObject*)owner;
		n_assert(0 != entity);

		hkUlong userData = entity->getUserData();

		HavokUtil::CheckWorldObjectUserData(userData);

		Ptr<PhysicsObject> physObject = (PhysicsObject*)userData;

		if (excludeSet.CheckObject(physObject) 
			|| InvalidIndex != result.FindIndex(physObject))	//< might be a shape of a compoundshape belonging to the same object
		{
			continue;
		}

		result.InsertSorted(physObject);
	}

	n_delete(collidable);

	return result.Size();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::SetupCollisionFilter()
{
	n_assert(this->world);

	/// set up a group filter as the default
	hkRefPtr<hkpGroupFilter> groupFilter = n_new(hkpGroupFilter());

	// initialize the group filter like hkpGroupFilterSetup::setupGroupFilter but with the default nebula collidecategories
	groupFilter->enableCollisionsUsingBitfield(Physics::All, Physics::All);
	groupFilter->enableCollisionsUsingBitfield(1<<Physics::None, 0xffffffff);
	groupFilter->enableCollisionsUsingBitfield(0xffffffff, 1<<Physics::None);
	//FIXME: Physics::CollideCategory is already shifted - its values should optimally be just 1, 2, 3... like in hkpGroupFilterSetup::setupGroupFilter. 
	//FIXME: (ctd) The below workaround have also been made for the physicsobjects' SetCollideCategory-methods
	groupFilter->disableCollisionsBetween((int)n_log2(Physics::Kinematic), (int)n_log2(Physics::Kinematic));
	groupFilter->disableCollisionsBetween((int)n_log2(Physics::Kinematic), (int)n_log2(Physics::Static));

	this->world->setCollisionFilter(groupFilter);
	this->filter = groupFilter;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::DisableCollisionBetweenCategories(int categoryA, int categoryB)
{
	n_assert(HK_NULL != this->filter);

	hkpGroupFilter* groupFilter = dynamic_cast<hkpGroupFilter*>(this->filter.val());
	groupFilter->disableCollisionsBetween(categoryA, categoryB);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokScene::EnableCollisionBetweenCategories(int categoryA, int categoryB)
{
	n_assert(HK_NULL != this->filter);

	hkpGroupFilter* groupFilter = dynamic_cast<hkpGroupFilter*>(this->filter.val());
	groupFilter->enableCollisionsBetween(categoryA, categoryB);
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokScene::IsCollisionEnabledBetweenCategories(int categoryA, int categoryB)
{
	n_assert(HK_NULL != this->filter);

	hkpGroupFilter* groupFilter = dynamic_cast<hkpGroupFilter*>(this->filter.val());
	return groupFilter->isCollisionEnabled(categoryA, categoryB);
}

}