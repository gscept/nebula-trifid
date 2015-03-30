//------------------------------------------------------------------------------
//  physics/composite.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/composite.h"
#include "physics/joint.h"
#include "physics/shape.h"

namespace Physics
{
__ImplementClass(Physics::Composite, 'PCOM', Core::RefCounted);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
Composite::Composite() :
    curBodyIndex(0),
    curJointIndex(0),
    curShapeIndex(0),
    entity(0),
    inBeginBodies(false),
    inBeginJoints(false),
    inBeginShapes(false),
    isAttached(false),
    odeSpaceId(0),
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
    }
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
    }
}

//------------------------------------------------------------------------------
/**
    Clear contained static shapes.
*/
void
Composite::ClearShapes()
{
    if (!this->shapeArray.IsEmpty())
    {
        this->shapeArray.Fill(0);
    }
}

//------------------------------------------------------------------------------
/**
    Begin adding rigid bodies to the rigid body group.

    @param  num     number of rigid bodies
*/
void
Composite::BeginBodies(int num)
{
    n_assert(!this->inBeginBodies);
    this->ClearBodies();
    this->bodyArray.SetSize(num);
    this->curBodyIndex = 0;
    this->inBeginBodies = true;
}

//------------------------------------------------------------------------------
/**
    Add a rigid body to the group. Increments refcount of rigid body.

    @param  body    pointer to a rigid body object
*/
void
Composite::AddBody(RigidBody* body)
{
    n_assert(this->inBeginBodies);
    n_assert(body);
    this->bodyArray[this->curBodyIndex++] = body;
    body->SetComposite(this);
}

//------------------------------------------------------------------------------
/**
    finish adding rigid bodies.
*/
void
Composite::EndBodies()
{
    n_assert(this->inBeginBodies);
    n_assert(this->bodyArray.Size() == this->curBodyIndex);
    this->inBeginBodies = false;
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
    Begin adding joints to the rigid body group.

    @param  num     number of joints
*/
void
Composite::BeginJoints(int num)
{
    n_assert(!this->inBeginJoints);
    this->ClearJoints();
    this->jointArray.SetSize(num);
    this->curJointIndex = 0;
    this->inBeginJoints = true;
}

//------------------------------------------------------------------------------
/**
    Add a joint to the group. Increments refcount of joint.

    @param  joint   pointer to a joint object
*/
void
Composite::AddJoint(Joint* joint)
{
    n_assert(this->inBeginJoints);
    n_assert(joint);
    this->jointArray[this->curJointIndex++] = joint;
}

//------------------------------------------------------------------------------
/**
    Finish adding joints.
*/
void
Composite::EndJoints()
{
    n_assert(this->inBeginJoints);
    n_assert(this->jointArray.Size() == this->curJointIndex);
    this->inBeginJoints = false;
}

//------------------------------------------------------------------------------
/**
    Begin adding static shapes.
*/
void
Composite::BeginShapes(int num)
{
    n_assert(!this->inBeginShapes);
    this->ClearShapes();
    this->shapeArray.SetSize(num);
    this->curShapeIndex = 0;
    this->inBeginShapes = true;
}

//------------------------------------------------------------------------------
/**
    Add a shape to the composite. Increments refcount of shape.
*/
void
Composite::AddShape(Shape* shape)
{
    n_assert(this->inBeginShapes);
    n_assert(shape);
    this->shapeArray[this->curShapeIndex++] = shape;
}

//------------------------------------------------------------------------------
/**
    Finish adding shapes.
*/
void
Composite::EndShapes()
{
    n_assert(this->inBeginShapes);
    n_assert(this->shapeArray.Size() == this->curShapeIndex);
    this->inBeginShapes = false;
}

//------------------------------------------------------------------------------
/**
    Attach the composite to the world.
*/
void
Composite::Attach(dWorldID worldID, dSpaceID dynamicSpaceId, dSpaceID staticSpaceId)
{
    n_assert(!this->IsAttached());

    // count the number of shapes in the composite, this
    // dictates whether and what type of local collision space
    // will be created.
    const int numBodies = this->GetNumBodies();
    if (numBodies > 0)
    {
        int numBodyShapes = 0;
        int bodyIndex;
        for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
        {
            numBodyShapes += this->GetBodyAt(bodyIndex)->GetNumShapes();
        }

        // if number of shapes is equal to 1 we don't allocate
        // a local collide space, but instead add the shape directly
        // to the global collide space, otherwise create a simple space
        dSpaceID localSpaceID = dynamicSpaceId;
        /*
        // FIXME: this doesn't work with the static/dynamic space approach
        if (numBodyShapes > 1)
        {
            // create a simple collision space
            this->odeSpaceId = dHashSpaceCreate(dynamicSpaceId);
            dHashSpaceSetLevels(this->odeSpaceId, -3, 2);
            localSpaceID = odeSpaceId;
        }
        */

        // attach bodies
        for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
        {
            RigidBody* body = this->GetBodyAt(bodyIndex);
            Math::matrix44 m = Math::matrix44::multiply(body->GetInitialTransform(), this->transform);
            body->Attach(worldID, localSpaceID, m);
        }

        // attach joints
        int numJoints = this->GetNumJoints();
        int jointIndex;
        for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
        {
            Joint* joint = this->GetJointAt(jointIndex);
            joint->Attach(worldID, 0, this->transform);
        }
    }

    // attach static shapes
    int numShapes = this->GetNumShapes();
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        Shape* shape = this->GetShapeAt(shapeIndex);
        shape->SetTransform(this->transform);
        shape->Attach(staticSpaceId);
    }

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

    // detach bodies
    int numBodies = this->GetNumBodies();
    int bodyIndex;
    for (bodyIndex = 0; bodyIndex < numBodies; bodyIndex++)
    {
        this->GetBodyAt(bodyIndex)->Detach();
    }

    // detach joints
    int numJoints = this->GetNumJoints();
    int jointIndex;
    for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
    {
        this->GetJointAt(jointIndex)->Detach();
    }

    // detach shapes
    int numShapes = this->GetNumShapes();
    int shapeIndex;
    for (shapeIndex = 0; shapeIndex < numShapes;  shapeIndex++)
    {
        this->GetShapeAt(shapeIndex)->Detach();
    }

    // release local collide space
    if (0 != this->odeSpaceId)
    {
        dSpaceDestroy(this->odeSpaceId);
        this->odeSpaceId = 0;
    }

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

        // update stored transform
        RigidBody* masterBody = this->GetMasterBody();
        if (masterBody)
        {
            Math::matrix44 m = masterBody->GetTransform();    
            this->transform = Math::matrix44::multiply(masterBody->GetInverseInitialTransform(), m);
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
        // take a snapshot of the current transform
        this->frameBeforeTransform = this->transform;
        this->transformChanged = false;

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
            RigidBody* curBody = this->GetBodyAt(i);
            curBody->OnFrameAfter();

            // if any rigid body is enabled, we set the 
            // transform changed flag
            if (curBody->IsEnabled())
            {
                this->transformChanged = true;
            }
        }

        // check if transform has changed by other means
        if (!this->transformChanged)
        {
            float4 epsilon(0.001, 0.001f, 0.001f, 0.001f);
            if (this->transformWasSet ||
                (!float4::nearequal4(this->frameBeforeTransform.get_xaxis(), this->transform.get_xaxis(), epsilon)) ||
                (!float4::nearequal4(this->frameBeforeTransform.get_yaxis(), this->transform.get_yaxis(), epsilon)) ||
                (!float4::nearequal4(this->frameBeforeTransform.get_zaxis(), this->transform.get_zaxis(), epsilon)) ||
                (!float4::nearequal4(this->frameBeforeTransform.get_position(), this->transform.get_position(), epsilon)))
            {
                this->transformChanged = true;
            }
        }
    }
    this->transformWasSet = false;
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
            bodyTransform = Math::matrix44::multiply(body->GetInitialTransform(), m);
            body->SetTransform(bodyTransform);
        }

        // update joint transforms
        int jointIndex;
        int numJoints = this->GetNumJoints();
        for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
        {
            this->GetJointAt(jointIndex)->UpdateTransform(m);
        }

        // update shape transforms
        int shapeIndex;
        int numShapes = this->GetNumShapes();
        for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
        {
            this->GetShapeAt(shapeIndex)->SetTransform(m);
        }
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

    const static Math::matrix44 ident = Math::matrix44::identity();
    int shapeIndex;
    int numShapes = this->shapeArray.Size();
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        this->shapeArray[shapeIndex]->RenderDebug(ident);
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
    int shapeIndex;
    int numShapes = this->shapeArray.Size();
    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
    {
        this->shapeArray[shapeIndex]->SetEntity(e);
    }
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

//------------------------------------------------------------------------------
/**
    Returns true if any of the contained rigid bodies has the
    provided link type. This method currently iterates
    through the rigid bodies (and thus may be slow).
*/
bool
Composite::HasLinkType(RigidBody::LinkType t)
{
    int i;
    int num = this->GetNumBodies();
    for (i = 0; i < num; i++)
    {
        if (this->GetBodyAt(i)->IsLinkValid(t))
        {
            return true;
        }
    }
    return false;
}

} // namespace Physics