//------------------------------------------------------------------------------
//  physics/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "Physics/physicsserver.h"
#include "Physics/debugdrawer.h"
#include "Physics/level.h"
#include "Physics/rigidbody.h"
#include "Physics/mousegripper.h"
#include "Physics/shape.h"
#include "io/ioserver.h"
#include "util/string.h"
#include "coregraphics/shaperenderer.h"
#include "core/factory.h"
#include "Physics/conversion.h"
#include <typeinfo.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

namespace Physics
{
__ImplementClass(Physics::PhysicsServer, 'PSR2', Core::RefCounted);
__ImplementSingleton(Physics::PhysicsServer);

using namespace Math;

uint PhysicsServer::UniqueStamp = 0;

//------------------------------------------------------------------------------
/**
*/
PhysicsServer::PhysicsServer() :
    isOpen(false),
    time(0.0),
    contactPoints(256, 256)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
PhysicsServer::~PhysicsServer()
{
    n_assert(!this->isOpen);
    n_assert(this->curLevel == 0);
    
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
btDiscreteDynamicsWorld *
PhysicsServer::GetBulletWorld() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetBulletWorld();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsServer::RegisterEntity(PhysicsEntity* entity)
{
	n_assert(entity)
		this->entityRegistry.Add(entity->GetUniqueId(), entity);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsServer::UnregisterEntity(PhysicsEntity* entity)
{
	n_assert(entity);
	this->entityRegistry.Erase(entity->GetUniqueId());
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
PhysicsServer::SetLevel(Level* level)
{
    n_assert(this->isOpen);
    if (this->curLevel != 0)
    {
        this->curLevel->OnDeactivate();
        this->curLevel = 0;
    }
    if (level)
    {
        this->curLevel = level;
        this->curLevel->OnActivate();
    }
}

//------------------------------------------------------------------------------
/**
    Return pointer to current level.

    @return     pointer to a Physics::Level object
*/
Level*
PhysicsServer::GetLevel() const
{
    return this->curLevel.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Initialize the physics subsystem.

    - 25-May-05 jo   Don't create default physics level.

    @return     true if physics subsystem initialized successfully
*/
bool
PhysicsServer::Open()
{
    n_assert(!this->isOpen);

    // setup the material table
    //MaterialTable::Setup();

    
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the physics subsystem.
*/
void
PhysicsServer::Close()
{
    n_assert(this->isOpen);
    this->SetLevel(0);    
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Perform one or more simulation steps. The number of simulation steps
    performed depends on the time of the last call to Trigger().
*/
void
PhysicsServer::Trigger()
{
    n_assert(this->isOpen);
    if (this->curLevel != 0)
    {
        // trigger the level
        this->curLevel->SetTime(this->time);
        this->curLevel->Trigger();
    }
}

//------------------------------------------------------------------------------
/**
    Set the current point of interest for the physics subsystem. This can
    be for instance the position of the game entity which has the input focus.
    Only the area around this point of interest should be simulated.
*/
void
PhysicsServer::SetPointOfInterest(const Math::vector& p)
{
    n_assert(this->curLevel != 0);
    this->curLevel->SetPointOfInterest(p);
}

//------------------------------------------------------------------------------
/**
    Get the current point of interest.
*/
const Math::vector&
PhysicsServer::GetPointOfInterest() const
{
    n_assert(this->curLevel != 0);
    return this->curLevel->GetPointOfInterest();
}

bool PhysicsServer::RayBundleCheck(const Math::vector& from, const Math::vector& to, const Math::vector& upVec, const Math::vector& leftVec, float bundleRadius, const FilterSet& excludeSet, float& outContactDist)
{
	Math::vector stabVector = to - from;
	Math::vector offset;

	outContactDist = Math::vector(to - from).length();
	bool contact = false;
	float distance = 0.0f;
	int i;
	for (i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0: offset = upVec * bundleRadius; break;
		case 1: offset = upVec * -bundleRadius; break;
		case 2: offset = leftVec * bundleRadius; break;
		case 3: offset = leftVec * -bundleRadius; break;
		default: break;
		}

		// do ray check
		Util::Array<Ptr<Physics::ContactPoint> > contacts = RayCheck(from + offset, from + offset + stabVector, excludeSet,Closest);

		// collided?
		
		IndexT j;
		for (j = 0; j < contacts.Size(); j++)
		{
			distance = Math::vector(contacts[j]->GetPosition() - from).length();

			// Stay away as far as possible
			if (distance < outContactDist)
			{
				outContactDist = distance;
				contact = true;
			}
		}
	}
	return contact;
}
//------------------------------------------------------------------------------
/**
    Do a ray check starting from position `fromPos' to position 'toPos'
	all in world coordinates
*/
Util::Array<Ptr<ContactPoint> >
PhysicsServer::RayCheck(const Math::vector& fromPos, const Math::vector& toPos, const FilterSet& excludeSet, RayTestType rayType)
{
    const static Math::matrix44 identity = Math::matrix44::identity();
    this->contactPoints.Clear();
	
	// setup filter
	short int filter = btBroadphaseProxy::AllFilter;
	short int excludes = excludeSet.GetCollideBits();
	filter &= ~excludes;

	btDiscreteDynamicsWorld *world = this->GetBulletWorld();
	n_assert(world);
	Util::Array<Ptr<ContactPoint> >points;
	btVector3 fromBt = Neb2BtVector(fromPos);
	btVector3 toBt = Neb2BtVector(toPos);
	switch(rayType)
	{
	case All:
		{			
			btCollisionWorld::AllHitsRayResultCallback cb(fromBt,toBt);
			cb.m_collisionFilterMask = filter;
			world->rayTest(fromBt,toBt,cb);
			if(cb.hasHit())
			{
				for(int i=0;cb.m_hitPointWorld.size();i++)
				{
					Math::vector v = Bt2NebVector(cb.m_hitPointWorld[i]);
					Math::vector n = Bt2NebVector(cb.m_hitNormalWorld[i]);
					Ptr<ContactPoint> p = ContactPoint::Create();
					p->SetPosition(v);
					p->SetNormalVector(n);
					p->SetType(ContactPoint::RayCheck);									
					if(typeid(*(cb.m_collisionObjects[i])) == typeid(btRigidBody))
					{
						void * bp = cb.m_collisionObjects[i]->getUserPointer();
						if(bp)
							p->SetRigidBody((RigidBody*)bp);
						else
							p->SetRigidBody(NULL);
					}else{
						p->SetRigidBody(NULL);
					}
					points.Append(p);
				}
			}
		}
		break;
	case Closest:
		{
			btCollisionWorld::ClosestRayResultCallback cb(fromBt,toBt);	
			cb.m_collisionFilterMask = filter;
			world->rayTest(fromBt,toBt,cb);
			if(cb.hasHit())
			{
				Math::vector v = Bt2NebVector(cb.m_hitPointWorld);
				Math::vector n = Bt2NebVector(cb.m_hitNormalWorld);
				Ptr<ContactPoint> p = ContactPoint::Create();
				p->SetPosition(v);
				p->SetNormalVector(n);
				p->SetType(ContactPoint::RayCheck);
				if(typeid(*cb.m_collisionObject) == typeid(btRigidBody))
				{
					void * bp = cb.m_collisionObject->getUserPointer();
					if(bp)
						p->SetRigidBody((RigidBody*)bp);
					else
						p->SetRigidBody(NULL);
				}else{
					p->SetRigidBody(NULL);
				}
				points.Append(p);
			}
		}		
	}
    return points;
}

//------------------------------------------------------------------------------
/**
    Create a new fully initialized box shape object.

    @param  m           locale transformation of shape
    @param  matType     MaterialType of the shape
    @param  size        size of the box
    @return             pointer to new box shape object
*/
Shape*
PhysicsServer::CreateBoxShape(const Math::matrix44& m, MaterialType matType, const Math::vector& size, bool isGhost) const
{
	// first remove scaling from transformation matrix
	matrix44 pure = matrix44::identity();
	pure.set_position(m.get_position());
	pure.set_xaxis(float4::normalize(m.get_xaxis()));
	pure.set_yaxis(float4::normalize(m.get_yaxis()));
	pure.set_zaxis(float4::normalize(m.get_zaxis()));

    Shape* boxShape = Shape::Create();
	
	btCollisionObject * coll;
	if(isGhost)
		coll = n_new(btPairCachingGhostObject);
	else
		coll = n_new(btCollisionObject);
	coll->setWorldTransform(Neb2BtM44Transform(pure));
	btVector3 halfSize = Neb2BtVector(size);
	halfSize *= 0.5f;
	btCollisionShape * shape = (btCollisionShape *)n_new(btBoxShape(halfSize));
	coll->setCollisionShape(shape);
	boxShape->SetInternalObject(Shape::Box,coll);
    return boxShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized sphere shape object.

    @param  m           locale transformation of shape
    @param  matType     MaterialType of the shape
    @param  radius      radius of sphere
    @return             pointer to new sphere shape object    
*/
Shape*
PhysicsServer::CreateSphereShape(const Math::matrix44& m, MaterialType matType, float radius, bool isGhost) const
{
    Shape* sphereShape = Shape::Create();
	btCollisionObject * coll;
	
	if(isGhost)
		coll = n_new(btPairCachingGhostObject);
	else
		coll = n_new(btCollisionObject);

	coll->setWorldTransform(Neb2BtM44Transform(m));	
	btCollisionShape * shape = n_new(btSphereShape(radius));
	coll->setCollisionShape(shape);
	sphereShape->SetInternalObject(Shape::Sphere,coll);    
    return sphereShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized capsule shape object.

    @param  m           locale transform of shape
    @param  matType     MaterialType of shape
    @param  radius      radius of capsule
    @param  length      lenght of capsule (not counting the caps)
    @return             pointer to a new CapsuleShape object
*/
Shape*
PhysicsServer::CreateCapsuleShape(const Math::matrix44& m, MaterialType matType, float radius, float length, bool isGhost) const
{
    Shape* capsuleShape = Shape::Create();
	btCollisionObject * coll;
	if(isGhost)
		coll = n_new(btPairCachingGhostObject);
	else
		coll = n_new(btCollisionObject);
	coll->setWorldTransform(Neb2BtM44Transform(m));	
	btCollisionShape * shape = n_new(btCapsuleShape(radius,length));
	coll->setCollisionShape(shape);
	capsuleShape->SetInternalObject(Shape::Capsule,coll);        
    return capsuleShape;
}


//------------------------------------------------------------------------------
/**
    Shoots a 3d ray into the world and returns the closest contact.

    @param  pos         starting position of ray
    @param  dir         direction and length of ray
    @param  exludeSet   filter set defining objects to exclude
    @return             pointer to closest ContactPoint, or 0 if no contact detected
*/
Ptr<ContactPoint>
PhysicsServer::GetClosestContactAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet)
{
    // do the actual ray check (returns all contacts)
    Util::Array<Ptr<ContactPoint> > contacts = this->RayCheck(pos, pos + dir, excludeSet,Closest);

    int closestContactIndex = -1;
    float closestDistance = dir.length();;
    int i;
    int numContacts = contacts.Size();
    Math::vector distVec;
    for (i = 0; i < numContacts; i++)
    {
        Ptr<ContactPoint> curContact = contacts[i];
        distVec = curContact->GetPosition() - pos;
        float dist = distVec.length();
        if (dist < closestDistance)
        {
            closestContactIndex = i;
            closestDistance = dist;
        }
    }
    if (closestContactIndex != -1)
    {
        return contacts[closestContactIndex];
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Shoots a 3d ray through the current mouse position and returns the
    closest contact, or a null pointer if no contact.
    NOTE: This gets the current view matrix from the Nebula gfx server.
    This means the check could be one frame of, if the "previous" view matrix
    is used.

    @param  worldMouseRay   Mouse ray in world space
    @param  excludeSet      filter set defining which objects to exclude
    @return                 pointer to closest ContactPoint or 0 if no contact detected
*/
Ptr<ContactPoint>
PhysicsServer::GetClosestContactUnderMouse(const Math::line& worldMouseRay, const FilterSet& excludeSet)
{
    return this->GetClosestContactAlongRay(worldMouseRay.start(), worldMouseRay.vec(), excludeSet);
}

//------------------------------------------------------------------------------
/**
    Apply an impulse on the first rigid body which lies along the defined ray.
*/
bool
PhysicsServer::ApplyImpulseAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, float impulse)
{
    const ContactPoint* contactPoint = this->GetClosestContactAlongRay(pos, dir, excludeSet);
    if (contactPoint)
    {
        RigidBody* rigidBody = contactPoint->GetRigidBody();
        if (rigidBody)
        {
            Math::vector normDir = dir;
            normDir = vector::normalize(normDir);
            rigidBody->ApplyImpulseAtPos(contactPoint->GetPosition(), normDir * impulse);
            return true;
        }
    }
    return false;
}


//------------------------------------------------------------------------------
/**
    Renders the debug visualization of the level.
*/
void
PhysicsServer::RenderDebug()
{
    if (this->curLevel != 0)
    {
        this->curLevel->RenderDebug();
    }    
}

//------------------------------------------------------------------------------
/**
    This method returns all physics entities touching the given spherical 
    area. The method creates a sphere shape and calls its collide
    method, so it's quite fast. Note that entities will be appended to the
    array, so usually you should make sure to pass an empty array. This method
    will also overwrite the internal contactPoints array which can be 
    queried after the method has returned, but note that there will only
    be one contact per physics shape.

    @param  pos         center of the sphere
    @param  radius      radius of the sphere
    @param  excludeSet  what contacts should be ignored?
    @param  result      array which will be filled with entity pointers
    @return             number of entities touching the sphere
*/
int
PhysicsServer::GetEntitiesInSphere(const Math::vector& pos, float radius, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result)
{
    n_assert(radius >= 0.0f);
    n_assert(this->GetLevel());
	
    // create a sphere shape and perform collision check
    Math::matrix44 m = Math::matrix44::identity();
    m.translate(pos);
    Ptr<Shape> sphereShape = this->CreateSphereShape(m, MaterialTable::StringToMaterialType("Wood"), radius);
	
	GetEntitiesInShape(sphereShape->GetShape(),excludeSet,result);
	return result.Size();
    
/*	sphereShape->Attach(this->GetLevel()->GetOdeDynamicSpaceId());
    this->contactPoints.Reset();
    sphereShape->Collide(excludeSet, this->contactPoints);
    sphereShape->Detach();

    // convert contacts to unique entities
    uint stamp = this->GetUniqueStamp();
    int i;
    int numContacts = this->contactPoints.Size();
    for (i = 0; i < numContacts; i++)
    {
        PhysicsEntity* entity = this->contactPoints[i].GetPhysicsEntity();
        if (entity && (entity->GetStamp() != stamp))
        {
            entity->SetStamp(stamp);
            result.Append(entity);
        }
    }
    return result.Size() - oldResultSize;
	*/
}
//------------------------------------------------------------------------------
/**
    This method returns all physics entities touching the given box shaped 
    area. The method creates a box shape and calls its collide
    method, so it's quite fast. Note that entities will be appended to the
    array, so usually you should make sure to pass an empty array. This method
    will also overwrite the internal contactPoints array which can be 
    queried after the method has returned, but note that there will only
    be one contact per physics shape.

    @param  pos         center of box
    @param  scale       box scaling
    @param  excludeSet  what contacts should be ignored?
    @param  result      array which will be filled with entity pointers
    @return             number of entities touching the box
*/

int
PhysicsServer::GetEntitiesInBox(const Math::vector& scale, const Math::matrix44& m, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result)
{
    n_assert(scale.x() >= 0 && scale.y() >= 0 && scale.z() >= 0);
    n_assert(this->GetLevel());
    
    Ptr<Shape> boxShape = this->CreateBoxShape(m, MaterialTable::StringToMaterialType("Wood"), scale);

	GetEntitiesInShape(boxShape->GetShape(),excludeSet,result);
	return result.Size();
	/*
	boxShape->Attach(this->GetLevel()->GetOdeDynamicSpaceId());
    this->contactPoints.Reset();
    boxShape->Collide(excludeSet, this->contactPoints);
    boxShape->Detach();

    // convert contacts to unique entities
    uint stamp = this->GetUniqueStamp();
    int i;
    int numContacts = this->contactPoints.Size();
    for (i = 0; i < numContacts; i++)
    {
        PhysicsEntity* entity = this->contactPoints[i].GetPhysicsEntity();
        if (entity && (entity->GetStamp() != stamp))
        {
            entity->SetStamp(stamp);
            result.Append(entity);
        }
    }
    return result.Size() - oldResultSize;
	*/
}


struct ContactCallback : public btCollisionWorld::ContactResultCallback {
	
	//! Constructor, pass whatever context you want to have available when processing contacts
	/*! You may also want to set m_collisionFilterGroup and m_collisionFilterMask
	 *  (supplied by the superclass) for needsCollision() */
	ContactCallback(btCollisionObject& shape,Util::Array<Ptr<PhysicsEntity> >& result)
		: btCollisionWorld::ContactResultCallback(), testShape(shape),results(result) { }
	
	btCollisionObject & testShape;
	Util::Array<Ptr<PhysicsEntity> >& results;
		
	//! Called with each contact for your own processing (e.g. test if contacts fall in within sensor parameters)
	virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObject* colObj0,int partId0,int index0,
		const btCollisionObject* colObj1,int partId1,int index1)
	{
		
		const btCollisionObject * foo;
		if(colObj0==&testShape) {
			foo = colObj1;				
		} else {
			foo = colObj0;
		}
		RigidBody * body = (RigidBody*)foo->getUserPointer();
		if(body)
		{
			Ptr<PhysicsEntity> ent = body->GetEntity();
			if(ent.isvalid())
				results.Append(ent);
		}
		// do stuff with the collision point
		return 0; // not actually sure if return value is used for anything...?
	}
};

void PhysicsServer::AddFilterSet(const Util::String & name, FilterSet* filter)
{
	this->filterSets.Add(name,filter);
}

FilterSet *

PhysicsServer::GetFilterSet(const Util::String &name)
{
	n_assert2(this->filterSets.Contains(name),"unknown filterset, add it first\n");
	return this->filterSets.ValueAtIndex(this->filterSets.FindIndex(name));
}

void PhysicsServer::GetEntitiesInShape(btCollisionObject * shape, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result )
{
	//FIXME add support for filters (they are set in ContactCallback)
	ContactCallback results(*shape,result);
	// setup filter
	short int filter = btBroadphaseProxy::AllFilter;
	short int excludes = excludeSet.GetCollideBits();
	filter &= ~excludes;
	results.m_collisionFilterMask = filter;
	GetBulletWorld()->getCollisionWorld()->contactTest(shape,results);

}
//------------------------------------------------------------------------------
/**
    This method computes a ray in 3d space thru the mouse position.
    It uses the projection and view matrix from the cam of the view set.

    @param  mousePos    2d screen position of mouse
    @param  scale       length of ray cast into the 3d world
*/
//Math::line 
//PhysicsServer::ComputeWorldMouseRay(const float2& mousePos, float length)
//{
//    n_assert(Graphics::GraphicsServer::HasInstance());
//    n_assert(this->graphicsView.isvalid());
//    const Ptr<Graphics::CameraEntity>& cam = this->graphicsView->GetCameraEntity();
//    n_assert(cam.isvalid());
//    const matrix44& view = matrix44::inverse(cam->GetViewTransform());
//    matrix44 invProj = matrix44::inverse(cam->GetProjTransform());
//
//    return Graphics::GraphicsServer::Instance()->ComputeWorldMouseRay(mousePos, length, view, invProj, cam->GetNearClipPlane());
//}


} // namespace Physics