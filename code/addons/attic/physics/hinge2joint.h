#ifndef PHYSICS_HINGE2JOINT_H
#define PHYSICS_HINGE2JOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::Hinge2Joint
    
    A hinge2 joint. See ODE docs for details.
    
    (C) 2005 RadonLabs GmbH
*/
#include "physics/joint.h"
#include "physics/jointaxis.h"
#include "util/fixedarray.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Hinge2Joint : public Joint
{
	__DeclareClass(Hinge2Joint);
public:
    /// constructor
    Hinge2Joint();
    /// destructor
    virtual ~Hinge2Joint();
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
    /// set suspension ERP
    void SetSuspensionERP(float f);
    /// get suspension ERP
    float GetSuspensionERP() const;
    /// set suspension CFM
    void SetSuspensionCFM(float f);
    /// get suspension CFM
    float GetSuspensionCFM() const;
    /// read/write access to axis parameters
    JointAxis& AxisParams(uint axisIndex);

private:
    Math::vector anchor;
    float suspErp;
    float suspCfm;
    Util::FixedArray<JointAxis> axisParams;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
Hinge2Joint::SetAnchor(const Math::vector& v)
{
    this->anchor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector& 
Hinge2Joint::GetAnchor() const
{
    return this->anchor;
}

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis&
Hinge2Joint::AxisParams(uint axisIndex)
{
    return this->axisParams[axisIndex];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Hinge2Joint::SetSuspensionERP(float f)
{
    this->suspErp = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Hinge2Joint::GetSuspensionERP() const
{
    return this->suspErp;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Hinge2Joint::SetSuspensionCFM(float f)
{
    this->suspCfm = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Hinge2Joint::GetSuspensionCFM() const
{
    return this->suspCfm;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
