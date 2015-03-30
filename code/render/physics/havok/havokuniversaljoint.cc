//------------------------------------------------------------------------------
//  havokuniversaljoint.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokuniversaljoint.h"

namespace Havok
{
__ImplementClass(Havok::HavokUniversalJoint, 'HKUJ', Physics::BaseUniversalJoint);

//------------------------------------------------------------------------------
/**
*/
HavokUniversalJoint::HavokUniversalJoint()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokUniversalJoint::~HavokUniversalJoint()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::Setup(const Math::vector & anchor, const Math::vector & axisA, const Math::vector & axisB)
{
	n_error("HavokUniversalJoint::Setup: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokUniversalJoint::GetAxisAngleA()
{
	n_error("HavokUniversalJoint::GetAxisAngleA: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokUniversalJoint::GetAxisAngleB()
{
	n_error("HavokUniversalJoint::GetAxisAngleB: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetLowLimits(float A, float B)
{
	n_error("HavokUniversalJoint::SetLowLimits: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetHighLimits(float A, float B)
{
	n_error("HavokUniversalJoint::SetHighLimits: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::UpdateTransform(const Math::matrix44& m)
{
	n_error("HavokUniversalJoint::UpdateTransform: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::RenderDebug()
{
	n_error("HavokUniversalJoint::RenderDebug: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::Attach(Physics::BaseScene * world)
{
	n_error("HavokUniversalJoint::Attach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::Detach()
{
	n_error("HavokUniversalJoint::Detach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokUniversalJoint::IsAttached() const
{
	n_error("HavokUniversalJoint::IsAttached: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetEnabled(bool b)
{
	n_error("HavokUniversalJoint::SetEnabled: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokUniversalJoint::IsEnabled() const
{
	n_error("HavokUniversalJoint::IsEnabled: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokUniversalJoint::GetBody1() const
{
	n_error("HavokUniversalJoint::GetBody1: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokUniversalJoint::GetBody2() const
{
	n_error("HavokUniversalJoint::GetBody2: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetLinkName(const Util::String& n)
{
	n_error("HavokUniversalJoint::SetLinkName: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
const Util::String& 
HavokUniversalJoint::GetLinkName()
{
	n_error("HavokUniversalJoint::GetLinkName: Not implemented");
	static Util::String dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokUniversalJoint::IsLinkValid() const
{
	n_error("HavokUniversalJoint::IsLinkValid: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetLinkIndex(int i)
{
	n_error("HavokUniversalJoint::SetLinkIndex: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokUniversalJoint::GetLinkIndex() const
{
	n_error("HavokUniversalJoint::GetLinkIndex: Not implemented");
	return -1;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetType(Physics::JointType t)
{
	n_error("HavokUniversalJoint::SetType: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetBodies(const Ptr<Physics::PhysicsBody> & body1, const Ptr<Physics::PhysicsBody> & body2)
{
	n_error("HavokUniversalJoint::SetBodies: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetBreakThreshold(float threshold)
{
	n_error("HavokUniversalJoint::SetBreakThreshold: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokUniversalJoint::GetBreakThreshold()
{
	n_error("HavokUniversalJoint::GetBreakThreshold: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetERP(float ERP, int axis /*= 0 */)
{
	n_error("HavokUniversalJoint::SetERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetStoppingERP(float ERP, int axis /*= 0*/)
{
	n_error("HavokUniversalJoint::SetStoppingERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokUniversalJoint::SetStoppingCFM(float CFM, int axis /*= 0*/)
{
	n_error("HavokUniversalJoint::SetStoppingCFM: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokUniversalJoint::GetERP(int axis /*= 0*/)
{
	n_error("HavokUniversalJoint::GetERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokUniversalJoint::GetCFM(int axis /*= 0*/)
{
	n_error("HavokUniversalJoint::GetCFM: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokUniversalJoint::GetStoppingERP(int axis /*= 0*/)
{
	n_error("HavokUniversalJoint::GetStoppingERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokUniversalJoint::GetStoppingCFM(int axis /*= 0*/)
{
	n_error("HavokUniversalJoint::GetStoppingCFM: Not implemented");
	return -1.0f;
}

}