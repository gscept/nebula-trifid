#pragma once
//------------------------------------------------------------------------------
/**
    @class Physics::BaseUniversalJoint

    A joint (also known as constraint) connects two Actors    

    (C) (C) 2012 Johannes Hirche, LTU Skelleftea
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "physics/joint.h"

//------------------------------------------------------------------------------
namespace Physics
{

class BaseUniversalJoint : public Joint
{
    __DeclareAbstractClass(BaseUniversalJoint);
public:
    

    /// default constructor
	BaseUniversalJoint() {}    

	virtual void Setup(const Math::vector & anchor, const Math::vector & axisA, const Math::vector & axisB ) = 0;
	
	virtual float GetAxisAngleA() = 0;
	virtual float GetAxisAngleB() = 0;

	virtual void SetLowLimits(float A, float B) = 0;
	virtual void SetHighLimits(float A, float B) = 0;
};
}

