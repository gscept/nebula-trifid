//------------------------------------------------------------------------------
//  physics/mousegripper.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/mousegripper.h"
#include "physics/physicsserver.h"
#include "physics/rigidbody.h"
#include "math/line.h"
#include "debugrender/debugshaperenderer.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "physics/conversion.h"
#include "debug/debugfloat.h"
namespace Physics
{
__ImplementClass(Physics::MouseGripper, 'PMOU', Core::RefCounted);

using namespace Math;
using namespace Debug;

const float MouseGripper::positionGain = -3.0f;
const float MouseGripper::positionStepSize = 0.01f;

//------------------------------------------------------------------------------
/**
*/
MouseGripper::MouseGripper() :
    maxDistance(20.0f),
    enabled(true),
    gripOpen(true),
	pickConstraint(0),
    maxForce(100.0f),
	useCenter(false)
{    
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
    if (!this->gripOpen)
    {
        RigidBody* rigidBody = this->contactPoint->GetRigidBody();
        if (!rigidBody)
        {
            this->gripOpen = true;
            this->contactPoint->Clear();
			return;
        }		
		this->UpdateGripForce();
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
/*
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
		*/
    }
}

//------------------------------------------------------------------------------
/**
    Open the grip, this will release the currently grabbed physics entity
    (if any).
*/
void
MouseGripper::OpenGrip()
{
	if(this->pickConstraint != NULL)
	{
		contactPoint->GetRigidBody()->GetBulletBody()->forceActivationState(ACTIVE_TAG);
		contactPoint->GetRigidBody()->GetBulletBody()->setDeactivationTime(0.f);
		PhysicsServer::Instance()->GetBulletWorld()->removeConstraint(this->pickConstraint);
		n_delete(this->pickConstraint);
		this->pickConstraint = 0;
		this->contactPoint->Clear();
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
	if(this->contactPoint.isvalid() && this->contactPoint->GetRigidBody()!= 0)
	{
	    
		PhysicsServer* physicsServer = PhysicsServer::Instance();
	    

		// get entity pointer (if it still exists)
		RigidBody* rigidBody = this->contactPoint->GetRigidBody();
		rigidBody->GetBulletBody()->setActivationState(DISABLE_DEACTIVATION);

		
			// compute the distance from the camera to the contact point            
		Math::vector diffVec = this->worldMouseRay.start() - this->contactPoint->GetPosition();
		this->curDistance = diffVec.length();
	

		btPoint2PointConstraint* p2p;
		if(useCenter)
		{
			p2p = new btPoint2PointConstraint(*rigidBody->GetBulletBody(),btVector3(0,0,0));
		}
		else
		{
			btVector3 pickPos = Neb2BtVector(this->contactPoint->GetPosition());
			btVector3 localPivot = rigidBody->GetBulletBody()->getCenterOfMassTransform().inverse() * pickPos;			
			p2p = new btPoint2PointConstraint(*rigidBody->GetBulletBody(),localPivot);
		}
		physicsServer->GetBulletWorld()->addConstraint(p2p);
		pickConstraint = p2p;
		// constraint settings
		p2p->m_setting.m_impulseClamp = 30;		
		p2p->m_setting.m_tau = 0.001f;
		p2p->m_setting.m_damping = 1.3f;

	
	this->gripOpen = false;
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
    Ptr<Physics::ContactPoint> contactPtr;
    FilterSet excludeSet;    
    vector rayDir = vector::normalize(this->worldMouseRay.end());
    this->worldMouseRay.set(this->worldMouseRay.start(), this->worldMouseRay.start() + rayDir * rayLen);
    contactPtr = physicsServer->GetClosestContactUnderMouse(this->worldMouseRay, excludeSet);
    if (this->gripOpen)
    {
        if (contactPtr.isvalid())
        {
            this->contactPoint = contactPtr;
            this->gripPosition.Reset(time, positionStepSize, positionGain, contactPtr->GetPosition());
        }
        else
        {
            this->gripPosition.Reset(time, positionStepSize, positionGain, this->worldMouseRay.end());
            //this->contactPoint->Clear();
        }
    }
    else
    {
		//DebugFloat::print(this->worldMouseRay.end(),"end of ray");		
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
        RigidBody* rigidBody = this->contactPoint->GetRigidBody();
        if (rigidBody)
        {
            bodyTransform.scale(Math::vector(0.1f, 0.1f, 0.1f));
            bodyTransform.set_position(Bt2NebVector(((btPoint2PointConstraint*)this->pickConstraint)->getPivotInB()));
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
    if (!this->gripOpen && (0 != this->contactPoint->GetRigidBody()))
    {
        RigidBody* rigidBody = this->contactPoint->GetRigidBody();
        if (rigidBody)
        {
            // update the dummy body's position to correspond with the mouse            
            rigidBody->SetEnabled(true);            
            const Math::vector& pos = this->gripPosition.GetState();
			((btPoint2PointConstraint*)this->pickConstraint)->setPivotB(Neb2BtVector(pos));
			//((btPoint2PointConstraint*)this->pickConstraint)->setPivotB(Neb2BtVector(this->worldMouseRay.end()));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Returns the entity id of the currently grabbed entity, or 0 if nothing
    grabbed.
*/
PhysicsEntity*
MouseGripper::GetGrabbedEntity() const
{
    if (!this->gripOpen && (this->contactPoint->GetRigidBody() != 0))
    {
		return this->contactPoint->GetRigidBody()->GetEntity();        
    }
    else
    {
        return 0;
    }
}

} // namespace Physics