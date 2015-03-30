#ifndef PHYSICS_UNIVERSALJOINT_H
#define PHYSICS_UNIVERSALJOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::UniversalJoint

    A universal joint. See ODE docs for details.
    
    (C) 2003 RadonLabs GmbH
*/
#include "physics/joint.h"
#include "physics/jointaxis.h"
#include "util/fixedarray.h"

//------------------------------------------------------------------------------
namespace Physics
{
class UniversalJoint : public Joint
{
	__DeclareClass(UniversalJoint);
public:
    /// constructor
    UniversalJoint();
    /// destructor
    virtual ~UniversalJoint();
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m);
    /// transform anchor and axis position/orientation into worldspace and set it in ode.
    virtual void UpdateTransform(const Math::matrix44& m);
    /// render debug visualization
    virtual void RenderDebug();
    /// set anchor position
    void SetAnchor(const Math::vector& v);
    /// get anchor position
    const Math::vector& GetAnchor() const;
    /// read/write access to axis parameters
    JointAxis& AxisParams(uint axisIndex) const;

private:
    Math::vector anchor;
    Util::FixedArray<JointAxis> axisParams;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
UniversalJoint::SetAnchor(const Math::vector& v)
{
    this->anchor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector& 
UniversalJoint::GetAnchor() const
{
    return this->anchor;
}

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis&
UniversalJoint::AxisParams(uint axisIndex) const
{
    return this->axisParams[axisIndex];
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
