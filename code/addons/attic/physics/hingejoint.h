#ifndef PHYSICS_HINGEJOINT_H
#define PHYSICS_HINGEJOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::HingeJoint

    A hinge joint. See ODE docs for details.

    (C) 2003 RadonLabs GmbH
*/
#include "physics/joint.h"
#include "physics/jointaxis.h"

//------------------------------------------------------------------------------
namespace Physics
{
class HingeJoint : public Joint
{
	__DeclareClass(HingeJoint);
public:
    /// constructor
    HingeJoint();
    /// destructor
    virtual ~HingeJoint();
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m);
    /// transform anchor and axis position/orientation into worldspace and set it in ode
    virtual void UpdateTransform(const Math::matrix44& m);
    /// render debug visualization
    virtual void RenderDebug();
    /// set local anchor position to `v'
    void SetAnchor(const Math::vector& v);
    /// local anchor position in world coordinates.
    const Math::vector& GetAnchor() const;
    /// read/write access to axis parameters
    JointAxis& AxisParams();

private:
    Math::vector anchor;
    JointAxis axisParams;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
HingeJoint::SetAnchor(const Math::vector& v)
{
    this->anchor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector& 
HingeJoint::GetAnchor() const
{
    return this->anchor;
}

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis&
HingeJoint::AxisParams()
{
    return this->axisParams;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
