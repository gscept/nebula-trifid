//------------------------------------------------------------------------------
//  physics/entity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/composite.h"
#include "physics/physicsentity.h"
#include "physics/level.h"
#include "physics/rigidbody.h"
#include "physics/shape.h"
#include "physics/compositeloader.h"
#include "debugrender/debugrender.h"


namespace Physics
{
__ImplementClass(Physics::PhysicsEntity, 'PENT', Core::RefCounted);

uint PhysicsEntity::uniqueIdCounter = 1;

//------------------------------------------------------------------------------
/**
*/
PhysicsEntity::PhysicsEntity() :
    active(false),
    locked(false),
    level(0),
    userData(0),
    stamp(0),
    numCollisions(0),
    numStaticCollisions(0),
    numDynamicCollisions(0),
    transform(Math::matrix44::identity()),
    lockedTransform(Math::matrix44::identity()),
	collRec(NULL)
{
    this->uniqueId = uniqueIdCounter++;
    PhysicsServer::Instance()->RegisterEntity(this);
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
    if (this->composite != 0)
    {
        // composite directly set
        this->composite->SetTransform(this->transform);   
        this->SetAlive(true);
    }
    else if (!this->resourceName.IsEmpty())
    {
        n_assert(this->composite == 0);

        // load and initialize composite object(s), the default
        // physics entity only works with one composite, we just
        // take the first in the physics.xml file
        CompositeLoader compLoader;
        Ptr<Composite> newComposite = compLoader.Load(this->resourceName)[0];
        newComposite->SetTransform(this->transform);    
        this->SetComposite(newComposite);
        this->SetAlive(true);
    }
    else
    {
        n_error("Physics::PhysicsEntity: no valid physics composite name given!");
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
    // NOTE: WE DON'T RELEASE THE COMPOSITE, INSTEAD WE KEEP
    // THE ENTITY FULLY INTACT HERE ON PURPOSE!!
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
    this->SetEnabled(false);
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
    if (this->composite != 0)
    {
        this->composite->SetTransform(m);
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
    if (this->composite != 0)
    {
        return this->composite->GetTransform();
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
    if (this->composite != 0)
    {
        return this->composite->HasTransformChanged();
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
    if ((this->composite != 0) && (this->composite->GetMasterBody()))
    {
        return this->composite->GetMasterBody()->GetLinearVelocity();
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
    if (this->composite != 0)
    {
        this->composite->OnStepBefore();
    }
}

//------------------------------------------------------------------------------
/**
    This method is invoked after a simulation step is taken.
*/
void
PhysicsEntity::OnStepAfter()
{
    if (this->composite != 0)
    {
        this->composite->OnStepAfter();
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
    if (this->composite != 0)
    {
        this->composite->OnFrameBefore();
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
    if (this->composite != 0)
    {
        this->composite->OnFrameAfter();
    }
}

//------------------------------------------------------------------------------
/**
    Reset the entity's velocity and force accumulators.
*/
void
PhysicsEntity::Reset()
{
    if (this->composite != 0)
    {
        this->composite->Reset();
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
    if (this->composite != 0)
    {
        this->composite->SetEnabled(b);
    }
}

//------------------------------------------------------------------------------
/**
    Get the enabled state of the physics entity.
*/
bool
PhysicsEntity::IsEnabled() const
{
    if (this->composite != 0)
    {
        return this->composite->IsEnabled();
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
    if (this->GetComposite())
    {
        this->SetEnabled(false);
    }
}

//------------------------------------------------------------------------------
/**
    Unlock the entity. This will reset the entity (set velocity and forces 
    to 0), and place it on the position where it was when the entity was
    locked. Note that the entity will NOT be enabled. This will happen
    automatically when necessary (for instance on contact with another
    active entity).
*/
void
PhysicsEntity::Unlock()
{
    n_assert(this->locked);
    this->locked = false;
    this->SetTransform(this->lockedTransform);
    this->Reset();
}

//------------------------------------------------------------------------------
/**
    Set direct composite pointer. This is optional to setting a name.
*/
void
PhysicsEntity::SetComposite(Composite* c)
{
    this->composite = c;
    this->composite->SetEntity(this);
    this->composite->SetTransform(this->transform);
    if (this->locked)
    {
        this->composite->SetEnabled(false);
    }
}

//------------------------------------------------------------------------------
/**
    Get pointer to composite object of this physics entity.
*/
Composite*
PhysicsEntity::GetComposite() const
{
    return this->composite.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of this entity.
*/
void
PhysicsEntity::RenderDebug()
{
    if (this->composite != 0)
    {
        this->composite->RenderDebug();
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
    if (this->composite.isvalid())
    {
        PhysicsServer* server = PhysicsServer::Instance();
        if (b && (!this->composite->IsAttached()))
        {
            this->composite->Attach(server->GetOdeWorldId(), server->GetOdeDynamicSpaceId(), server->GetOdeStaticSpaceId());
        }
        else if ((!b) && this->composite->IsAttached())
        {
            this->composite->Detach();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
PhysicsEntity::IsAlive() const
{
    if (this->composite.isvalid())
    {
        return this->composite->IsAttached();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    This method is called by the global collide callback to decide
    if a collision between this entity and a shape should be valid.
*/
bool
PhysicsEntity::OnCollide(Shape* collidee)
{
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
    return true;
}


void
PhysicsEntity::CollideCallback(PhysicsEntity* collidee)
{
	if(collidee != NULL)
	{
		if (userData!=0)
		{
			if(collRec !=NULL)
			{
 				collRec->doCollide(collidee);
			}
 		}
	}
}

//------------------------------------------------------------------------------
/**
    Apply an impulse vector at a position in the global coordinate frame.
*/
void
PhysicsEntity::ApplyImpulseAtPos(const Math::vector& impulse, const Math::point& pos, bool multByMass)
{
    // get the physics composite
    Ptr<Physics::Composite> curComposite = this->GetComposite();
    
    n_assert(composite.isvalid());

    // apply the impulse to all bodys
    int idxBody;
    for(idxBody = 0; idxBody < curComposite->GetNumBodies(); idxBody++)
    {
        Physics::RigidBody* body = curComposite->GetBodyAt(idxBody);
        body->ApplyImpulseAtPos(impulse, pos, multByMass);
    }

}

} // namespace Physics