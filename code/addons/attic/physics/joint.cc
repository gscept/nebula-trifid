//------------------------------------------------------------------------------
//  physics/joint.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/joint.h"
#include "physics/rigidbody.h"

namespace Physics
{
__ImplementClass(Physics::Joint, 'PJOI', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Joint::Joint() : 
    type(InvalidType),
    odeJointId(0),
    isAttached(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Joint::Joint(Type t) : 
    type(t),
    odeJointId(0),
    isAttached(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Joint::~Joint()
{
    if (this->IsAttached())
    {
        this->Detach();
    }
}

//------------------------------------------------------------------------------
/**
    Attach the joint to the world given a parent transformation. This will 
    actually create the ODE joint, since ODE doesn't have a way to attach and 
    detach joints.
*/
void
Joint::Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m)
{
    // attach joint to rigid bodies
    dBodyID bodyID1 = 0;
    dBodyID bodyID2 = 0;
    if (this->rigidBody1.isvalid())
    {
        bodyID1 = this->rigidBody1->GetOdeBodyId();
        n_assert(0 != bodyID1);
    }
    if (this->rigidBody2.isvalid())
    {
        bodyID2 = this->rigidBody2->GetOdeBodyId();
        n_assert(0 != bodyID2);
    }
    dJointAttach(this->odeJointId, bodyID1, bodyID2);
}

//------------------------------------------------------------------------------
/**
    Detach the joint from the world. This will destroy the ODE joint.
*/
void
Joint::Detach()
{
    n_assert(this->IsAttached());

    // destroy joint object
    if (0 != this->odeJointId)
    {
        dJointDestroy(this->odeJointId);
        this->odeJointId = 0;
    }
}

//------------------------------------------------------------------------------
/**
    This method should be overwritten in subclasses.
*/
void
Joint::UpdateTransform(const Math::matrix44& m)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Joint::SetBodies(RigidBody* body1, RigidBody* body2)
{
    this->rigidBody1 = body1;
    this->rigidBody2 = body2;
}

//------------------------------------------------------------------------------
/**
    Pointer to the first body to wich the joint is attached to.
*/
const RigidBody*
Joint::GetBody1() const
{
    return this->rigidBody1.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Pointer to the first body to wich the joint is attached to.
*/
const RigidBody*
Joint::GetBody2() const
{
    return this->rigidBody2.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of this shape.
*/
void
Joint::RenderDebug()
{
    // empty
}

} // namespace Physics