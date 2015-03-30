//------------------------------------------------------------------------------
//  physics/composite.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "physics/composite.h"
#include "physics/joint.h"
#include "physics/shape.h"

namespace Physics
{
__ImplementClass(Physics::Composite, 'PCO2', Core::RefCounted);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Composite::Composite() :
    curBodyIndex(0),
    curJointIndex(0),
	masterBodyIndex(0),
    entity(0),
    isAttached(false),
    dynamicsWorld(0),
    transformChanged(false),
    transformWasSet(false),
    frameBeforeTransform(Math::matrix44::identity()),
    transform(Math::matrix44::identity())
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Composite::~Composite()
{
    if (this->IsAttached())
    {
        this->Detach();
    }
    this->ClearJoints();
    this->ClearBodies();
}

//------------------------------------------------------------------------------
/**
    Clear contained joints.
*/
void
Composite::ClearJoints()
{
    if (!this->jointArray.IsEmpty())
    {
        this->jointArray.Fill(0);
		this->jointArray.Clear();
    }
	this->curJointIndex = 0;
}


void 
Composite::SetSizes(int numBodies, int numJoints)
{
	n_assert(this->jointArray.IsEmpty() && this->bodyArray.IsEmpty());
	this->bodyArray.SetSize(numBodies);
	this->bodyArray.Fill(0);
	this->jointArray.SetSize(numJoints);
	this->jointArray.Fill(0);
	this->curBodyIndex = 0;
	this->curJointIndex = 0;
}

//------------------------------------------------------------------------------
/**
    Clear contained rigid bodies.
*/
void
Composite::ClearBodies()
{
    if (!this->bodyArray.IsEmpty())
    {
        this->bodyArray.Fill(0);
		this->bodyArray.Clear();
    }
	this->curBodyIndex = 0;
}



//------------------------------------------------------------------------------
/**
    Add a rigid body to the group. Increments refcount of rigid body.

    @param  body    pointer to a rigid body object
*/
void
Composite::AddBody(RigidBody* body)
{    
    n_assert(body);
    this->bodyArray[this->curBodyIndex++] = body;
    body->SetComposite(this);
}


void 
Composite::SetMasterBody(const Util::String & name)
{
	n_assert2(this->HasBodyWithName(name),"Master body not found");
	IndexT i;
	for (i = 0; i < bodyArray.Size(); i++)
	{
		if (name == bodyArray[i]->GetName())
		{
			this->masterBodyIndex = i;
		}
	}

}
//------------------------------------------------------------------------------
/**
*/
bool
Composite::HasBodyWithName(const Util::String& name) const
{
    IndexT i;
    for (i = 0; i < bodyArray.Size(); i++)
    {
        if (name == bodyArray[i]->GetName())
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
RigidBody*
Composite::GetBodyByName(const Util::String& name) const
{
    n_assert(HasBodyWithName(name));
    IndexT i;
    for (i = 0; i < bodyArray.Size(); i++)
    {
        if (name == bodyArray[i]->GetName())
        {
            return bodyArray[i];
        }
    }
    
    n_error("Body with name %s not found!", name.AsCharPtr());
    return 0;
}

//------------------------------------------------------------------------------
/**
    Add a joint to the group. Increments refcount of joint.

    @param  joint   pointer to a joint object
*/
void
Composite::AddJoint(Joint* joint)
{    
    n_assert(joint);
    this->jointArray[this->curJointIndex++] = joint;
}


//------------------------------------------------------------------------------
/**
    Attach the composite to the world.
*/
void
Composite::Attach(btDynamicsWorld* world, const Math::matrix44 & m_)
{
    n_assert(!this->IsAttached());
	this->transform = m_;

    // count the number of shapes in the composite, this
    // dictates whether and what type of local collision space
    // will be created.	
    const int numBodies = this->GetNumBodies();
	if(numBodies == 0)
		return;
    if (numBodies > 1)
    {
        
        int bodyIndex;              

        // attach bodies
        for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
        {
            RigidBody* body = this->GetBodyAt(bodyIndex);
          //  Math::matrix44 m = Math::matrix44::multiply(body->GetInitialTransform(), this->transform);
            body->Attach(world, this->transform);
        }

        // attach joints
        int numJoints = this->GetNumJoints();
        int jointIndex;
        for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
        {
            Joint* joint = this->GetJointAt(jointIndex);
            joint->Attach(world);//, 0, this->transform);
        }
	}
	else
	{
		RigidBody* body = this->GetBodyAt(0);
		body->Attach(world,this->transform);
	}
#if 0
	//FIXME we dont support optional static collision shapes in composites (not sure we want them either)
    // attach static shapes
    int numShapes = this->GetNumShapes();
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        Shape* shape = this->GetShapeAt(shapeIndex);
        shape->SetTransform(this->transform);
        shape->Attach(staticSpaceId);
    }
#endif
    this->isAttached = true;
}

//------------------------------------------------------------------------------
/**
    Detach the composite from the world.
*/
void
Composite::Detach()
{
    n_assert(this->IsAttached());

	// detach joints
	int numJoints = this->GetNumJoints();
	int jointIndex;
	for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
	{
		this->GetJointAt(jointIndex)->Detach();
	}

    // detach bodies
    int numBodies = this->GetNumBodies();
    int bodyIndex;
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        this->GetBodyAt(bodyIndex)->Detach();
    }
	this->ClearJoints();
	this->ClearBodies();

   
#if 0
	//FIXME
    // detach shapes
    int numShapes = this->GetNumShapes();
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < numShapes;  shapeIndex++)
    {
        this->GetShapeAt(shapeIndex)->Detach();
    }
#endif

    this->isAttached = false;
}

//------------------------------------------------------------------------------
/**
    This method is called before a simulation step is taken.
*/
void
Composite::OnStepBefore()
{
    if (this->IsAttached())
    {
        int num = this->GetNumBodies();
        int i;
        for (i = 0; i < num; i++)
        {
            this->GetBodyAt(i)->OnStepBefore();
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called after a simulation step is taken.
*/
void
Composite::OnStepAfter()
{
    if (this->IsAttached())
    {
        int num = this->GetNumBodies();
        int i;
        for (i = 0; i < num; i++)
        {
            this->GetBodyAt(i)->OnStepAfter();
        }        
    }
}

//------------------------------------------------------------------------------
/**
    This method is called before a physics frame is taken.
*/
void
Composite::OnFrameBefore()
{
    if (this->IsAttached())
    {
		int num = this->GetNumBodies();
		int i;
		for (i = 0; i < num; i++)
		{
			this->GetBodyAt(i)->OnFrameBefore();
		}   
      
    }
}

//------------------------------------------------------------------------------
/**
    This method is called after a physics frame is taken.
*/
void
Composite::OnFrameAfter()
{
    if (this->IsAttached())
    {
		int num = this->GetNumBodies();
		int i;
		for (i = 0; i < num; i++)
		{
			this->GetBodyAt(i)->OnFrameAfter();
		} 
	}
}

//------------------------------------------------------------------------------
/**
    Enable/disable the composite. The enabled state is simply distributed
    to all rigid bodies in the composite. Disabled bodies will reenable
    themselves automatically on contact with other enabled bodies.
*/
void
Composite::SetEnabled(bool b)
{
    int num = this->GetNumBodies();
    int i;
    for (i = 0; i < num; i++)
    {
        this->GetBodyAt(i)->SetEnabled(b);
    }
	num = this->GetNumJoints();	
	for (i = 0; i < num; i++)
	{
		this->GetJointAt(i)->SetEnabled(b);
	}

}

//------------------------------------------------------------------------------
/**
    Return the enabled/disabled state of the composite. This is 
    the enabled state of the master rigid body.
*/
bool
Composite::IsEnabled() const
{
    RigidBody* masterBody = this->GetMasterBody();
    if (masterBody)
    {
        return this->GetMasterBody()->IsEnabled();
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Composite::SetTransform(const Math::matrix44& m)
{
    
	// only set on master body, ignore the rest

	if(this->GetMasterBody())
		this->GetMasterBody()->SetTransform(m);
	this->transform = m;
	this->transformWasSet = true;
    if (this->IsAttached())
    {
        // update rigid body transforms
        int bodyIndex;
        int numBodies = this->GetNumBodies();
        Math::matrix44 bodyTransform;
        for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
        {
            // take body's local initial position into account
            RigidBody* body = this->GetBodyAt(bodyIndex);
            body->SetTransform(m);
        }


	#if 0
		//FIXME
        // update shape transforms
        int shapeIndex;
        int numShapes = this->GetNumShapes();
        for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
        {
            this->GetShapeAt(shapeIndex)->SetTransform(m);
        }
	#endif
    }
}

//------------------------------------------------------------------------------
/**
    This resets the velocities and force accumulators of the contained
    rigid bodies.
*/
void
Composite::Reset()
{
    int numBodies = this->bodyArray.Size();
    int i;
    for (i = 0; i < numBodies; i++)
    {
        RigidBody* body = this->bodyArray[i];
        body->Reset();
    }
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of the composite object.
*/
void
Composite::RenderDebug()
{
    int bodyIndex;
    int numBodies = this->bodyArray.Size();
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        this->bodyArray[bodyIndex]->RenderDebug();
    }
    int jointIndex;
    int numJoints = this->jointArray.Size();
    for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
    {
        this->jointArray[jointIndex]->RenderDebug();
    }
    
}

//------------------------------------------------------------------------------
/**
    NOTE: this will not increment the refcount of the entity to avoid
    a cyclic reference.
*/
void
Composite::SetEntity(PhysicsEntity* e)
{
    n_assert(e);
    this->entity = e;

    // distribute entity pointer to shapes
    int bodyIndex;
    int numBodies = this->bodyArray.Size();
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        this->bodyArray[bodyIndex]->SetEntity(e);
    }
#if 0
	//FIXME
    int shapeIndex;
    int numShapes = this->shapeArray.Size();
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        this->shapeArray[shapeIndex]->SetEntity(e);
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Checks if a rigid body with the given unique id is contained in the
    composite, and returns a pointer to it, or 0 if not exists.
*/
RigidBody*
Composite::FindBodyByUniqueId(uint id) const
{
    if (0 != id)
    {
        uint i;
        uint num = (uint)this->GetNumBodies();
        for (i = 0; i < num; i++)
        {
            if (id == this->GetBodyAt(i)->GetUniqueId())
            {
                return this->GetBodyAt(i);
            }
        }
    }
    return 0;
}

} // namespace Physics