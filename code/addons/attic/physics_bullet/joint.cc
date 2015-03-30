//------------------------------------------------------------------------------
//  physics/joint.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "Physics/shape.h"
#include "Physics/joint.h"
#include "Physics/rigidbody.h"

namespace Physics
{
__ImplementClass(Physics::Joint, 'PJO2', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Joint::Joint() : 
    type(InvalidType),
	constraint(NULL),
	bworld(NULL),
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

void
Joint::SetType(Type t)
{
	type = t;
}
void
Joint::SetConstraint(btTypedConstraint* con)
{
	this->constraint = con;
	btTypedConstraintType tt = con->getConstraintType();
	switch(tt)
	{
	case POINT2POINT_CONSTRAINT_TYPE:
		this->type = Point2PointConstraint;
		break;
	case HINGE_CONSTRAINT_TYPE:
		this->type = HingeConstraint;
		break;
	case D6_CONSTRAINT_TYPE:
		this->type = UniversalConstraint;
		break;
	case SLIDER_CONSTRAINT_TYPE:
		this->type = SliderConstraint;
		break;
	case CONTACT_CONSTRAINT_TYPE:
		this->type = ContactConstraint;
		break;			
	default:
		this->type = UniversalConstraint;
		n_assert(false);
		break;
	}
}

void Joint::Attach(btDynamicsWorld* world)
{
	n_assert(this->constraint);
	this->bworld = world;
	world->addConstraint(this->constraint);
}

void Joint::SetEnabled(bool b)
{
	n_assert(this->constraint);
	this->constraint->setEnabled(b);
}

bool Joint::IsEnabled() const
{
	n_assert(this->constraint);
	return this->constraint->isEnabled();
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
    if (0 != this->constraint)
    {
 		bworld->removeConstraint(constraint);
        n_delete(this->constraint);
        this->constraint = 0;
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