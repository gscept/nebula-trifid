#ifndef PHYSICS_BALLJOINT_H
#define PHYSICS_BALLJOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::BallJoint

    A ball-and-socket joint (see ODE docs for details).

    (C) 2003 RadonLabs GmbH
*/
#include "physics/joint.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Physics
{
class BallJoint : public Joint
{
	__DeclareClass(BallJoint);
public:
    /// constructor
    BallJoint();
    /// destructor
    virtual ~BallJoint();
    /// initialize the joint (attach to world)
    virtual void Attach(dWorldID worldID, dJointGroupID groupID, const Math::matrix44& m);
    /// Update position and orientation.
    virtual void UpdateTransform(const Math::matrix44& m);
    /// render debug visualization
    virtual void RenderDebug();
    /// Set local anchor position to `v'.
    void SetAnchor(const Math::vector& v);
    /// Local anchor position in world coordinates.
    const Math::vector& GetAnchor() const;

private:
    Math::vector anchor;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
BallJoint::SetAnchor(const Math::vector& v)
{
    this->anchor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector& 
BallJoint::GetAnchor() const
{
    return this->anchor;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
