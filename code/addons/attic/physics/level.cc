//------------------------------------------------------------------------------
//  physics/level.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/level.h"
#include "physics/physicsentity.h"
#include "physics/composite.h"
#include "physics/shape.h"
#include "physics/physicsserver.h"
#include "physics/rigidbody.h"
#include "input/inputserver.h"
#include "core/factory.h"

namespace Physics
{
__ImplementClass(Physics::Level, 'PLEV', Core::RefCounted);

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
    odeWorldId(0),
    odeDynamicSpaceId(0),
    odeStaticSpaceId(0),
    odeCommonSpaceId(0),
    contactJointGroup(0),
    //gravity(0.0f, -9.81f, 0.0f),
	gravity(0.0f, 0.0f, 0.0f),
    scrachSpaceID(0),
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
    n_assert(0 == this->odeWorldId);
    n_assert(0 == this->odeDynamicSpaceId);
    n_assert(0 == this->odeStaticSpaceId);
    n_assert(0 == this->odeCommonSpaceId);
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
    // initialize ODE
    this->odeWorldId = dWorldCreate();
    dWorldSetQuickStepNumIterations(this->odeWorldId, 20);

	// FIXME(enno): is a quadtree significantly faster? -- can't count geoms with quadtree
    this->odeCommonSpaceId  = dSimpleSpaceCreate(0);
    this->odeDynamicSpaceId = dSimpleSpaceCreate(this->odeCommonSpaceId);
    this->odeStaticSpaceId  = dSimpleSpaceCreate(this->odeCommonSpaceId);

    dVector3 odeVector;
    PhysicsServer::Vector3ToOde(this->GetGravity(), odeVector);
    dWorldSetGravity(this->odeWorldId, odeVector[0], odeVector[1], odeVector[2]);
    dWorldSetContactSurfaceLayer(this->odeWorldId, 0.001f);
    dWorldSetContactMaxCorrectingVel(this->odeWorldId, 100.0f);
    dWorldSetERP(this->odeWorldId, 0.2f);     // ODE's default value
    dWorldSetCFM(this->odeWorldId, 0.001f);    // the default is 10^-5

    // setup autodisabling
    dWorldSetAutoDisableFlag(this->odeWorldId, 1);
    dWorldSetAutoDisableSteps(this->odeWorldId, 5);
    dWorldSetAutoDisableLinearThreshold(this->odeWorldId, 0.05f);   // default is 0.01
    dWorldSetAutoDisableAngularThreshold(this->odeWorldId, 0.1f);  // default is 0.01

    // create a contact group for joints
    this->contactJointGroup = dJointGroupCreate(0);

    // init ode
    dInitODE();
}

//------------------------------------------------------------------------------
/**
    Called by Physics::PhysicsServer when the level is removed from the server.
*/
void
Level::OnDeactivate()
{
    n_assert(0 != this->odeWorldId);
    n_assert(0 != this->odeDynamicSpaceId);
    n_assert(0 != this->odeStaticSpaceId);
    n_assert(0 != this->odeCommonSpaceId);

    // clear the collision sound hash table
    this->collisionSounds.Clear();

    // release all attached collide shapes
    int shapeIndex;
    int numShapes = this->GetNumShapes();
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        this->shapeArray[shapeIndex]->Detach();
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

    // delete the contact group for joints
    dJointGroupDestroy(this->contactJointGroup);

    // shutdown ode
    dSpaceDestroy(this->odeDynamicSpaceId);
    dSpaceDestroy(this->odeStaticSpaceId);
    dSpaceDestroy(this->odeCommonSpaceId);
    dWorldDestroy(this->odeWorldId);
   
    // clean up scratch spaces
    while(this->scrachSpaces.Size())
    {
        this->RemoveScratchSpace(this->scrachSpaces.Size() - 1);
    }

    dCloseODE();
    this->odeCommonSpaceId = 0;
    this->odeDynamicSpaceId = 0;
    this->odeStaticSpaceId = 0;
    this->odeWorldId = 0;
}

//------------------------------------------------------------------------------
/**
    Set current gravity.
*/
void
Level::SetGravity(const Math::vector& v)
{
    this->gravity = v;
    if (this->odeWorldId)
    {
        dVector3 odeVector;
        PhysicsServer::Vector3ToOde(v, odeVector);
        dWorldSetGravity(this->odeWorldId, odeVector[0], odeVector[1], odeVector[2]);
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
    if (!shape->Attach(this->odeStaticSpaceId))
    {
        n_error("Level::AttachShape(): Failed opening a shape!");
    }
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
    (*iter)->Detach();
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
    The "Near Callback". ODE calls this during collision detection to
    decide whether 2 geoms collide, and if yes, to generate contact
    joints between the 2 involved rigid bodies.
*/
void
Level::OdeNearCallback(void* data, dGeomID o1, dGeomID o2)
{
    Level* level = (Level*) data;

	//hack for level editor
	if(level->GetDisabledPhysics())
		return;
    //level->statsNumNearCallbackCalled++;

    // handle sub-spaces
    if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
    {
        //level->statsNumSpaceCollideCalled++;
        // collide a space with something
        dSpaceCollide2(o1, o2, data, &OdeNearCallback);
        return;
    }

    // handle shape/shape collisions
    dBodyID body1 = dGeomGetBody(o1);
    dBodyID body2 = dGeomGetBody(o2);

    //// do nothing if 2 bodies are connected by a joint
    // HACK ... FOR GODSEND
    //n_assert(body1 != body2);
    if (body1 && body2)
    {
        if (dAreConnectedExcluding(body1, body2, dJointTypeContact))
        {
            // FIXME: bodies are connected, check if jointed-collision is enabled
            // for both bodies (whether 2 bodies connected by a joint should 
            // collide or not, for this, both bodies must have set the
            // CollideConnected() flag set.
            RigidBody* physicsBody0 = (RigidBody*) dBodyGetData(body1);
            n_assert(physicsBody0 && physicsBody0->IsInstanceOf(RigidBody::RTTI));
            if (!physicsBody0->GetConnectedCollision())
            {
                return;
            }
            RigidBody* physicsBody1 = (RigidBody*) dBodyGetData(body2);
            n_assert(physicsBody1 && physicsBody1->IsInstanceOf(RigidBody::RTTI));
            if (!physicsBody1->GetConnectedCollision())
            {
                return;
            }
        }
    }
     //HACK ... FOR GODSEND

    Shape* shape1 = Shape::GetShapeFromGeom(o1);
    Shape* shape2 = Shape::GetShapeFromGeom(o2);
    n_assert(shape1 && shape2);
    n_assert(!((shape1->GetType() == Shape::Mesh) && (shape2->GetType() == Shape::Mesh)));

    shape1->ClearContactPoints();
    shape2->ClearContactPoints();
    //Server* server = Physics::PhysicsServer::Instance();
    //level->statsNumCollideCalled++;

    // initialize contact array
    Physics::MaterialType mat1 = shape1->GetMaterialType();
    Physics::MaterialType mat2 = shape2->GetMaterialType();
    float friction = Physics::MaterialTable::GetFriction(mat1, mat2);
    float bounce   = Physics::MaterialTable::GetBounce(mat1, mat2);
    static dContact contact[MaxContacts];
    int i;
    for (i = 0; i < MaxContacts; i++)
    {
        contact[i].surface.mode = dContactBounce | dContactSoftCFM;
        contact[i].surface.mu = friction;
        contact[i].surface.mu2 = 0.0f;
        contact[i].surface.bounce = bounce;
        contact[i].surface.bounce_vel = 1.0f;
        contact[i].surface.soft_cfm = 0.0001f;  
        contact[i].surface.soft_erp = 0.2f;
    }

    // do collision detection
    int numColls = dCollide(o1, o2, MaxContacts, &(contact[0].geom), sizeof(dContact));
    if (numColls > 0)
    {
        //level->statsNumCollided++;
        bool validCollision = true;
        validCollision &= shape1->OnCollide(shape2);
        validCollision &= shape2->OnCollide(shape1);
        if (!validCollision)
        {
            return;
        }
        Util::Array<ContactPoint> contactPoints1;
        Util::Array<ContactPoint> contactPoints2;
        contactPoints1.Reserve(numColls);
        contactPoints2.Reserve(numColls);
        Math::vector contactPos;
        Math::vector contactNormal;
        for (i = 0; i < numColls; i++)
        {  
            //// create a contact for each collision
            // HACK ... FOR GODSEND
            dJointID jointId = dJointCreateContact(level->odeWorldId, level->contactJointGroup, &(contact[i]));
            dJointAttach(jointId, body1, body2);
            // HACK ... FOR GODSEND
            ContactPoint contactPoint;
            contactPos.set(contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2]);
            contactNormal.set(contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2]);
            contactPoint.SetPosition(contactPos);
            contactPoint.SetUpVector(contactNormal);

            // set type depending on spaces
            const dSpaceID s1 = dGeomGetSpace(o1);
            const dSpaceID s2 = dGeomGetSpace(o2);            
            if (s1 == s2 && s2 == level->GetOdeDynamicSpaceId())
            {
                contactPoint.SetType(ContactPoint::DynamicType);
            }
            else if (s1 != s2)
            {
                contactPoint.SetType(ContactPoint::StaticType);
            }
            else
            {
                n_error("Collision between two static objects detected! Should NEVER happen!!!");
            }

            // first entity
            PhysicsEntity* entity = shape2->GetEntity();
            if (entity)
            {
                contactPoint.SetPhysicsEntityId(entity->GetUniqueId());            
            }
            RigidBody* rigidBody = shape2->GetRigidBody();
            if (rigidBody)
            {
                contactPoint.SetRigidBodyId(rigidBody->GetUniqueId());
            }
            contactPoints1.Append(contactPoint);

            // second entity
            entity = shape1->GetEntity();
            if (entity)
            {
                contactPoint.SetPhysicsEntityId(entity->GetUniqueId());            
            }
            rigidBody = shape1->GetRigidBody();
            if (rigidBody)
            {
                contactPoint.SetRigidBodyId(rigidBody->GetUniqueId());
            }
            contactPoints2.Append(contactPoint);
        }
        shape1->SetContactPoints(contactPoints1);
        shape2->SetContactPoints(contactPoints2);
		
		PhysicsEntity* e1 = shape1->GetEntity();
		PhysicsEntity* e2 = shape2->GetEntity();
		if(e1 && e2)
		{
			e1->CollideCallback(e2);
			e2->CollideCallback(e1);
		}
	}

    // generate collide sounds
    // FIXME: not really ready for prime time 
    // TODO: implement roll / slide sounds (sounds that stop as soon as the contact is gone)
    //       roll / slide sounds also need to consider relative velocity
    //
    // 22-Jul-06    floh    optimized and rewritten for nHashTable
    if (numColls != 0)
    {
        // check if a sound is defined between the 2 materials
        const Util::String& soundName = Physics::MaterialTable::GetCollisionSound(shape1->GetMaterialType(), shape2->GetMaterialType());
        if (soundName.IsValid())
        {
            RigidBody* rigid1 = shape1->GetRigidBody();
            RigidBody* rigid2 = shape2->GetRigidBody();
            if ((rigid1 && rigid1->IsEnabled()) || (rigid2 && rigid2->IsEnabled()))
            {
                // compute sound volume
                Math::vector normal(contact[0].geom.normal[0], contact[0].geom.normal[1], contact[0].geom.normal[1]);
                Math::vector velocity = 
                    (0 != rigid1 ? rigid1->GetLinearVelocity() : Math::vector(0.0f, 0.0f, 0.0f)) - 
                    (0 != rigid2 ? rigid2->GetLinearVelocity() : Math::vector(0.0f, 0.0f, 0.0f));
                float volume = n_saturate((vector::dot3(velocity * -1, normal) - 0.3f) / 4.0f);
                if (volume > 0.0f) 
                {
                    // check if the sound has been played shortly before
                    Shape* keyData[2];
                    if (shape1 < shape2)
                    {
                        keyData[0] = shape1;
                        keyData[1] = shape2;
                    }
                    else
                    {
                        keyData[0] = shape2;
                        keyData[1] = shape1;
                    }
                    Util::Blob key(keyData, sizeof(keyData));
                    Timing::Time now = PhysicsServer::Instance()->GetTime();
                    bool soundWasPlayedBefore = level->collisionSounds.Contains(key);
                    bool playSound = true;
                    if (soundWasPlayedBefore)
                    {

                        Timing::Time& lastPlayTimeRef = level->collisionSounds[key];
                        playSound = (now - lastPlayTimeRef) > 0.25;
                        lastPlayTimeRef = now;
                    }
                    else
                    {
                        // sound wasn't played before, create a new entry in the hash table
                        playSound = true;
                        level->collisionSounds.Add(key, now);
                    }

                    // play collision sound effect
                    /*if (playSound)
                    {
                        Audio::Server* audioServer = Audio::Server::Instance();
                        Math::vector pos(contact[0].geom.pos[0], contact[0].geom.pos[1], contact[0].geom.pos[1]);
                        audioServer->PlaySoundEffect(soundName, pos, velocity, volume);
                    }*/
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Trigger the ODE simulation. This method should be called frequently
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
    // catch time exceptions
    Timing::Time timeDiff = this->time - this->simTimeStamp;
    if (timeDiff < 0.0)
    {
        this->simTimeStamp = this->time;
    }
    else if (timeDiff > 0.5f)
    {
        this->simTimeStamp = this->time;
    }

    // invoke the "on-frame-before" methods
    //PROFILER_START(this->profFrameBefore);
    int numEntities = this->GetNumEntities();
    int entityIndex;
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        this->GetEntityAt(entityIndex)->OnFrameBefore();
    }
    //PROFILER_STOP(this->profFrameBefore);

    //PROFILER_RESET(this->profStepBefore);
    //PROFILER_RESET(this->profStepAfter);
    //PROFILER_RESET(this->profCollide);
    //PROFILER_RESET(this->profStep);
    //PROFILER_RESET(this->profJointGroupEmpty);
    
    #ifdef __NEBULA_STATS__
    this->statsNumNearCallbackCalled = 0;
    this->statsNumCollideCalled = 0;
    this->statsNumCollided = 0;
    this->statsNumSpaceCollideCalled = 0;
    this->statsNumSteps = 0;
    #endif

    // step simulation until simulated time is present
    this->simSteps = (int) ((this->time - this->simTimeStamp) / this->stepSize);
    while (this->simTimeStamp < this->time)
    {    
        // invoke the "on-step-before" methods
        //PROFILER_STARTACCUM(this->profStepBefore);
        int entityIndex;
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            this->GetEntityAt(entityIndex)->OnStepBefore();
        }
        //PROFILER_STOPACCUM(this->profStepBefore);

        // do collision detection
        //PROFILER_STARTACCUM(this->profCollide);
        //this->statsNumSpaceCollideCalled++;
        // collide the dynamic space against the static space
        dSpaceCollide2((dGeomID)this->odeDynamicSpaceId, (dGeomID) this->odeStaticSpaceId, this, &OdeNearCallback);
        // collide the dynamic space against itself
        dSpaceCollide(this->odeDynamicSpaceId, this, &OdeNearCallback);
        //PROFILER_STOPACCUM(this->profCollide);

        // step physics simulation
        //PROFILER_STARTACCUM(this->profStep);
        dWorldQuickStep(this->odeWorldId, dReal(this->stepSize));
        //PROFILER_STOPACCUM(this->profStep);

        // clear contact joints
        //PROFILER_STARTACCUM(this->profJointGroupEmpty);
        dJointGroupEmpty(this->contactJointGroup);
        //PROFILER_STOPACCUM(this->profJointGroupEmpty);

        // invoke the "on-step-after" methods
        //PROFILER_STARTACCUM(this->profStepAfter);
        for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
        {
            this->GetEntityAt(entityIndex)->OnStepAfter();
        }        
        //PROFILER_STOPACCUM(this->profStepAfter);

        //this->statsNumSteps++;
        this->simTimeStamp += this->stepSize;
    }
    
    // export statistics
    #ifdef __NEBULA_STATS__
    nWatched watchSpaceCollideCalled("statsMangaPhysicsSpaceCollideCalled", nArg::Int);
    nWatched watchNearCallbackCalled("statsMangaPhysicsNearCallbackCalled", nArg::Int);
    nWatched watchCollideCalled("statsMangaPhysicsCollideCalled", nArg::Int);
    nWatched watchCollided("statsMangaPhysicsCollided", nArg::Int);
    nWatched watchSpaces("statsMangaPhysicsSpaces", nArg::Int);
    nWatched watchShapes("statsMangaPhysicsShapes", nArg::Int);
    nWatched watchSteps("statsMangaPhysicsSteps", nArg::Int);
    if (statsNumSteps > 0)
    {
        watchSpaceCollideCalled->SetI(this->statsNumSpaceCollideCalled/this->statsNumSteps);
        watchNearCallbackCalled->SetI(this->statsNumNearCallbackCalled/this->statsNumSteps);
        watchCollideCalled->SetI(this->statsNumCollideCalled/this->statsNumSteps);
        watchCollided->SetI(this->statsNumCollided/this->statsNumSteps);
    }
    watchSpaces->SetI(this->statsNumSpaces);
    watchShapes->SetI(this->statsNumShapes);
    watchSteps->SetI(this->statsNumSteps);
    #endif

    // invoke the "on-frame-after" methods
    //PROFILER_START(this->profFrameAfter);
    for (entityIndex = 0; entityIndex < numEntities; entityIndex++)
    {
        this->GetEntityAt(entityIndex)->OnFrameAfter();
    }
    //PROFILER_STOP(this->profFrameAfter);
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

//------------------------------------------------------------------------------
/**
*/
dSpaceID
Level::GetScratchSpaceSpaceID(unsigned int scrachSpaceId) const
{
    n_assert(this->scrachSpaces.Contains(scrachSpaceId));
    return this->scrachSpaces[scrachSpaceId];
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Level::CreateScratchSpace()
{
    this->scrachSpaceID++;
    this->scrachSpaces.Add(this->scrachSpaceID, dSimpleSpaceCreate(0));
    return this->scrachSpaceID;
}

//------------------------------------------------------------------------------
/**
*/
void
Level::RemoveScratchSpace(unsigned int scrachSpaceId)
{
    n_assert(0 < scrachSpaceId);

    IndexT index = this->scrachSpaces.FindIndex(scrachSpaceId);
    n_assert(0 <= index);

    dSpaceID space = this->scrachSpaces.ValueAtIndex(index);
    dSpaceDestroy(space);

    this->scrachSpaces.EraseAtIndex(index);
}

} // namespace Physics