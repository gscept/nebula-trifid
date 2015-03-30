//------------------------------------------------------------------------------
//  physics/rigidbody.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "debugdrawer.h"
#include "Physics/conversion.h"
#include "Physics/rigidbody.h"
#include "Physics/nebulamotionstate.h"
#include "Physics/shape.h"
#include "Physics/level.h"


namespace Physics
{
__ImplementClass(Physics::RigidBody, 'PRI2', Core::RefCounted);

using namespace Math;

uint RigidBody::uniqueIdCounter = 1;

//------------------------------------------------------------------------------
/**
*/
RigidBody::RigidBody() :    
    shape(0),
    entity(0),
	body(0),
	composite(0),
    connectedCollision(false),
    dampingActive(true),
    angularDamping(0.01f),
    linearDamping(0.005f),    
    stamp(0),
	mass(0),
	world(0),
	transformChanged(false),
	gravityEnabled(true),
	externalBody(false),
    initialTransform(Math::matrix44::identity()),
    transform(Math::matrix44::identity())	
{
    // set new unique id
    this->uniqueId = uniqueIdCounter++;
    inverseInitialTransform = Math::matrix44::inverse(initialTransform);
}

//------------------------------------------------------------------------------
/**
*/
RigidBody::~RigidBody()
{
    if (this->IsAttached())
    {
        this->Detach();
    }	
}

void
RigidBody::SetMass(float m)
{
	this->mass = m;
	if(this->body)
	{
		btVector3 inertia(0,0,0);
		this->shape->calculateLocalInertia(this->mass,inertia);
		this->body->setMassProps(mass,inertia);
	}
}
//------------------------------------------------------------------------------
/**
    Attach the rigid body to the world and initialize its position. 
    This will create an ODE rigid body object and create all associated shapes.
*/
void
RigidBody::Attach(btDynamicsWorld* inworld, const Math::matrix44 & m)
{	
	this->world = inworld;
	motion = n_new(NebulaMotionState(*this));
	motion->SetInitialTransform(this->initialTransform);
	motion->SetTransform(m);
	if(!externalBody)
	{
		n_assert(!this->IsAttached());
		n_assert(this->shape);
		btVector3 invInertia(0,0,0);
		shape->calculateLocalInertia(mass, invInertia);
		
		btRigidBody::btRigidBodyConstructionInfo boxInfo(mass, motion, shape, invInertia);
		this->body = n_new(btRigidBody(boxInfo));
		world->addRigidBody(this->body);
	}else
	{
		this->shape = this->body->getCollisionShape();
		this->body->setMotionState(motion);
		world->addRigidBody(this->body);
	}
		
	if((this->body->getCollisionFlags()&btCollisionObject::CF_STATIC_OBJECT) !=0)
	{
		// set the static category
		SetCollideCategory(Physics::Static);
	}
	this->body->setUserPointer(this);	
 
    // update body's transformation
    this->SetTransform(m);
}

void
RigidBody::SetRigidBody(btRigidBody * b)
{
	this->body = b;
	this->body->setUserPointer(this);
	this->externalBody = true;
	
}
//------------------------------------------------------------------------------
/**
    Detach the rigid body to the world. This will detach all shapes and
    destroy the ODE rigid body object.
*/
void
RigidBody::Detach()
{
    n_assert(this->IsAttached());

	this->world->removeRigidBody(this->body);
	this->world = NULL;
	n_delete(this->body);
	this->body = NULL;
}

//------------------------------------------------------------------------------
/**
    This method is called before a physics frame consisting of several
    steps takes place.
*/
void
RigidBody::OnFrameBefore()
{
    this->transformChanged = false;
}

//------------------------------------------------------------------------------
/**
    This method is called after a physics frame consisting of several
    steps takes place.

    28-Nov-04   floh    removed autodisable check, we now use ODE's builtin
                        autodisable feature
*/
void
RigidBody::OnFrameAfter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Apply an impulse vector at a position in the global coordinate frame.
	FIXME: multiplybymass does nothing atm
*/
void
RigidBody::ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multiplyByMass)
{
    n_assert(this->IsAttached());

	btTransform t = this->body->getInterpolationWorldTransform().inverse();
	btVector3 bPos = Neb2BtVector(pos);
	btVector3 bImpulse = Neb2BtVector(impulse);
	bPos = t*bPos;	
	this->body->applyImpulse(bImpulse,bPos);
    this->body->activate(true);	
}

//------------------------------------------------------------------------------
/**
    Apply linear and angular damping.
*/
void
RigidBody::ApplyDamping()
{
	//FIXME do we even need this
#if 0
    n_assert(this->IsEnabled());

    // apply angular damping
    if (this->angularDamping > 0.0f)
    {
        const dReal* odeVec = dBodyGetAngularVel(this->odeBodyId);
        Math::vector v(odeVec[0], odeVec[1], odeVec[2]);
        float damp = n_saturate(1.0f - this->angularDamping);
        v *= damp;

        // clamp at some upper value
        if (v.length() > 30.0f)
        {
            v  = vector::normalize(v);
            v *= 30.0f;
        }
        dBodySetAngularVel(this->odeBodyId, v.x(), v.y(), v.z());
    }
    if (this->linearDamping > 0.0f)
    {
        const dReal* odeVec = dBodyGetLinearVel(this->odeBodyId);
        Math::vector v(odeVec[0], odeVec[1], odeVec[2]);
        float damp = n_saturate(1.0f - this->linearDamping);
        v *= damp;

        // clamp at some upper value
        if (v.length() > 50.0f)
        {
            v =  vector::normalize(v);
            v *= 50.0f;
        }
        dBodySetLinearVel(this->odeBodyId, v.x(), v.y(), v.z());
    }
#endif
}

//------------------------------------------------------------------------------
/**
    This method is called before a simulation step is taken.
*/
void
RigidBody::OnStepBefore()
{
   
}

//------------------------------------------------------------------------------
/**
	Called by NebulaMotionState when updated by bullet
*/
void
RigidBody::UpdateTransform(const Math::matrix44 & m)
{
	 n_assert(this->IsAttached());
	 this->transform = m;
	 this->transformChanged = true;
}
//------------------------------------------------------------------------------
/**
    This method is called after a simulation step is taken.
*/
void
RigidBody::OnStepAfter()
{        
}

//------------------------------------------------------------------------------
/**
    Set the body's current transformation in global space. Only translation
    and rotation is allowed (no scale or shear).

    @param  m   the translate/rotate matrix    
*/
void
RigidBody::SetTransform(const Math::matrix44& m)
{
  this->transform = m;

    if (this->IsAttached())
    {
		this->motion->SetTransform(m);
		btTransform newWorld;
		this->motion->getWorldTransform(newWorld);
		this->body->setWorldTransform(newWorld);
		this->transformChanged = true;
    }
}

//------------------------------------------------------------------------------
/**
    Get the body's current transformation in global space.

    @param  m   [out] will be filled with the resulting matrix
*/
const Math::matrix44&
RigidBody::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
    Set the body's current linear velocity.

    @param  v   linear velocity
*/
void
RigidBody::SetLinearVelocity(const Math::vector& v)
{
	btVector3 btVel = Neb2BtVector(v);
	this->body->setLinearVelocity(btVel);
	this->body->activate(true);
}

//------------------------------------------------------------------------------
/**
    Get the body's current linear velocity.

    @param  v   [out] linear velocity
*/
Math::vector
RigidBody::GetLinearVelocity() const
{
   return Bt2NebVector(this->body->getLinearVelocity());
}

//------------------------------------------------------------------------------
/**
    Set the body's current angular velocity.

    @param  v   linear velocity
*/
void
RigidBody::SetAngularVelocity(const Math::vector& v)
{    
    btVector3 btVel = Neb2BtVector(v);
	this->body->setAngularVelocity(btVel);
	this->body->activate(true);
}

//------------------------------------------------------------------------------
/**
    Get the body's current angular velocity.

    @param  v   [out] linear velocity
*/
Math::vector
RigidBody::GetAngularVelocity() const
{    
	return Bt2NebVector(this->body->getAngularVelocity());
}

//------------------------------------------------------------------------------
/**
    Get the current accumulated force vector.

    @param  force   [out] filled with force vector
*/
void
RigidBody::GetLocalForce(Math::vector& force) const
{
	btVector3 fforce = this->body->getTotalForce();
	force = Bt2NebVector(this->body->getInterpolationWorldTransform()*fforce);
}

//------------------------------------------------------------------------------
/**
    Get the current accumulated torque vector.

    @param  force   [out] filled with torque vector
*/
void
RigidBody::GetLocalTorque(Math::vector& torque) const
{
	btVector3 force = this->body->getTotalTorque();
	torque = Bt2NebVector(this->body->getInterpolationWorldTransform()*force);	
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of the rigid body.
*/
void
RigidBody::RenderDebug()
{
    if (this->IsAttached())
    {
        Math::matrix44 m = this->GetTransform();        
		//this->shape->RenderDebug(m);        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::SetEntity(PhysicsEntity* e)
{
    n_assert(e);
    this->entity = e;   
}

//------------------------------------------------------------------------------
/**
    Transforms a point in global space into the local coordinate system
    of the body.
*/
Math::vector
RigidBody::GlobalToLocalPoint(const Math::vector& p) const
{
	n_assert(false);
	return Math::vector();
}

//------------------------------------------------------------------------------
/**
    Transforms a point in body-local space into global space.
*/
Math::vector
RigidBody::LocalToGlobalPoint(const Math::vector& p) const
{
	n_assert(false);    
	return Math::vector();
}

void
RigidBody::SetKinematic(bool set)
{
	n_assert(this->IsAttached());
	if (set)
	{
		this->body->setCollisionFlags( this->body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);		
	}
	else
	{
		this->body->setCollisionFlags( this->body->getCollisionFlags() & !btCollisionObject::CF_KINEMATIC_OBJECT);		
	}
}
//------------------------------------------------------------------------------
/**
*/
void
RigidBody::SetEnableGravity(bool enable)
{
	n_assert(this->body);
	if(enable)
	{
		this->body->setGravity(Physics::PhysicsServer::Instance()->GetBulletWorld()->getGravity());
	}else
	{
		this->body->setGravity(btVector3(0.0f,0.0f,0.0f));
	}    
	gravityEnabled = enable;
}


void
RigidBody::SetEnabled(bool b)
{
	n_assert(this->body);
    if (b) 
		this->body->setActivationState(ACTIVE_TAG);
	else
		this->body->setActivationState(DISABLE_SIMULATION);    
}

//------------------------------------------------------------------------------
/**
    Returns the current enabled state of the body.

    @return     true if body is enabled
*/

bool
RigidBody::IsEnabled() const
{
	n_assert(this->body);
    return (this->body->isActive());
}

//------------------------------------------------------------------------------
/**
    Reset the force and torque accumulators of the body.
*/
void
RigidBody::Reset()
{
	n_assert(this->body);
	this->body->clearForces();    
}

unsigned int 
RigidBody::GetCollideCategory() const
{
	n_assert2(this->GetBulletBody(),"Empty physics body");
	return this->GetBulletBody()->getBroadphaseProxy()->m_collisionFilterGroup;
}

void 
RigidBody::SetCollideCategory( unsigned int coll )
{
	n_assert2(this->GetBulletBody(),"Empty physics body");
	this->GetBulletBody()->getBroadphaseProxy()->m_collisionFilterGroup = coll;
}

} // namespace Physics