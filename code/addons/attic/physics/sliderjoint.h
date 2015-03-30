#ifndef PHYSICS_SLIDERJOINT_H
#define PHYSICS_SLIDERJOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::SliderJoint

    A slider joint (see ODE docs for details).

    (C) 2003 RadonLabs GmbH
*/
#include "physics/joint.h"
#include "physics/jointaxis.h"

//------------------------------------------------------------------------------
namespace Physics
{
class SliderJoint : public Joint
{
	__DeclareClass(SliderJoint);
public:
    /// constructor
    SliderJoint();
    /// destructor
    virtual ~SliderJoint();
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m);
    /// transform anchor and axis position/orientation into worldspace and set it in ode
    virtual void UpdateTransform(const Math::matrix44& m);
    /// read/write access to axis parameters
    JointAxis& AxisParams();

private:
    JointAxis axisParams;
};

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis&
SliderJoint::AxisParams()
{
    return this->axisParams;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
