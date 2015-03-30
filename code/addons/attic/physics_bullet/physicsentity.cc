//------------------------------------------------------------------------------
//  physics/entity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "debugdrawer.h"
#include "btBulletDynamicsCommon.h"
#include "BulletMultiThreaded/btThreadSupportInterface.h"
#include "Physics/rigidbody.h"
#include "Physics/shape.h"
#include "Physics/level.h"
#include "physics/composite.h"
#include "Physics/physicsentity.h"
#include "physics/joint.h"
#include "debugrender/debugrender.h"
#include "io/filestream.h"
#include "bullet/bullet/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"
#include "physics/conversion.h"
#include "physics/filterset.h"

namespace Physics
{
__ImplementClass(Physics::PhysicsEntity, 'PEN2', Core::RefCounted);

uint PhysicsEntity::uniqueIdCounter = 1;

//------------------------------------------------------------------------------
/**
*/
PhysicsEntity::PhysicsEntity() :
    active(false),
    locked(false),
    level(0),
	loaded(false),	
	dummy(false),
    userData(0),
    stamp(0),
    numCollisions(0),
    numStaticCollisions(0),
    numDynamicCollisions(0),
    transform(Math::matrix44::identity()),
    lockedTransform(Math::matrix44::identity()),
	collRec(NULL),
	filterSet(NULL)
{
    this->uniqueId = uniqueIdCounter++;
    PhysicsServer::Instance()->RegisterEntity(this);
	this->body = Composite::Create();
}

//------------------------------------------------------------------------------
/**
*/
PhysicsEntity::~PhysicsEntity()
{
    n_assert(this->level == 0);
    if (this->active)
    {
        this->OnDeactivate();
    }
    PhysicsServer::Instance()->UnregisterEntity(this);
	this->body = 0;
}

bool
PhysicsEntity::LoadFromResource(bool dontFail)
{
	n_assert(!this->loaded);
	n_assert(!this->active);
	n_assert(!this->resourceName.IsEmpty());

	// open stream for reading
	Util::String path;
	path.Format("physics:%s.bullet", this->resourceName.AsCharPtr());    

	Ptr<IO::Stream> reader = IO::FileStream::Create();
	reader->SetURI(path);		
	if(reader->Open())
	{
		//FIXME, move this to physicsserver or level
		IO::Stream::Size size = reader->GetSize();
		char* buffer = n_new(char[size]);
		reader->Read(buffer,size);
		btBulletWorldImporter* fileLoader = new btBulletWorldImporter(0);
		fileLoader->loadFileFromMemory(buffer,size);

		if(fileLoader->getNumRigidBodies()>1)
		{				
			this->body->SetSizes(fileLoader->getNumRigidBodies(),fileLoader->getNumConstraints());
			for(int i = 0;i<fileLoader->getNumRigidBodies();i++)
			{
				btRigidBody * body = (btRigidBody*) fileLoader->getRigidBodyByIndex(i);
				Ptr<RigidBody> rb = RigidBody::Create();
				rb->SetInitialTransform(Bt2NebTransform(body->getWorldTransform()));
				rb->SetRigidBody(body);
				this->body->AddBody(rb);
				rb->SetName(fileLoader->getNameForPointer(body));
			}
			for(int i = 0;i<fileLoader->getNumConstraints();i++)
			{
				btTypedConstraint * con = (btTypedConstraint*) fileLoader->getConstraintByIndex(i);
				Ptr<Joint> rb = Joint::Create();
				rb->SetConstraint(con);
				rb->SetBodies((RigidBody*)con->getRigidBodyA().getUserPointer(),(RigidBody*)con->getRigidBodyB().getUserPointer());					
				this->body->AddJoint(rb);
				rb->SetLinkName(fileLoader->getNameForPointer(body));
			}

			this->body->SetEntity(this);
		}
		else
		{
			if(fileLoader->getNumRigidBodies() == 0)
			{
				// no rigid body, cant continue				
				n_error("Bullet file %s has no rigid body, cant continue",reader->GetURI().AsString());
			}
			btRigidBody * body = (btRigidBody*) fileLoader->getRigidBodyByIndex(0);
			this->body->SetSizes(1,0);
			Ptr<RigidBody> rb = RigidBody::Create();
			rb->SetInitialTransform(Bt2NebTransform(body->getWorldTransform()));
			rb->SetRigidBody(body);
			this->body->AddBody(rb);								
			this->body->SetEntity(this);
			rb->SetName(fileLoader->getNameForPointer(body));
		}						
		delete fileLoader;
		loaded = true;
	}
	else
	{
		if(dontFail)
		{
			btCollisionShape * shape = (btCollisionShape *)n_new(btBoxShape(btVector3(3,3,3)));
			this->body->SetSizes(1,0);
			Ptr<RigidBody> rb = RigidBody::Create();
			rb->SetShape(shape);
			rb->SetMass(1);
			this->body->AddBody(rb);								
			this->body->SetEntity(this);	
			loaded = true;
			dummy = true;
			rb->SetName("dummy");
		}		
	}
	return loaded;
}
//------------------------------------------------------------------------------
/**
    This method is called when the physics entity becomes active
    (i.e. when it is attached to a game entity).
*/
void
PhysicsEntity::OnActivate()
{
    n_assert(!this->active);
    if (this->body->GetNumBodies() > 0)
    {
        // composite directly set
        //this->body->SetTransform(this->transform);           
		this->body->SetEntity(this);
    }
    else if (!this->resourceName.IsEmpty())
    {
		if(!loaded)
			LoadFromResource(true);
    }
    else
    {
        //n_error("Physics::PhysicsEntity: no valid physics composite name given!");
		// dummy
//		btCollisionShape * shape = (btCollisionShape *)n_new(btBoxShape(btVector3(3,3,3)));
//		this->body = RigidBody::Create();
//		body->SetShape(shape);
//		body->SetMass(200);
    }

    this->active = true;
    this->numCollisions = 0;
    this->numStaticCollisions = 0;
    this->numDynamicCollisions = 0;
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity becomes inactive,
    (i.e. when it is detached from its game entity).
*/
void
PhysicsEntity::OnDeactivate()
{
    n_assert(this->active);
    this->SetAlive(false);    
    this->active = false;
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity is attached to a
    physics level object.

    @param  level   pointer to a level object
*/
void
PhysicsEntity::OnAttachedToLevel(Level* levl)
{
    n_assert(levl);
    n_assert(this->level == 0);
    this->level = levl;
	this->body->Attach(this->level->GetBulletWorld(),this->transform);
    this->SetEnabled(true);
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity is removed from
    its physics level object.
*/
void
PhysicsEntity::OnRemovedFromLevel()
{
    n_assert(this->level != 0);
    this->level = 0;
}

//------------------------------------------------------------------------------
/**
    Set the current transformation in world space. This method should
    only be called once at initialization time, since the main job
    of a physics object is to COMPUTE the transformation for a game entity.

    @param  m   a Math::matrix44 defining the world space transformation
*/
void
PhysicsEntity::SetTransform(const Math::matrix44& m)
{
    if (this->body != 0)
    {
        this->body->SetTransform(m);
    }
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
    Get the current transformation in world space. The transformation
    is updated during Physics::PhysicsServer::Trigger().

    @return     transformation of physics entity in world space
*/
Math::matrix44
PhysicsEntity::GetTransform() const
{
    if (this->body != 0)
    {
        return this->body->GetMasterBody()->GetTransform();
    }
    else
    {
        return this->transform;
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the transformation has changed during the frame.
*/
bool
PhysicsEntity::HasTransformChanged() const
{
    if (this->body != 0)
    {
        return this->body->GetMasterBody()->HasTransformChanged();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Get the current world space velocity.
*/
Math::vector
PhysicsEntity::GetVelocity() const
{
    if ((this->body != 0) )
    {
        return this->body->GetMasterBody()->GetLinearVelocity();
    }
    else
    {
        return Math::vector(0.0f, 0.0f, 0.0f);
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked before a simulation step is taken.
*/
void
PhysicsEntity::OnStepBefore()
{
    if (this->body != 0)
    {
        this->body->OnStepBefore();
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked after a simulation step is taken.
*/
void
PhysicsEntity::OnStepAfter()
{
    if (this->body != 0)
    {
        this->body->OnStepAfter();
    }

    // if we are locked, and have become enabled automatically,
    // disable again, and restore original position
    if (this->IsLocked())
    {
        if (this->IsEnabled())
        {
            this->SetTransform(this->lockedTransform);
            this->SetEnabled(false);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked before a physics frame starts (consisting of 
    several physics steps).
*/
void
PhysicsEntity::OnFrameBefore()
{
    if (this->body != 0)
    {
        this->body->OnFrameBefore();
    }
    this->numCollisions = 0;
    this->numStaticCollisions = 0;
    this->numDynamicCollisions = 0;
}

//------------------------------------------------------------------------------
/**
    This method is invoked after a physics frame is taken (consisting of 
    several physics steps).
*/
void
PhysicsEntity::OnFrameAfter()
{
    if (this->body != 0)
    {
        this->body->OnFrameAfter();
    }
}

//------------------------------------------------------------------------------
/**
    Reset the entity's velocity and force accumulators.
*/
void
PhysicsEntity::Reset()
{
    if (this->body != 0)
    {
        this->body->Reset();
    }
}

//------------------------------------------------------------------------------
/**
    Enable/disable the physics entity. A disabled entity will enable itself
    automatically on contact with other enabled entities.
*/
void
PhysicsEntity::SetEnabled(bool b)
{
    if (this->body != 0)
    {
        this->body->SetEnabled(b);
    }
}

//------------------------------------------------------------------------------
/**
    Get the enabled state of the physics entity.
*/
bool
PhysicsEntity::IsEnabled() const
{
    if (this->body != 0)
    {
        return this->body->IsEnabled();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Lock the entity. A locked entity acts like a disabled entity,
    but will never re-enable itself on contact with another entity.
*/
void
PhysicsEntity::Lock()
{
    n_assert(!this->locked);
    this->locked = true;
    this->lockedTransform = this->GetTransform();
    if (this->body.isvalid())
    {
        this->SetEnabled(false);
    }
}

//------------------------------------------------------------------------------
/**
    Unlock the entity. This will reset the entity (set velocity and forces 
    to 0), and place it on the position where it was when the entity was
    locked. 
*/
void
PhysicsEntity::Unlock()
{
    n_assert(this->locked);
    this->locked = false;
    this->SetTransform(this->lockedTransform);
    this->Reset();
	this->SetEnabled(true);
}

//------------------------------------------------------------------------------
/**
    Set direct composite pointer. This is optional to setting a name.
*/
void
PhysicsEntity::SetRigidBody(RigidBody* b)
{
	this->body->SetSizes(1,0);
	this->body->AddBody(b);    
    this->body->SetEntity(this);
    this->body->SetTransform(this->transform);
    if (this->locked)
    {
        this->body->SetEnabled(false);
    }
}

//------------------------------------------------------------------------------
/**
    Get pointer to composite object of this physics entity.
*/
RigidBody*
PhysicsEntity::GetRigidBody() const
{
    return this->body->GetMasterBody();
}


RigidBody*
PhysicsEntity::GetRigidBodyByName(const Util::String & name) const
{
	n_assert2(this->body->HasBodyWithName(name),"named body not found in entity");
	return this->body->GetBodyByName(name);
}


void
PhysicsEntity::SetShape(btCollisionShape* b)
{
	n_assert(body.isvalid());
	this->body->GetMasterBody()->SetShape(b);        
}

//------------------------------------------------------------------------------
/**
    Get pointer to composite object of this physics entity.
*/
btCollisionShape*
PhysicsEntity::GetShape() const
{
	n_assert(this->body.isvalid());
    return this->body->GetMasterBody()->GetShape();
}


//------------------------------------------------------------------------------
/**
    Render the debug visualization of this entity.
*/
void
PhysicsEntity::RenderDebug()
{
    if (this->body != 0)
    {
        this->body->RenderDebug();
        Util::String txt = "ID:";
        txt.Append(Util::String::FromInt(this->GetUniqueId()));
        Math::point pos = this->GetTransform().get_position();
        _debug_text3D(txt, pos, Math::float4(0,0,1,1));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsEntity::SetAlive(bool b)
{
    if (this->body.isvalid())
    {
        PhysicsServer* server = PhysicsServer::Instance();
        if (b && (!this->body->IsAttached()))
        {
            this->body->Attach(server->GetBulletWorld(),this->transform);
        }
        else if ((!b) && this->body->IsAttached())
        {
            this->body->Detach();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
PhysicsEntity::IsAlive() const
{
    if (this->body.isvalid())
    {
        return this->body->IsAttached();
    }
    else
    {
        return false;
    }
}

void
PhysicsEntity::CreateDummyBox(const Math::vector & size, float mass, const Util::String & name)
{
	Ptr<RigidBody> _body;
	btVector3 s = Neb2BtVector(size);	
	btCollisionShape * shape = (btCollisionShape *)n_new(btBoxShape(s));
	_body = RigidBody::Create();				
	_body->SetShape(shape);
	_body->SetMass(mass);
	SetRigidBody(_body);
	_body->SetName(name);
}

void 
PhysicsEntity::CreateFromGfx(const Util::Array<Ptr<Graphics::ModelEntity>> & gfxEntities)
{
	if(gfxEntities.Size() > 0)
	{	
		bool wasAlive = this->IsAlive();
		if(wasAlive)
		{
			this->transform = this->body->GetTransform();
			SetAlive(false);
		}

		Ptr<RigidBody> _body;		
		btCompoundShape * compound = n_new(btCompoundShape);
		for (int e = 0; e < gfxEntities.Size(); e++)
		{
			if(gfxEntities[e] != NULL && gfxEntities[e]->IsValid())
			{							
				Math::bbox _box = gfxEntities[e]->GetGlobalBoundingBox();
				btVector3 halfSize = Neb2BtVector(_box.size());
				halfSize *= 0.5f;
				btBoxShape* _boxShape = n_new(btBoxShape(halfSize));
				Math::matrix44 pos;
				pos.set_position(_box.to_matrix44().get_position());
				compound->addChildShape(Neb2BtM44Transform(pos),_boxShape);							
			}
		}
		_body = RigidBody::Create();
		_body->SetShape((btCollisionShape*)compound);					
		_body->SetMass(0);
		this->SetRigidBody(_body);
		if(wasAlive)
			this->SetAlive(true);
	}
}



bool
PhysicsEntity::OnCollide(RigidBody* collidee)
{	
	if(filterSet)
		return !this->filterSet->CheckBody(collidee);
	return true;
}

//------------------------------------------------------------------------------
/** 
    This method is called by the global collide callback to decide
    if a collision between this entity and a shape should be valid.
*/
bool
PhysicsEntity::OnCollide(Shape* collidee)
{	
	if(filterSet)
		return !this->filterSet->CheckShape(collidee);
	//FIXME
#if 0
	n_assert(0 != collidee);
    if (collidee->GetRigidBody())
    {
        // a dynamic collision
        this->numDynamicCollisions++;
    }
    else
    {
        // a collision with a static object
        this->numStaticCollisions++;
    }
    this->numCollisions++;
#endif
    return true;
}

void
PhysicsEntity::CollideCallback(Shape* collidee, Ptr<ContactPoint> contact)
{
	if(collRec !=NULL)
	{
		collRec->doCollideOther(collidee, contact);
	}
}
void
PhysicsEntity::CollideCallback(PhysicsEntity* collidee, Ptr<ContactPoint> contact)
{
	if(collRec !=NULL)
	{
		collRec->doCollideEntity(collidee, contact);					
	}
}

void
PhysicsEntity::SetFilterSet(FilterSet* filter)
{
	this->filterSet = filter;
}

//------------------------------------------------------------------------------
/**
    Apply an impulse vector at a position in the global coordinate frame.
*/
void
PhysicsEntity::ApplyImpulseAtPos(const Math::vector& impulse, const Math::point& pos, bool multByMass)
{
	n_assert(body.isvalid());
	
	body->GetMasterBody()->ApplyImpulseAtPos(impulse,pos,multByMass);
}

} // namespace Physics