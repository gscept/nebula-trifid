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

namespace PhysX
{

using namespace Physics;
using namespace physx;

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
	BaseScene::Attach(obj);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::Detach(const Ptr<Physics::PhysicsObject> & obj)
{
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
    n_error("not implemented");
    return Util::Array<Ptr<Physics::Contact>>();
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
	this->time = -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::OnDeactivate()
{
	BaseScene::OnDeactivate();
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