//------------------------------------------------------------------------------
//  physics/amotor.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/amotor.h"

namespace Physics
{
__ImplementClass(Physics::AMotor, 'PMOT', Physics::Joint);

//------------------------------------------------------------------------------
/**
*/
AMotor::AMotor() :
    Joint(Joint::AMotor)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AMotor::~AMotor()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    NOTE: it is important that rigid bodies are added 
    (happens in Joint::Attach()) before joint transforms are set!!!
*/
void
AMotor::Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m)
{
    // create ODE joint
    this->odeJointId = dJointCreateAMotor(worldID, groupID);

    // configure ODE joint
    dJointSetAMotorMode(this->odeJointId, dAMotorUser);
    dJointSetAMotorNumAxes(this->odeJointId, this->GetNumAxes());
    int i;
    int num = this->GetNumAxes();
    for (i = 0; i < num; i++)
    {
        const JointAxis& curAxis = this->axisParams[i];
        dJointSetAMotorAngle(this->odeJointId, i, curAxis.GetAngle());
        if (curAxis.IsLoStopEnabled())
        {
            dJointSetAMotorParam(this->odeJointId, dParamLoStop + dParamGroup * i, curAxis.GetLoStop());
        }
        if (curAxis.IsHiStopEnabled())
        {
            dJointSetAMotorParam(this->odeJointId, dParamHiStop + dParamGroup * i, curAxis.GetHiStop());
        }
        dJointSetAMotorParam(this->odeJointId, dParamVel + dParamGroup * i, curAxis.GetVelocity());
        dJointSetAMotorParam(this->odeJointId, dParamFMax + dParamGroup * i, curAxis.GetFMax());
        dJointSetAMotorParam(this->odeJointId, dParamFudgeFactor + dParamGroup * i, curAxis.GetFudgeFactor());
        dJointSetAMotorParam(this->odeJointId, dParamBounce + dParamGroup * i, curAxis.GetBounce());
        dJointSetAMotorParam(this->odeJointId, dParamCFM + dParamGroup * i, curAxis.GetCFM());
        dJointSetAMotorParam(this->odeJointId, dParamStopERP + dParamGroup * i, curAxis.GetStopERP());
        dJointSetAMotorParam(this->odeJointId, dParamStopCFM + dParamGroup * i, curAxis.GetStopCFM());
    }

    // hand to parent class
    Joint::Attach(worldID, groupID, m);

    // configure ODE joint
    this->UpdateTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
void
AMotor::UpdateVelocity(uint axisIndex)
{
    n_assert(this->GetNumAxes() > axisIndex);

    const JointAxis& curAxis = this->axisParams[axisIndex];        
    dJointSetAMotorParam(this->odeJointId, dParamVel + dParamGroup * axisIndex, curAxis.GetVelocity());  
}

//------------------------------------------------------------------------------
/**
*/
void
AMotor::UpdateTransform(const Math::matrix44& m)
{
    Math::matrix44 m44(m.getrow0(), m.getrow1(), m.getrow2(), Math::float4(0,0,0,1));

    int i;
    int num = this->GetNumAxes();
    for (i = 0; i < num; i++)
    {
        Math::vector a = Math::matrix44::transform(this->axisParams[i].GetAxis(), m44);
        dJointSetAMotorAxis(this->odeJointId, i, 0, a.x(), a.y(), a.z());
    }
}

}; // namespace Physics