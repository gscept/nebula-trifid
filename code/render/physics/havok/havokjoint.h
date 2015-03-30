#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokJoint

	>> Work in progress, will not work! <<
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/basejoint.h"

namespace Physics
{
	class PhysicsBody;
}

namespace Havok
{
class HavokJoint : public Physics::BaseJoint
{
	__DeclareClass(HavokJoint);
public:
	/// default constructor
	HavokJoint();
	/// destructor
	~HavokJoint();

	/// update position and orientation
	virtual void UpdateTransform(const Math::matrix44& m);
	/// render debug visualization
	virtual void RenderDebug();

	/// attach to scene
	virtual void Attach(Physics::BaseScene* world);
	/// detach from scene
	virtual void Detach();
	/// is attached
	virtual bool IsAttached() const;
	/// set enable
	virtual void SetEnabled(bool b);

	/// set the breaking threshold
	virtual void SetBreakThreshold(float threshold);
	/// get the breaking threshold
	virtual float GetBreakThreshold();

	//TODO: What the hell is ERP
	/// derp
	virtual void SetERP(float ERP, int axis = 0);
	/// derp2
	virtual float GetERP(int axis = 0);
	/// set constraint force mixing
	virtual void SetCFM(float CFM, int axis = 0);
	/// get constraint force mixing
	virtual float GetCFM(int axis = 0);

	/// derp3
	virtual void SetStoppingERP(float ERP, int axis = 0);
	/// derp4
	virtual float GetStoppingERP(int axis = 0);
	/// set stopping constraint force mixing
	virtual void SetStoppingCFM(float CFM, int axis = 0);
	/// get stopping constraint force mixing
	virtual float GetStoppingCFM(int axis = 0);
};

}