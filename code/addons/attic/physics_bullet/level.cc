//------------------------------------------------------------------------------
//  physics/level.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "debugdrawer.h"
#include "btBulletDynamicsCommon.h"
#include "BulletMultiThreaded/btThreadSupportInterface.h"
#include "Physics/shape.h"
#include "framesync/framesynctimer.h"
#include "Physics/level.h"
#include "Physics/physicsentity.h"
#include "Physics/composite.h"
#include "Physics/physicsserver.h"
#include "input/inputserver.h"
#include "core/factory.h"
#include "Physics/conversion.h"
#include "physics/contactpoint.h"
#include <typeinfo>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

namespace Physics
{
__ImplementClass(Physics::Level, 'PLE2', Core::RefCounted);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Level::Level() :
#ifdef __NEBULA_STATS__
    statsNumSpaceCollideCalled(0),
    statsNumNearCallbackCalled(0),
    statsNumCollideCalled(0),
    statsNumCollided(0),
    statsNumSpaces(0),
    statsNumShapes(0),
    statsNumSteps(0),
#endif
    time(0.0),
    stepSize(0.01),
    simTimeStamp(0.0),
    simSteps(0),
    collisionSounds(1024),       
	gravity(0.0f, 0.0f, 0.0f),    
	m_bDisablePhysics(false)
{
    /*PROFILER_INIT(this->profFrameBefore, "profMangaPhysFrameBefore");
    PROFILER_INIT(this->profFrameAfter, "profMangaPhysFrameAfter");
    PROFILER_INIT(this->profStepBefore, "profMangaPhysStepBefore");
    PROFILER_INIT(this->profStepAfter, "profMangaPhysStepAfter");
    PROFILER_INIT(this->profCollide, "profMangaPhysCollide");
    PROFILER_INIT(this->profStep, "profMangaPhysStep");
    PROFILER_INIT(this->profJointGroupEmpty, "profMangaPhysJointGroupEmpty");*/
}

//------------------------------------------------------------------------------
/**
*/
Level::~Level()
{
	n_assert(this->physics.dynamicsWorld == 0);
    n_assert(this->shapeArray.Size() == 0);
    n_assert(this->entityArray.Size() == 0);
}


//------------------------------------------------------------------------------
/**
    Called by Physics::PhysicsServer when the level is attached to the server.
*/
void
Level::OnActivate()
{
	
this->physics.collisionConfiguration = n_new(btDefaultCollisionConfiguration());

    //const btVector3 worldAabbMin(-200,-10,-200);
    //const btVector3 worldAabbMax(200,200,200);

    //this->physics.broadphase = n_new(btAxisSweep3(worldAabbMin, worldAabbMax));

	// use AABB 
    this->physics.broadphase = n_new(btDbvtBroadphase());
    this->physics.constraintSolver = n_new(btSequentialImpulseConstraintSolver());
    this->physics.dispatcher = n_new(btCollisionDispatcher(this->physics.collisionConfiguration));
    this->physics.dynamicsWorld = n_new(btDiscreteDynamicsWorld(this->physics.dispatcher, this->physics.broadphase, this->physics.constraintSolver, this->physics.collisionConfiguration));

    this->debugDrawer = n_new(DebugDrawer);
    this->debugDrawer->setDebugMode(
        btIDebugDraw::DBG_DrawFeaturesText +
        btIDebugDraw::DBG_DrawText +
        btIDebugDraw::DBG_DrawWireframe +
        btIDebugDraw::DBG_DrawContactPoints/* +
        btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits*/);
    this->physics.dynamicsWorld->setDebugDrawer(this->debugDrawer);
	this->physics.dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

//------------------------------------------------------------------------------
/**
    Called by Physics::PhysicsServer when the level is removed from the server.
*/
void
Level::OnDeactivate()
{
    n_assert(0 != this->physics.dynamicsWorld);
    
    // clear the collision sound hash table
    this->collisionSounds.Clear();

    // release all attached collide shapes
    int shapeIndex;
    int numShapes = this->GetNumShapes();	
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        physics.dynamicsWorld->removeCollisionObject(this->shapeArray[shapeIndex]->GetShape());
        this->shapeArray[shapeIndex] = 0;
    }
    this->shapeArray.Clear();

    // release all attached entities
    int entityIndex;
    int numEntities = this->GetNumEntities();
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        this->entityArray[entityIndex]->OnRemovedFromLevel();
        this->entityArray[entityIndex] = 0;
    }
    this->entityArray.Clear();

	n_delete(this->physics.dynamicsWorld);
	this->physics.dynamicsWorld = NULL;

	n_delete(this->physics.collisionConfiguration);
	this->physics.collisionConfiguration = NULL;

	n_delete(this->physics.broadphase);
	this->physics.broadphase = NULL;

	n_delete(this->physics.constraintSolver);
	this->physics.constraintSolver = NULL;

	n_delete(this->physics.dispatcher);
	this->physics.dispatcher = NULL;

	n_delete(this->debugDrawer);
	this->debugDrawer = NULL;
}

//------------------------------------------------------------------------------
/**
    Set current gravity.
*/
void
Level::SetGravity(const Math::vector& v)
{
    this->gravity = v;
    if (this->physics.dynamicsWorld)
    {
		this->physics.dynamicsWorld->setGravity(Neb2BtVector(gravity));        
    }
}

//------------------------------------------------------------------------------
/**
    Attach a static collide shape to the level.
*/
void
Level::AttachShape(Physics::Shape* shape)
{
    n_assert(shape);
    this->shapeArray.Append(shape);
	physics.dynamicsWorld->addCollisionObject(shape->GetShape());    
}

//------------------------------------------------------------------------------
/**
    Remove a static collide shape to the level
*/
void
Level::RemoveShape(Physics::Shape* shape)
{
    n_assert(shape);
    Util::Array<Ptr<Physics::Shape> >::Iterator iter = this->shapeArray.Find(shape);
    n_assert(iter);
	physics.dynamicsWorld->removeCollisionObject(shape->GetShape());    
    this->shapeArray.Erase(iter);
}

//------------------------------------------------------------------------------
/**
    Get number of collide shapes attached to level.
*/
int
Level::GetNumShapes() const
{
    return this->shapeArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return pointer to collide shape at index.
*/
Shape*
Level::GetShapeAt(int index) const
{
    return this->shapeArray[index];
}

//------------------------------------------------------------------------------
/**
    Attach a physics entity to the level. The entity's refcount
    will be incremented.

    @param  entity      pointer to a physics entity
*/
void
Level::AttachEntity(PhysicsEntity* entity)
{
    n_assert(entity);
    n_assert(entity->GetLevel() == 0);

    // make sure the entity is not already in the array
    n_assert(!this->entityArray.Find(entity));

	entity->OnActivate();
	entity->OnAttachedToLevel(this);
    this->entityArray.Append(entity);
}

//------------------------------------------------------------------------------
/**
    Remove a physics entity from the level. The entity must currently be
    attached to this level! The refcount of the entity will be decremented.

    @param  entity      pointer to a physics entity
*/
void
Level::RemoveEntity(PhysicsEntity* entity)
{
    n_assert(entity);
    n_assert(entity->GetLevel() == this);

    n_assert(entity->GetRefCount() > 0);
    Util::Array<Ptr<PhysicsEntity> >::Iterator iter = this->entityArray.Find(entity);
    n_assert(0 != iter);
    this->entityArray.Erase(iter);
    entity->OnRemovedFromLevel();
    entity->OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    Get entity pointer at index.
*/
PhysicsEntity*
Level::GetEntityAt(int index) const
{
    return this->entityArray[index];
}



//------------------------------------------------------------------------------
/**
    Trigger the simulation. This method should be called frequently
    (call SetTime() before invoking Trigger() to update the current time).
    The method will invoke dWorldStep one or several times, depending
    on the time since the last call, and the step size of the level.
    The method will make sure that the physics simulation is triggered
    using a constant step size.
*/
void
Level::Trigger()
{
    //Server* server = Physics::PhysicsServer::Instance();

	//hack for level editor
	if(this->GetDisabledPhysics())
		return;	
		
	// FIXME (does this even work?)
	Timing::Time delta_t =  FrameSync::FrameSyncTimer::Instance()->GetFrameTime();	
	if(delta_t > 0.0f)
	{
		this->physics.dynamicsWorld->stepSimulation((btScalar)delta_t, 10000);	
		int numManifolds = this->physics.dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i=0;i<numManifolds;i++)
		{
			btPersistentManifold* contactManifold =  this->physics.dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
						
			Core::RefCounted *  va = (Core::RefCounted *)obA->getUserPointer();
			Core::RefCounted *  vb = (Core::RefCounted *)obB->getUserPointer();

			if(!va || !vb)
				continue;
			if(contactManifold->getNumContacts()==0)
				continue;
			
			Ptr<ContactPoint> cp = ContactPoint::Create();
						
			const btManifoldPoint &contact = contactManifold->getContactPoint(0);				
			cp->SetNormalVector(Bt2NebVector(contact.m_normalWorldOnB));
			cp->SetPosition(Bt2NebVector(contact.getPositionWorldOnA()));
			cp->SetDepth(contact.getDistance());						
			for(int cc = 0; cc < contactManifold->getNumContacts();cc++)
			{
				Ptr<ContactPoint> newcp = ContactPoint::Create();
				newcp->SetNormalVector(Bt2NebVector(contactManifold->getContactPoint(cc).m_normalWorldOnB));
				newcp->SetPosition(Bt2NebVector(contactManifold->getContactPoint(cc).getPositionWorldOnA()));
				newcp->SetDepth(contactManifold->getContactPoint(cc).getDistance());
				cp->AddContact(newcp);
			}						
			
			if(va->IsA(RigidBody::RTTI))
			{
				PhysicsEntity * ent = static_cast<RigidBody*>(va)->GetEntity();
				if(ent)
				{
					if(vb->IsA(RigidBody::RTTI) && ent->OnCollide(static_cast<RigidBody*>(vb)))
					{
						RigidBody * bb = static_cast<RigidBody*>(vb);
						cp->SetRigidBody(bb);
						if(bb->GetEntity())
							ent->CollideCallback(bb->GetEntity(),cp);
					}
					else if(vb->IsA(Shape::RTTI) && ent->OnCollide(static_cast<Shape*>(vb)))
					{
						ent->CollideCallback(static_cast<Shape*>(vb),cp);
					}
				}
			}

			if(vb->IsA(RigidBody::RTTI))
			{
				PhysicsEntity * ent = static_cast<RigidBody*>(vb)->GetEntity();
				if(ent)
				{
					if(va->IsA(RigidBody::RTTI) && ent->OnCollide(static_cast<RigidBody*>(va)))
					{
						RigidBody * bb = static_cast<RigidBody*>(va);
						cp->SetRigidBody(bb);
						if(bb->GetEntity())
							ent->CollideCallback(bb->GetEntity(),cp);
					}
					else if(va->IsA(Shape::RTTI) && ent->OnCollide(static_cast<Shape*>(va)))
					{
						ent->CollideCallback(static_cast<Shape*>(va),cp);
					}
				}
			}
		}
	}
	
   
    #ifdef __NEBULA_STATS__
    this->statsNumNearCallbackCalled = 0;
    this->statsNumCollideCalled = 0;
    this->statsNumCollided = 0;
    this->statsNumSpaceCollideCalled = 0;
    this->statsNumSteps = 0;
    #endif
#if 0
  
    // invoke the "on-frame-after" methods
    //PROFILER_START(this->profFrameAfter);
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        this->GetEntityAt(entityIndex)->OnFrameAfter();
    }
    //PROFILER_STOP(this->profFrameAfter);
#endif
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the level. 
    Called by Physics::PhysicsServer::RenderDebug().
*/
void
Level::RenderDebug()
{
    // render collide shapes
	this->physics.dynamicsWorld->debugDrawWorld();
    const Math::matrix44 identity = Math::matrix44::identity();
    int numShapes = this->GetNumShapes();
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        Shape* shape = this->GetShapeAt(shapeIndex);
        shape->RenderDebug(identity);
    }

    // render entities
    int numEntities = this->GetNumEntities();
    int entityIndex;
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        PhysicsEntity* entity = this->GetEntityAt(entityIndex);
        entity->RenderDebug();
    }
}


} // namespace Physics