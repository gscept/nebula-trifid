//------------------------------------------------------------------------------
//  havokhinge.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokhinge.h"

namespace Havok
{
__ImplementClass(Havok::HavokHinge, 'HKPM', Physics::BaseHinge);

//------------------------------------------------------------------------------
/**
*/
HavokHinge::HavokHinge()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokHinge::~HavokHinge()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::Setup(const Math::vector & pivot, const Math::vector & axis)
{
	n_error("HavokHinge::Setup: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::Setup(const Math::vector & pivotA, const Math::vector & axisInA, const Math::vector & pivotB, const Math::vector & axisInB)
{
	n_error("HavokHinge::Setup: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetLimits(float low, float high)
{
	n_error("HavokHinge::SetLimits: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetLowLimit()
{
	n_error("HavokHinge::GetLowLimit: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetHighLimit()
{
	n_error("HavokHinge::GetHighLimit: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetAxis(const Math::vector & axis)
{
	n_error("HavokHinge::SetAxis: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetAngularMotor(float targetVelocity, float maxImpulse)
{
	n_error("HavokHinge::SetAngularMotor: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetEnableAngularMotor(bool enable)
{
	n_error("HavokHinge::SetEnableAngularMotor: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetHingeAngle()
{
	n_error("HavokHinge::GetHingeAngle: Not implemented");
	return -1.0f;
}


//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::UpdateTransform(const Math::matrix44& m)
{
	n_error("HavokHinge::UpdateTransform: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::RenderDebug()
{
	n_error("HavokHinge::RenderDebug: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::Attach(Physics::BaseScene * world)
{
	n_error("HavokHinge::Attach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::Detach()
{
	n_error("HavokHinge::Detach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokHinge::IsAttached() const
{
	n_error("HavokHinge::IsAttached: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetEnabled(bool b)
{
	n_error("HavokHinge::SetEnabled: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokHinge::IsEnabled() const
{
	n_error("HavokHinge::IsEnabled: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokHinge::GetBody1() const
{
	n_error("HavokHinge::GetBody1: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokHinge::GetBody2() const
{
	n_error("HavokHinge::GetBody2: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetLinkName(const Util::String& n)
{
	n_error("HavokHinge::SetLinkName: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
const Util::String& 
HavokHinge::GetLinkName()
{
	n_error("HavokHinge::GetLinkName: Not implemented");
	static Util::String dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokHinge::IsLinkValid() const
{
	n_error("HavokHinge::IsLinkValid: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetLinkIndex(int i)
{
	n_error("HavokHinge::SetLinkIndex: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokHinge::GetLinkIndex() const
{
	n_error("HavokHinge::GetLinkIndex: Not implemented");
	return -1;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetType(Physics::JointType t)
{
	n_error("HavokHinge::SetType: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetBodies(const Ptr<Physics::PhysicsBody> & body1, const Ptr<Physics::PhysicsBody> & body2)
{
	n_error("HavokHinge::SetBodies: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetBreakThreshold(float threshold)
{
	n_error("HavokHinge::SetBreakThreshold: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetBreakThreshold()
{
	n_error("HavokHinge::GetBreakThreshold: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetERP(float ERP, int axis /*= 0 */)
{
	n_error("HavokHinge::SetERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetStoppingERP(float ERP, int axis /*= 0*/)
{
	n_error("HavokHinge::SetStoppingERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokHinge::SetStoppingCFM(float CFM, int axis /*= 0*/)
{
	n_error("HavokHinge::SetStoppingCFM: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetERP(int axis /*= 0*/)
{
	n_error("HavokHinge::GetERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetCFM(int axis /*= 0*/)
{
	n_error("HavokHinge::GetCFM: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetStoppingERP(int axis /*= 0*/)
{
	n_error("HavokHinge::GetStoppingERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokHinge::GetStoppingCFM(int axis /*= 0*/)
{
	n_error("HavokHinge::GetStoppingCFM: Not implemented");
	return -1.0f;
}

}