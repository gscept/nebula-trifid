#ifndef PHYSICS_COMPOSITE_H
#define PHYSICS_COMPOSITE_H
//------------------------------------------------------------------------------
/**
    @class Physics::Composite

    A Physics::Composite contains one or several rigid bodies, connected
    by joints. There is one master body defined in the pool which receives
    and delivers positions and forces from/to the outside world. Composites
    are usually constructed once and not altered (i.e. you cannot remove
    bodies from the composite). The master body will always be the body at index
    0 (the first body added to the group).

    Composites may also contain optional static shapes. These are pure collide
    shapes which will move with the composite but will not act physically.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "util/array.h"
#include "util/string.h"
#include "Physics/rigidbody.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Joint;
class PhysicsEntity;

class Composite : public Core::RefCounted
{
	__DeclareClass(Composite);
public:
    /// constructor
    Composite();
    /// destructor
    virtual ~Composite();
    /// set composite name
    void SetName(const Util::String& n);
    /// get composite name
    const Util::String& GetName() const;
    /// attach the composite to the world
    virtual void Attach(btDynamicsWorld* world, const Math::matrix44 & m_);
    /// detach the composite from the world
    virtual void Detach();
    /// return true if currently attached to world
    bool IsAttached() const;
    /// set position and orientation.
    void SetTransform(const Math::matrix44& m);
    /// get position and orientation.
    const Math::matrix44& GetTransform() const;
    /// return true if transformation has changed during frame
    bool HasTransformChanged() const;
        /// add a rigid body to the composite (incs refcount)
    virtual void AddBody(RigidBody* body);
    
	// resizes internal arrays to store desired amount of bodies.
	// needs to be done before adding any bodies, can be done only once!
	virtual void SetSizes(int numBodies, int numJoints);
    /// get number of rigid bodies in the composite
    int GetNumBodies() const;
    /// get pointer to rigid body at index
    RigidBody* GetBodyAt(int index) const;
    /// find rigid body by its unique id
    RigidBody* FindBodyByUniqueId(uint id) const;
    /// does body with name `name' exist?
    bool HasBodyWithName(const Util::String& name) const;
    /// Body with name `name'.
    RigidBody* GetBodyByName(const Util::String& name) const;
    
	/// select the master body
	void SetMasterBody(const Util::String & name);
    
	virtual void AddJoint(Joint* joint);
    
    /// get number of joints in composite
    int GetNumJoints() const;
    /// get pointer to joint at index
    Joint* GetJointAt(int index) const;
    /// get pointer to the master body (first body added)
    RigidBody* GetMasterBody() const;
    /// called before simulation step is taken
    void OnStepBefore();
    /// called after simulation step is taken
    void OnStepAfter();
    /// called before physics frame is taken
    void OnFrameBefore();
    /// called after simulation frame is taken
    void OnFrameAfter();
    /// enable/disable the composite
    void SetEnabled(bool b);
    /// get enabled state of the composite
    bool IsEnabled() const;
    /// set the entity we are attached to
    void SetEntity(PhysicsEntity* e);
    /// get the entity we are attached to
    PhysicsEntity* GetEntity() const;
    /// reset the velocity and force accumulators of the rigid bodies
    void Reset();
    /// render the debug visualization
    void RenderDebug();

protected:
    /// clear rigid bodies
    void ClearBodies();
    /// clear joints
    void ClearJoints();
    
	Util::String name;
    Math::matrix44 frameBeforeTransform;
    Math::matrix44 transform;
    bool transformChanged;
    bool transformWasSet;
    Util::FixedArray<Ptr<RigidBody> > bodyArray;
    Util::FixedArray<Ptr<Joint> > jointArray;    
	int masterBodyIndex;
    int curBodyIndex;
    int curJointIndex;    
    PhysicsEntity* entity;    
    bool isAttached;
    btDynamicsWorld * dynamicsWorld;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Composite::SetName(const Util::String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
Composite::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
RigidBody*
Composite::GetMasterBody() const
{
    if (this->bodyArray.Size() > 0)
    {
        return this->bodyArray[this->masterBodyIndex];
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44&
Composite::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Composite::IsAttached() const
{
    return this->isAttached;
}

//------------------------------------------------------------------------------
/**
*/
inline
PhysicsEntity*
Composite::GetEntity() const
{
    return this->entity;
}

//------------------------------------------------------------------------------
/**
    Returns number of bodies in composite.
*/
inline
int
Composite::GetNumBodies() const
{
    return this->bodyArray.Size();
}

//------------------------------------------------------------------------------
/**
    Returns pointer to rigid body at index.
*/
inline
RigidBody*
Composite::GetBodyAt(int index) const
{
    return this->bodyArray[index];
}

//------------------------------------------------------------------------------
/**
    Returns number of joints in composite.
*/
inline
int
Composite::GetNumJoints() const
{
    return this->jointArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return pointer to joint at index.
*/
inline
Joint*
Composite::GetJointAt(int index) const
{
    return this->jointArray[index];
}


//------------------------------------------------------------------------------
/**
    Return true if transformation has changed during frame.
*/
inline
bool
Composite::HasTransformChanged() const
{
    return this->transformChanged;
}

}; // namespace Physics

//------------------------------------------------------------------------------
#endif
