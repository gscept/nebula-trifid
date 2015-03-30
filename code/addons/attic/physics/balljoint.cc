//------------------------------------------------------------------------------
//  physics/balljoint.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/balljoint.h"
#include "debugrender/debugshaperenderer.h"

namespace Physics
{
__ImplementClass(Physics::BallJoint, 'PBJT', Physics::Joint);

using namespace Debug;

//------------------------------------------------------------------------------
/**
*/
BallJoint::BallJoint() : 
    Joint(Joint::BallJoint)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BallJoint::~BallJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added 
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
BallJoint::Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateBall(worldID, groupID);

    // hand to parent class
    Joint::Attach(worldID, groupID, m);

    // configure ODE joint
    this->UpdateTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
void 
BallJoint::UpdateTransform(const Math::matrix44& m)
{
    Math::vector a = Math::matrix44::transform(this->anchor, m);
    dJointSetBallAnchor(this->odeJointId, a.x(), a.y(), a.z());
}

//------------------------------------------------------------------------------
/**
*/
void
BallJoint::RenderDebug()
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
        Math::matrix44 m = Math::matrix44::identity();
        dVector3 curAnchor;
        dJointGetBallAnchor(this->odeJointId, curAnchor);
        m.scale(Math::vector(0.1f, 0.1f, 0.1f));
        m.translate(Math::vector(curAnchor[0], curAnchor[1], curAnchor[2]));
        DebugShapeRenderer::Instance()->DrawSphere(m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics