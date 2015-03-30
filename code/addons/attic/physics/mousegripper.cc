//------------------------------------------------------------------------------
//  physics/mousegripper.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/mousegripper.h"
#include "physics/physicsserver.h"
#include "physics/rigidbody.h"
#include "physics/composite.h"
#include "math/line.h"
#include "debugrender/debugshaperenderer.h"

namespace Physics
{
__ImplementClass(Physics::MouseGripper, 'PMOU', Core::RefCounted);

using namespace Math;
using namespace Debug;

const float MouseGripper::positionGain = -2.0f;
const float MouseGripper::positionStepSize = 0.001f;

//------------------------------------------------------------------------------
/**
*/
MouseGripper::MouseGripper() :
    maxDistance(20.0f),
    enabled(true),
    gripOpen(true),
    dummyBodyId(0),
    dummyJointId(0),
    dummyAMotorId(0),
    maxForce(100.0f)
{
    memset(&this->jointFeedback, 0, sizeof(this->jointFeedback));
}

//------------------------------------------------------------------------------
/**
*/
MouseGripper::~MouseGripper()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called before a physics frame (consisting of multiple steps) 
    is evaluated.
*/
void
MouseGripper::OnFrameBefore()
{
    // check if grabbed rigid body has gone away for some reason
    if (!this->gripOpen && (this->contactPoint.GetRigidBodyId() != 0))
    {
        RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (!rigidBody)
        {
            this->gripOpen = true;
            this->contactPoint.Clear();
        }
    }
    this->UpdateGripPosition();
}

//------------------------------------------------------------------------------
/**
    This method is called after a physics frame is evaluated.
*/
void
MouseGripper::OnFrameAfter()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called before a physics step is taken.
*/
void
MouseGripper::OnStepBefore()
{
    this->UpdateGripForce();
}

//------------------------------------------------------------------------------
/**
    This method is called after a physics step is taken.
*/
void
MouseGripper::OnStepAfter()
{
    // decide if joint should break
    if (!this->gripOpen)
    {
        n_assert(this->dummyJointId);
        dJointFeedback* jf = dJointGetFeedback(this->dummyJointId);
        n_assert(jf);
        Math::vector f1, f2;
        PhysicsServer::OdeToVector3(jf->f1, f1);
        PhysicsServer::OdeToVector3(jf->f2, f2);
        float maxForce = n_max(f1.length(), f2.length());
        if (maxForce > this->maxForce)
        {
            // break the joint (open the grip)
            this->OpenGrip();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Returns pointer to rigid body from id's stored in contact point object.
    This may return 0 if the ids are invalid, or the body has gone away.
*/
RigidBody*
MouseGripper::GetRigidBodyFromContact() const
{
    return this->contactPoint.GetRigidBody();
}

//------------------------------------------------------------------------------
/**
    Open the grip, this will release the currently grabbed physics entity
    (if any).
*/
void
MouseGripper::OpenGrip()
{
    this->contactPoint.Clear();
    this->gripOpen = true;
    if (this->dummyJointId != 0)
    {
        dJointDestroy(this->dummyJointId);
        this->dummyJointId = 0;
    }
    if (this->dummyAMotorId != 0)
    {
        dJointDestroy(this->dummyAMotorId);
        this->dummyAMotorId = 0;
    }
    if (this->dummyBodyId != 0)
    {
        dBodyDestroy(this->dummyBodyId);
        this->dummyBodyId = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Close the grip, if a physics entity is under the mouse, it will be grabbed.
*/
void
MouseGripper::CloseGrip()
{
    if (!this->gripOpen)
    {
        // already closed
        return;
    }
    
    // check if we are over a physics entity
    if (this->contactPoint.GetPhysicsEntityId() != 0)
    {
        PhysicsServer* physicsServer = PhysicsServer::Instance();

        // get entity pointer (if it still exists)
        RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (rigidBody)
        {
            // compute the distance from the camera to the contact point            
            Math::vector diffVec = this->worldMouseRay.start() - this->contactPoint.GetPosition();
            this->curDistance = diffVec.length();

            // store the body-relative contact point
            const Math::vector& globalGripPos = this->contactPoint.GetPosition();
            this->bodyGripPosition = rigidBody->GlobalToLocalPoint(globalGripPos);

            // initialize the global grip position feedback loop
            Timing::Time time = physicsServer->GetTime();
            this->gripPosition.Reset(time, positionStepSize, positionGain, globalGripPos);

            // create a dummy rigid body
            this->dummyBodyId = dBodyCreate(physicsServer->GetOdeWorldId());
            dBodySetGravityMode(this->dummyBodyId, 0);
            dBodySetPosition(this->dummyBodyId, globalGripPos.x(), globalGripPos.y(), globalGripPos.z());
            dMass mass;
            dMassSetSphereTotal(&mass, 1000.0f, 0.1f);
            dBodySetMass(this->dummyBodyId, &mass);

            // create a ball joint and attach it to the bodies
            this->dummyJointId = dJointCreateBall(physicsServer->GetOdeWorldId(), 0);
            dJointSetFeedback(this->dummyJointId, &this->jointFeedback);
            dJointAttach(this->dummyJointId, rigidBody->GetOdeBodyId(), this->dummyBodyId);
            dJointSetBallAnchor(this->dummyJointId, globalGripPos.x(), globalGripPos.y(), globalGripPos.z());

            // create an angular motor and attach it to the bodies
            this->dummyAMotorId = dJointCreateAMotor(physicsServer->GetOdeWorldId(), 0);
            dJointAttach(this->dummyAMotorId, rigidBody->GetOdeBodyId(), this->dummyBodyId);

            dJointSetAMotorMode(this->dummyAMotorId, dAMotorEuler);
            dJointSetAMotorAxis(this->dummyAMotorId, 0, 1, 1.0f, 0.0f, 0.0f);
            dJointSetAMotorAxis(this->dummyAMotorId, 2, 2, 0.0f, 0.0f, 1.0f);

            dJointSetAMotorParam(this->dummyAMotorId, dParamVel,   0.0f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamVel2,  0.0f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamVel3,  0.0f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamFMax,  0.5f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamFMax2, 0.5f);
            dJointSetAMotorParam(this->dummyAMotorId, dParamFMax3, 0.5f);

            this->gripOpen = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    This updates the grip position depending on the current mouse position.
*/
void
MouseGripper::UpdateGripPosition()
{
    Physics::PhysicsServer* physicsServer = Physics::PhysicsServer::Instance();

    // do a ray check into the environment, using maximum or current distance
    float rayLen;
    if (this->gripOpen)
    {
        rayLen = this->maxDistance;
    }
    else
    {
        rayLen = this->curDistance;
    }
    Timing::Time time = physicsServer->GetTime();
    const Physics::ContactPoint* contactPtr = 0;
    FilterSet excludeSet;
    excludeSet.AddMaterialType(Physics::MaterialTable::StringToMaterialType("Character"));
    vector rayDir = vector::normalize(this->worldMouseRay.end());
    this->worldMouseRay.set(this->worldMouseRay.start(), this->worldMouseRay.start() + rayDir * rayLen);
    contactPtr = physicsServer->GetClosestContactUnderMouse(this->worldMouseRay, excludeSet);
    if (this->gripOpen)
    {
        if (contactPtr)
        {
            this->contactPoint = *contactPtr;
            this->gripPosition.Reset(time, positionStepSize, positionGain, contactPtr->GetPosition());
        }
        else
        {
            this->gripPosition.Reset(time, positionStepSize, positionGain, this->worldMouseRay.end());
            this->contactPoint.Clear();
        }
    }
    else
    {
        this->gripPosition.SetGoal(this->worldMouseRay.end());
    }
    this->gripPosition.Update(time);
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the gripper.
*/
void
MouseGripper::RenderDebug()
{
    DebugShapeRenderer* shapeRenderer = DebugShapeRenderer::Instance();

    Math::matrix44 gripTransform = Math::matrix44::identity();
    Math::float4 gripColor;
    Math::matrix44 bodyTransform = Math::matrix44::identity();
    Math::float4 bodyColor(1.0f, 0.0f, 0.0f, 1.0f);

    gripTransform.scale(Math::vector(0.1f, 0.1f, 0.1f));
    float4 pos = this->gripPosition.GetState();
    gripTransform.set_position(pos);
    if (this->gripOpen)
    {
        gripColor.set(1.0f, 1.0f, 0.0f, 1.0f);
    }
    else
    {
        RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (rigidBody)
        {
            bodyTransform.scale(Math::vector(0.1f, 0.1f, 0.1f));
            bodyTransform.set_position(rigidBody->LocalToGlobalPoint(this->bodyGripPosition));
            shapeRenderer->DrawSphere(bodyTransform, bodyColor);
        }
        gripColor.set(1.0f, 0.0f, 1.0f, 1.0f);
    }
    shapeRenderer->DrawSphere(gripTransform, gripColor);
}
    
//------------------------------------------------------------------------------
/**
    If grip closed around a valid rigid body, apply a force to the
    rigid body which moves the body into the direction of the 
    mouse.
*/
void
MouseGripper::UpdateGripForce()
{
    if (!this->gripOpen && (0 != this->contactPoint.GetRigidBodyId()))
    {
        RigidBody* rigidBody = this->GetRigidBodyFromContact();
        if (rigidBody)
        {
            // update the dummy body's position to correspond with the mouse
            n_assert(this->dummyBodyId != 0);
            rigidBody->SetEnabled(true);
            dBodyEnable(this->dummyBodyId);
            const Math::vector& pos = this->gripPosition.GetState();
            dBodySetPosition(this->dummyBodyId, pos.x(), pos.y(), pos.z());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Returns the entity id of the currently grabbed entity, or 0 if nothing
    grabbed.
*/
uint
MouseGripper::GetGrabbedEntityId() const
{
    if (!this->gripOpen && (this->contactPoint.GetPhysicsEntityId() != 0))
    {
        return this->contactPoint.GetPhysicsEntityId();
    }
    else
    {
        return 0;
    }
}

} // namespace Physics