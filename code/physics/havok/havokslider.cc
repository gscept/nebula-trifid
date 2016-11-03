//------------------------------------------------------------------------------
//  havokslider.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokslider.h"

namespace Havok
{
__ImplementClass(Havok::HavokSlider, 'HKUJ', Physics::BaseSlider);

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetAngularLimits(float low, float high)
{
	n_error("HavokSlider::SetAngularLimits: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetAngularLowLimit()
{
	n_error("HavokSlider::GetAngularLowLimit: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetAngularHighLimit()
{
	n_error("HavokSlider::GetAngularHighLimit: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetLinearLimits(float low, float high)
{
	n_error("HavokSlider::SetLinearLimits: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetLinearLowLimit()
{
	n_error("HavokSlider::GetLinearLowLimit: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetLinearHighLimit()
{
	n_error("HavokSlider::GetLinearHighLimit: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetAxis(const Math::vector & axis)
{
	n_error("HavokSlider::SetAxis: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetAngularMotor(float targetVelocity, float maxImpulse)
{
	n_error("HavokSlider::SetAngularMotor: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetEnableAngularMotor(bool enable)
{
	n_error("HavokSlider::SetEnableAngularMotor: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetLinearMotor(float targetVelocity, float maxImpulse)
{
	n_error("HavokSlider::SetLinearMotor: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetEnableLinearMotor(bool enable)
{
	n_error("HavokSlider::SetEnableLinearMotor: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetLinearPosition()
{
	n_error("HavokSlider::GetLinearPosition: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetAngularPosition()
{
	n_error("HavokSlider::GetAngularPosition: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::UpdateTransform(const Math::matrix44& m)
{
	n_error("HavokSlider::UpdateTransform: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::RenderDebug()
{
	n_error("HavokSlider::RenderDebug: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::Attach(Physics::BaseScene * world)
{
	n_error("HavokSlider::Attach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::Detach()
{
	n_error("HavokSlider::Detach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokSlider::IsAttached() const
{
	n_error("HavokSlider::IsAttached: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetEnabled(bool b)
{
	n_error("HavokSlider::SetEnabled: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokSlider::IsEnabled() const
{
	n_error("HavokSlider::IsEnabled: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokSlider::GetBody1() const
{
	n_error("HavokSlider::GetBody1: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokSlider::GetBody2() const
{
	n_error("HavokSlider::GetBody2: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetLinkName(const Util::String& n)
{
	n_error("HavokSlider::SetLinkName: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
const Util::String& 
HavokSlider::GetLinkName()
{
	n_error("HavokSlider::GetLinkName: Not implemented");
	static Util::String dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokSlider::IsLinkValid() const
{
	n_error("HavokSlider::IsLinkValid: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetLinkIndex(int i)
{
	n_error("HavokSlider::SetLinkIndex: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokSlider::GetLinkIndex() const
{
	n_error("HavokSlider::GetLinkIndex: Not implemented");
	return -1;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetType(Physics::JointType t)
{
	n_error("HavokSlider::SetType: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetBodies(const Ptr<Physics::PhysicsBody> & body1, const Ptr<Physics::PhysicsBody> & body2)
{
	n_error("HavokSlider::SetBodies: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetBreakThreshold(float threshold)
{
	n_error("HavokSlider::SetBreakThreshold: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetBreakThreshold()
{
	n_error("HavokSlider::GetBreakThreshold: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetERP(float ERP, int axis /*= 0 */)
{
	n_error("HavokSlider::SetERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetStoppingERP(float ERP, int axis /*= 0*/)
{
	n_error("HavokSlider::SetStoppingERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::SetStoppingCFM(float CFM, int axis /*= 0*/)
{
	n_error("HavokSlider::SetStoppingCFM: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetERP(int axis /*= 0*/)
{
	n_error("HavokSlider::GetERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetCFM(int axis /*= 0*/)
{
	n_error("HavokSlider::GetCFM: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetStoppingERP(int axis /*= 0*/)
{
	n_error("HavokSlider::GetStoppingERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokSlider::GetStoppingCFM(int axis /*= 0*/)
{
	n_error("HavokSlider::GetStoppingCFM: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokSlider::Setup(const Math::matrix44 & frameA, const Math::matrix44 & frameB)
{
	n_error("HavokSlider::Setup: Not implemented");
}

}