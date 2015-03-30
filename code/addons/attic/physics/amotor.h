#ifndef PHYSICS_AMOTOR_H
#define PHYSICS_AMOTOR_H
//------------------------------------------------------------------------------
/**
    @class Physics::AMotor

    An angular motor joint. This will just constraint angles, not positions.
    Can be used together with a ball joint if additional position constraints
    are needed.

    (C) 2005 Radon Labs GmbH
*/
#include "physics/joint.h"
#include "physics/jointaxis.h"
#include "util/fixedarray.h"

//------------------------------------------------------------------------------
namespace Physics
{
class AMotor : public Joint
{
	__DeclareClass(AMotor);
public:
    /// constructor
    AMotor();
    /// destructor
    virtual ~AMotor();
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m);
    /// transform anchor and axis position/orientation into worldspace and set it in ode
    virtual void UpdateTransform(const Math::matrix44& m);
    /// set number of axes (0..3)
    void SetNumAxes(uint num);
    /// get number of axes
    uint GetNumAxes() const;
    /// read/write access to axis parameters
    JointAxis& AxisParams(uint axisIndex) const;
    /// update axis parameter velocity in ode joint motor
    void UpdateVelocity(uint axisIndex);

private:
    Util::FixedArray<JointAxis> axisParams;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
AMotor::SetNumAxes(uint num)
{
    n_assert(num <= 3);
    this->axisParams.SetSize(num);
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
AMotor::GetNumAxes() const
{
    return this->axisParams.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis&
AMotor::AxisParams(uint axisIndex) const
{
    return this->axisParams[axisIndex];
}

}; // namespace Physics

//------------------------------------------------------------------------------
#endif
