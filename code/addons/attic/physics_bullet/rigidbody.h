#ifndef PHYSICS_RIGIDBODY_H
#define PHYSICS_RIGIDBODY_H
//------------------------------------------------------------------------------
/**
    @class Physics::RigidBody

    RigidBody is the internal base class for all types of rigid bodies.
    Subclasses of RigidBody implement specific shapes. RigidBodies can
    be connected by Joints to form a hierarchy.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "math/matrix44.h"
#include "math/vector.h"
#include "math/plane.h"
#include "Physics/physicsserver.h"
#include "util/fixedarray.h"

//------------------------------------------------------------------------------
class btDynamicsWorld;
class btRigidBody;
namespace Physics
{
class Shape;
class Composite;

class RigidBody : public Core::RefCounted
{
	__DeclareClass(RigidBody);
public:
    /// link types (for linking to graphical representation)
    enum LinkType
    {
        ModelNode = 0,        
        JointNode,

        NumLinkTypes,
    };  

    /// an unique rigid body id
    typedef unsigned int Id;
    /// constructor
    RigidBody();
    /// destructor
    virtual ~RigidBody();
    /// get the rigid body's unique id
    Id GetUniqueId() const;
    /// attach the rigid body to the world and initialize its position
	void Attach(btDynamicsWorld* world, const Math::matrix44 & m_);
    /// detach the rigid body from the world
    void Detach();
    /// return true if currently attached
    bool IsAttached() const;
    /// set name of rigid body
    void SetName(const Util::String& n);
    /// get name of rigid body
    const Util::String& GetName() const;
    /// set the body's initial local transform within the composite
    void SetInitialTransform(const Math::matrix44& m);
    /// get the body's initial local transform within the composite
    const Math::matrix44& GetInitialTransform() const;
    /// get the inverse of the initial local transform
    const Math::matrix44& GetInverseInitialTransform() const;
    /// enable/disable the rigid body
    void SetEnabled(bool b);
    /// get enabled/disabled state of the rigid body
    bool IsEnabled() const;
	/// set the body's world space transform
    void SetTransform(const Math::matrix44& m);
    /// get the body's world space transform
    const Math::matrix44& GetTransform() const;
    /// set the body's linear velocity
    void SetLinearVelocity(const Math::vector& v);
    /// get the body's linear velocity
    Math::vector GetLinearVelocity() const;
    /// set the body's angular velocity
    void SetAngularVelocity(const Math::vector& v);
    /// get the body's angular velocity
    Math::vector GetAngularVelocity() const;
	void SetMass(float);
    /// get the mass of the body
    float GetMass() const;
    /// reset the force and torque accumulators
    void Reset();
    /// transform a global point into the body's local space
    Math::vector GlobalToLocalPoint(const Math::vector& p) const;
    /// transform a body-local point into global space
    Math::vector LocalToGlobalPoint(const Math::vector& p) const;
    /// get the current accumulated force vector in local space
    void GetLocalForce(Math::vector& v) const;
    /// get the current accumulated torque vector in local space
    void GetLocalTorque(Math::vector& v) const;
    /// apply a global impulse vector at the next time step at a global position
    void ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multByMass = false);
    /// called before simulation step is taken
    void OnStepBefore();
    /// called after simulation step is taken
    void OnStepAfter();
    /// called before simulation takes place
    void OnFrameBefore();
    /// called after simulation takes place
    void OnFrameAfter();
    /// set pointer to composite we are attached to
    void SetShape(btCollisionShape * c);
    /// get pointer to composite we are attached to
    btCollisionShape* GetShape() const;
    /// set pointer to physics entity we are attached to
    void SetEntity(PhysicsEntity* e);
    /// get pointer to entity we are attached to
    PhysicsEntity* GetEntity() const;
	/// set pointer to composite we are attached to
	void SetComposite(Composite* c);
	/// get pointer to composite we are attached to
	Composite* GetComposite() const;
    /// render the debug shapes
    void RenderDebug();
    /// get the ode rigid body id
    btRigidBody* GetBulletBody() const;
    /// enable/disable collision for connected bodies
    void SetConnectedCollision(bool b);
    /// get connected collision flag
    bool GetConnectedCollision() const;
    /// set the damping flag
    void SetDampingActive(bool active);
    /// get the damping flag
    bool GetDampingActive() const;
    /// set angular damping factor (0.0f..1.0f)
    void SetAngularDamping(float f);
    /// get angular damping factor (0.0f..1.0f)
    float GetAngularDamping() const;
    /// set linear damping factor
    void SetLinearDamping(float f);
    /// get linear damping factor
    float GetLinearDamping() const;
    /// set temporary stamp value
    void SetStamp(uint s);
    /// get temporary stamp value
    uint GetStamp() const;
    /// set whether body is influenced by gravity
    void SetEnableGravity(bool enable);
    /// get whether body is influenced by gravity
    bool GetEnableGravity() const;

	void SetCollideCategory(unsigned int coll);
	unsigned int GetCollideCategory() const;	

	void SetKinematic(bool set);

	bool HasTransformChanged() const;

private:
    /// apply angular damping
    void ApplyDamping();		
	void UpdateTransform(const Math::matrix44 & m);
	void SetRigidBody(btRigidBody * b);
	friend class NebulaMotionState;
	friend class PhysicsEntity;
	friend class ActorPhysicsEntity;
    friend class Shape;
    friend class Joint;

    static Id uniqueIdCounter;

    Math::matrix44 initialTransform;
    Math::matrix44 inverseInitialTransform;
    Math::matrix44 transform;
    Id uniqueId;
	bool transformChanged;
    Util::String name;
    btCollisionShape * shape;
    PhysicsEntity* entity;
	Composite * composite;
    btRigidBody * body;    
	btDynamicsWorld * world;       
    bool connectedCollision;
    float angularDamping;
    float linearDamping;
    bool dampingActive;
    uint stamp;
	float mass;
	bool gravityEnabled;
	NebulaMotionState * motion;

	bool externalBody;		
};

//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetStamp(uint s)
{
    this->stamp = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
RigidBody::GetStamp() const
{
    return this->stamp;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
RigidBody::IsAttached() const
{
    return (0 != this->body);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
	
*/
inline
bool RigidBody::HasTransformChanged() const
{
	return transformChanged;
}

//------------------------------------------------------------------------------
/**
*/
inline
RigidBody::Id
RigidBody::GetUniqueId() const
{
    return this->uniqueId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetShape(btCollisionShape * c)
{
    this->shape = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
btCollisionShape*
RigidBody::GetShape() const
{
    return this->shape;
}

//------------------------------------------------------------------------------
/**
*/
inline
PhysicsEntity*
RigidBody::GetEntity() const
{
    return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetName(const Util::String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
RigidBody::GetName() const
{
    return this->name;
}



//------------------------------------------------------------------------------
/**
    Enable/disable the rigid body. Bodies which have reached a resting position
    should be disabled. Bodies will enable themselves as soon as they are
    touched by another body.

    @param  b   new enabled state
*/


//------------------------------------------------------------------------------
/**
    Get the body's mass.

    @return     mass
*/
inline
float
RigidBody::GetMass() const
{
    return this->mass;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetInitialTransform(const Math::matrix44& m)
{
    this->initialTransform = m;
    this->inverseInitialTransform = Math::matrix44::inverse(m);    
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44&
RigidBody::GetInitialTransform() const
{
    return this->initialTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44&
RigidBody::GetInverseInitialTransform() const
{
    return  this->inverseInitialTransform;
}

//------------------------------------------------------------------------------
/**
    Enable/disable collision for bodies which are connected by a joint.
    For collision, both bodies connected by a joint must have this flag set.
    Default is off.
*/
inline
void
RigidBody::SetConnectedCollision(bool b)
{
    this->connectedCollision = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
RigidBody::GetConnectedCollision() const
{
    return this->connectedCollision;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetDampingActive(bool active)
{
    this->dampingActive = active;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
RigidBody::GetDampingActive() const
{
    return this->dampingActive;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetAngularDamping(float f)
{
    n_assert((f >= 0.0f) && (f <= 1.0f));
    this->angularDamping = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
RigidBody::GetAngularDamping() const
{
    return this->angularDamping;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetLinearDamping(float f)
{
    n_assert((f >= 0.0f) && (f <= 1.0f));
    this->linearDamping = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
RigidBody::GetLinearDamping() const
{
    return this->linearDamping;
}

inline
btRigidBody*
RigidBody::GetBulletBody() const
{
	return body;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
RigidBody::GetEnableGravity() const
{
    return gravityEnabled;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
RigidBody::SetComposite(Composite* c)
{
	this->composite = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
Composite*
RigidBody::GetComposite() const
{
	return this->composite;
}

}; // namespace Physics

//------------------------------------------------------------------------------
#endif    