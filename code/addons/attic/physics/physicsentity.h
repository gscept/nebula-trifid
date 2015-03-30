#ifndef PHYSICS_PHYSICSENTITY_H
#define PHYSICS_PHYSICSENTITY_H
//------------------------------------------------------------------------------
/**
    @class Physics::PhysicsEntity

    A physics entity is the frontend to a physics simulation object.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "math/vector.h"
#include "math/point.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Level;
class Composite;
class Shape;

class PhysicsEntity;

class CollisionReceiver 
{
public:
	virtual void doCollide(PhysicsEntity* collidee)=0;
};

class PhysicsEntity : public Core::RefCounted
{
	__DeclareClass(PhysicsEntity);
public:
    /// an entity id
    typedef unsigned int Id;
    /// constructor
    PhysicsEntity();
    /// destructor
    virtual ~PhysicsEntity();

    /// render the debug visualization of the entity
    virtual void RenderDebug();
    /// get the entity's unique id
    Id GetUniqueId() const;
    /// set the resource name for this object
    void SetResourceName(const Util::String& name);
    /// get the resource name for this object
    const Util::String& GetResourceName() const;
    /// set direct composite pointer
    void SetComposite(Composite* c);
    /// get composite pointer of this physics entity
    Composite* GetComposite() const;

    /// called when attached to game entity
    virtual void OnActivate();
    /// called when removed from game entity
    virtual void OnDeactivate();
    /// return true if active 
    bool IsActive() const;
    /// called when attached to physics level
    virtual void OnAttachedToLevel(Level* level);
    /// called when removed from physics level
    virtual void OnRemovedFromLevel();
    /// return true if attached to level
    bool IsAttachedToLevel() const;
    /// invoked before a simulation step is taken
    virtual void OnStepBefore();
    /// invoked after a simulation step is taken
    virtual void OnStepAfter();
    /// invoked before stepping the simulation
    virtual void OnFrameBefore();
    /// invoked after a stepping the simulation
    virtual void OnFrameAfter();
    /// called on collision; decide if it is valid
    virtual bool OnCollide(Shape* collidee);
	/// called after collision
	virtual void CollideCallback(PhysicsEntity* collidee);

    /// enable/disable the physics entity, re-enables itself on contact
    void SetEnabled(bool b);
    /// get enabled state of the physics entity
    bool IsEnabled() const;
    /// lock this entity in place, will never re-enable itself automatically
    void Lock();
    /// unlock the entity again
    void Unlock();
    /// is this entity locked?
    bool IsLocked() const;
    /// set the entity to "alive", this will actually attach/remove it from the world
    void SetAlive(bool b);
    /// return true if the entity is "visible"
    bool IsAlive() const;

    /// get level currently attached to
    Level* GetLevel() const;
    /// set the current world space transformation
    virtual void SetTransform(const Math::matrix44& m);
    /// get the current world space transformation
    virtual Math::matrix44 GetTransform() const;
    /// return true if transformation has changed between OnFrameBefore() and OnFrameAfter()
    virtual bool HasTransformChanged() const;
    /// get the current world space velocity
    virtual Math::vector GetVelocity() const;
    /// set user data, this is a dirty way to get this physics entity's game entity
    void SetUserData(int d);
    /// get user data
    int GetUserData() const;
    /// reset the velocity and force accumulators of the entity
    void Reset();
    /// set temporary stamp value
    void SetStamp(uint s);
    /// get temporary stamp value
    uint GetStamp() const;
    /// get the number of collisions in the last frame
    int GetNumCollisions() const;
    /// get the number of collisions with static objects (environment) in the last frame
    int GetNumStaticCollisions() const;
    /// get the number of collisions with dynamic objects in the last frame
    int GetNumDynamicCollisions() const;

    /// apply a global impulse vector at the next time step at a global position
    void ApplyImpulseAtPos(const Math::vector& impulse, const Math::point& pos, bool multByMass = false);
	
	CollisionReceiver* GetCollRec() { return collRec; }
	void SetCollRec(CollisionReceiver * val) { collRec = val; }

protected:
    Util::String resourceName;
    Ptr<Composite> composite;           // the composite of this entity
    bool active;                        // currently between Activate()/Deactivate()
    bool locked;                        // entity currently locked?
    Math::matrix44 lockedTransform;           // backup transform matrix when locked
    Level* level;                       // currently attached to this level
    Math::matrix44 transform;                 // the backup transform matrix
    Id uniqueId;
    static Id uniqueIdCounter;
    int userData;
    uint stamp;
    //Util::Array<Ptr<Shape> > collidedShapes; -- DEADCODE??!?!?!
    int numCollisions;
    int numDynamicCollisions;
    int numStaticCollisions;
	CollisionReceiver* collRec;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysicsEntity::GetNumCollisions() const
{
    return this->numCollisions;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysicsEntity::GetNumStaticCollisions() const
{
    return this->numStaticCollisions;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
PhysicsEntity::GetNumDynamicCollisions() const
{
    return this->numDynamicCollisions;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
PhysicsEntity::IsActive() const
{
	return this->active;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
PhysicsEntity::IsAttachedToLevel() const
{
    return (0 != this->level);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysicsEntity::SetStamp(uint s)
{
    this->stamp = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
PhysicsEntity::GetStamp() const
{
    return this->stamp;
}

//------------------------------------------------------------------------------
/**
    Set the resource name of the entity which will be loaded in
    OnActivate().
*/
inline
void
PhysicsEntity::SetResourceName(const Util::String& name)
{
    this->resourceName = name;
}

//------------------------------------------------------------------------------
/**
    Get the composite name of the entity.
*/
inline
const Util::String&
PhysicsEntity::GetResourceName() const
{
    return this->resourceName;
}

//------------------------------------------------------------------------------
/**
    Get the entity's unique id. Use the physics server's FindEntityByUniqueId()
    method to lookup an entity by its unique id.
*/
inline
PhysicsEntity::Id
PhysicsEntity::GetUniqueId() const
{
    return this->uniqueId;
}

//------------------------------------------------------------------------------
/**
    Set the user data of the entity. This is a not-quite-so-nice way to 
    store the game entity's unique id in the physics entity.
*/
inline
void
PhysicsEntity::SetUserData(int d)
{
    this->userData = d;
}

//------------------------------------------------------------------------------
/**
    Get the user data associated with this physics entity.
*/
inline
int
PhysicsEntity::GetUserData() const
{
    return this->userData;
}

//------------------------------------------------------------------------------
/**
    Get locked state of entity.
*/
inline
bool
PhysicsEntity::IsLocked() const
{
    return this->locked;
}

//------------------------------------------------------------------------------
/**
    Return pointer to level object which the physics entity is currently
    attached to.

    @return     pointer to Physics::Level object
*/
inline
Level*
PhysicsEntity::GetLevel() const
{
    return this->level;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
