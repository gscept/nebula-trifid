//------------------------------------------------------------------------------
//  physxscene.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "physics/physx/physxscene.h"
#include "physics/physicsbody.h"
#include "physics/collider.h"
#include "db/filterset.h"
#include "PxScene.h"
#include "PxSceneDesc.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "physxphysicsserver.h"
#include "physxutils.h"
#include "PxPhysics.h"
#include "PxRigidActor.h"
#include "PxSimulationEventCallback.h"
#include "physics/physicsprobe.h"
#include "characterkinematic/PxControllerManager.h"
#include "PxQueryReport.h"
#include "physics/physicsobject.h"

namespace PhysX
{

using namespace Physics;
using namespace physx;


class EventCallBack : public physx::PxSimulationEventCallback
{
public:
	EventCallBack(PhysXScene * scene) { this->scene = scene; }
	///
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
	///
	virtual void onWake(PxActor** actors, PxU32 count) {}
	///
	virtual void onSleep(PxActor** actors, PxU32 count){}
	///
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs){}
	///
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count);
	PhysXScene * scene;
};

//------------------------------------------------------------------------------
/**
*/
void
EventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count;i++)
	{
		n_assert(pairs[i].triggerActor->userData && ((Core::RefCounted*)pairs[i].triggerActor->userData)->IsA(Physics::PhysicsProbe::RTTI));
		PhysX::PhysXProbe * probe = (PhysX::PhysXProbe*)pairs[i].triggerActor->userData;
		if (pairs[i].otherActor->userData)
		{
			probe->AddOverlap(pairs[i].otherActor);
		}		
	}
}

__ImplementAbstractClass(PhysX::PhysXScene, 'PXSC', Physics::BaseScene);

//------------------------------------------------------------------------------
/**
*/
PhysXScene::PhysXScene():
	scene(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PhysXScene::~PhysXScene()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXScene::Attach(const Ptr<Physics::PhysicsObject> & obj)
{
	if (obj->IsA(PhysX::PhysXProbe::RTTI))
	{
		this->triggers.InsertSorted(obj.cast<PhysX::PhysXProbe>());
	}
	BaseScene::Attach(obj);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::Detach(const Ptr<Physics::PhysicsObject> & obj)
{
	if (obj->IsA(PhysX::PhysXProbe::RTTI))
	{
		IndexT idx = this->triggers.BinarySearchIndex(obj.cast<PhysX::PhysXProbe>());
		n_assert(idx != InvalidIndex);
		this->triggers.EraseIndex(idx);
	}
	BaseScene::Detach(obj);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::RenderDebug()
{
	n_error("not implemented");
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Physics::Contact>>
PhysXScene::RayCheck(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, RayTestType rayType)
{	
	Math::vector ndir = Math::vector::normalize3(dir);
	PxRaycastBuffer hits;
	Util::Array<Ptr<Physics::Contact>> contacts;
	//FIXME setup filter
	PxQueryFilterData filter;
	filter.flags = PxQueryFlag::eANY_HIT;
	if (this->scene->raycast(Neb2PxVec(pos), Neb2PxVec(ndir), dir.length3(), hits, PxHitFlag::eDEFAULT, filter))
	{
		Ptr<Physics::Contact> contact = Physics::Contact::Create();		
		const PxRaycastHit & hit = hits.touches[0];
		contact->SetPoint(Px2NebPoint(hit.position));
		contact->SetType(Physics::BaseContact::Type::RayCheck);
		contact->SetCollisionObject((Physics::PhysicsObject*)hit.actor->userData);
		contacts.Append(contact);
	}
	return contacts;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::SetGravity(const Math::vector& v)
{
	this->scene->setGravity(Neb2PxVec(v));
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::AttachStatic(const Ptr<Physics::PhysicsObject> &obj)
{
	n_error("not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::OnActivate()
{	
	BaseScene::OnActivate();
	PxSceneDesc sceneDesc(PhysXServer::Instance()->physics->getTolerancesScale());
	sceneDesc.gravity = Neb2PxVec(this->GetGravity());
	this->dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = this->dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	this->scene = PhysXServer::Instance()->physics->createScene(sceneDesc);
	this->eventCallback = n_new(EventCallBack(this));
	this->scene->setSimulationEventCallback(this->eventCallback);
	this->time = -1.0f;
	this->controllerManager= PxCreateControllerManager(*this->scene);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::OnDeactivate()
{
	BaseScene::OnDeactivate();
	n_delete(this->eventCallback);
	this->controllerManager->release();
	this->scene->release();
	this->dispatcher->release();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::Trigger()
{	
	const float& simulationFrameTime = PhysXServer::Instance()->GetSimulationFrameTime();

	if (this->time < 0.0f)
	{
		// first frame, simulate one step
		this->time = PhysXServer::Instance()->GetTime()+ simulationFrameTime;
		this->scene->simulate(simulationFrameTime);		
		// for now be synchronous
		this->scene->fetchResults(true);
	}
	else
	{		
		Timing::Time delta = this->simulationSpeed * (PhysXServer::Instance()->GetTime() - this->time);
		if (delta > simulationFrameTime)
		{
			// clear triggers
			for (int i = 0;i < this->triggers.Size();i++)
			{
				this->triggers[i]->ClearOverlap();
			}			
		}
		while (delta > simulationFrameTime)
		{
			this->scene->simulate(simulationFrameTime);
			this->scene->fetchResults(true);
			delta -= simulationFrameTime;
			this->time += simulationFrameTime;
		}
	}		
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXScene::AddIgnoreCollisionPair(const Ptr<Physics::PhysicsBody>& bodyA, const Ptr<Physics::PhysicsBody>& bodyB)
{
	n_error("BaseScene::AddIgnoreCollisionPair: Do not use the base method, implement and use the method from a sub class from this");
}

}