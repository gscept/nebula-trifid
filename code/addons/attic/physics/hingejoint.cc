//------------------------------------------------------------------------------
//  physics/hingejoint.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/hingejoint.h"
#include "debugrender/debugshaperenderer.h"

namespace Physics
{
__ImplementClass(Physics::HingeJoint, 'PHJO', Physics::Joint);

using namespace Math;
using namespace Debug;

//------------------------------------------------------------------------------
/**
*/
HingeJoint::HingeJoint() : 
    Joint(Joint::HingeJoint)
{
    this->axisParams.SetAxis(Math::vector(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
*/
HingeJoint::~HingeJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added 
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
HingeJoint::Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateHinge(worldID, groupID);

    // configure ODE joint
    if (this->axisParams.IsLoStopEnabled())
    {
        dJointSetHingeParam(this->odeJointId, dParamLoStop, this->axisParams.GetLoStop());
    }
    if (this->axisParams.IsHiStopEnabled())
    {
        dJointSetHingeParam(this->odeJointId, dParamHiStop, this->axisParams.GetHiStop());
    }
    dJointSetHingeParam(this->odeJointId, dParamVel, this->axisParams.GetVelocity());
    dJointSetHingeParam(this->odeJointId, dParamFMax, this->axisParams.GetFMax());
    dJointSetHingeParam(this->odeJointId, dParamFudgeFactor, this->axisParams.GetFudgeFactor());
    dJointSetHingeParam(this->odeJointId, dParamBounce, this->axisParams.GetBounce());
    dJointSetHingeParam(this->odeJointId, dParamCFM, this->axisParams.GetCFM());
    dJointSetHingeParam(this->odeJointId, dParamStopERP, this->axisParams.GetStopERP());
    dJointSetHingeParam(this->odeJointId, dParamStopCFM, this->axisParams.GetStopCFM());
    
    // hand to parent class
    Joint::Attach(worldID, groupID, m);

    // configure ODE joint
    this->UpdateTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
void 
HingeJoint::UpdateTransform(const Math::matrix44& m)
{
    Math::vector p = matrix44::transform(this->GetAnchor(), m);
    dJointSetHingeAnchor(this->odeJointId, p.x(), p.y(), p.z());

    matrix44 m44(m.get_xaxis(), m.get_yaxis(), m.get_zaxis(), vector::nullvec());
    Math::vector a = matrix44::transform(this->axisParams.GetAxis(), m44);
    dJointSetHingeAxis(this->odeJointId, a.x(), a.y(), a.z());
}

//------------------------------------------------------------------------------
/**
*/
void
HingeJoint::RenderDebug()
{
    if (this->IsAttached())
    {
        Math::matrix44 m = Math::matrix44::identity();
        dVector3 curAnchor;
        dJointGetHingeAnchor(this->odeJointId, curAnchor);
        m.scale(Math::vector(0.1f, 0.1f, 0.1f));
        m.translate(Math::vector(curAnchor[0], curAnchor[1], curAnchor[2]));
        DebugShapeRenderer::Instance()->DrawSphere(m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics