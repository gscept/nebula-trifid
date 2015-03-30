//------------------------------------------------------------------------------
//  havokpointtopoint.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokpointtopoint.h"

namespace Havok
{
__ImplementClass(Havok::HavokPointToPoint, 'HPTP', Physics::BasePointToPoint);

//------------------------------------------------------------------------------
/**
*/
HavokPointToPoint::HavokPointToPoint()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokPointToPoint::~HavokPointToPoint()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::Setup(const Math::vector & pivot)
{
	n_error("HavokPointToPoint::Setup: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::Setup(const Math::vector & pivotA, const Math::vector & pivotB)
{
	n_error("HavokPointToPoint::Setup: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetPivotA(const Math::vector & pivot)
{
	n_error("HavokPointToPoint::SetPivotA: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetPivotB(const Math::vector & pivot)
{
	n_error("HavokPointToPoint::SetPivotB: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
HavokPointToPoint::GetPivotA()
{
	n_error("HavokPointToPoint::GetPivotA: Not implemented");
	Math::vector dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
HavokPointToPoint::GetPivotB()
{
	n_error("HavokPointToPoint::GetPivotB: Not implemented");
	Math::vector dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetJointParams(float tau, float damping, float impulseclamp)
{
	n_error("HavokPointToPoint::SetJointParams: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::UpdateTransform(const Math::matrix44& m)
{
	n_error("HavokPointToPoint::UpdateTransform: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::RenderDebug()
{
	n_error("HavokPointToPoint::RenderDebug: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::Attach(Physics::BaseScene * world)
{
	n_error("HavokPointToPoint::Attach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::Detach()
{
	n_error("HavokPointToPoint::Detach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokPointToPoint::IsAttached() const
{
	n_error("HavokPointToPoint::IsAttached: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetEnabled(bool b)
{
	n_error("HavokPointToPoint::SetEnabled: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokPointToPoint::IsEnabled() const
{
	n_error("HavokPointToPoint::IsEnabled: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokPointToPoint::GetBody1() const
{
	n_error("HavokPointToPoint::GetBody1: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::PhysicsBody> & 
HavokPointToPoint::GetBody2() const
{
	n_error("HavokPointToPoint::GetBody2: Not implemented");
	static Ptr<Physics::PhysicsBody> dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetLinkName(const Util::String& n)
{
	n_error("HavokPointToPoint::SetLinkName: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
const Util::String& 
HavokPointToPoint::GetLinkName()
{
	n_error("HavokPointToPoint::GetLinkName: Not implemented");
	static Util::String dummy;
	return dummy;
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokPointToPoint::IsLinkValid() const
{
	n_error("HavokPointToPoint::IsLinkValid: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetLinkIndex(int i)
{
	n_error("HavokPointToPoint::SetLinkIndex: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokPointToPoint::GetLinkIndex() const
{
	n_error("HavokPointToPoint::GetLinkIndex: Not implemented");
	return -1;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetType(Physics::JointType t)
{
	n_error("HavokPointToPoint::SetType: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetBodies(const Ptr<Physics::PhysicsBody> & body1, const Ptr<Physics::PhysicsBody> & body2)
{
	n_error("HavokPointToPoint::SetBodies: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetBreakThreshold(float threshold)
{
	n_error("HavokPointToPoint::SetBreakThreshold: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokPointToPoint::GetBreakThreshold()
{
	n_error("HavokPointToPoint::GetBreakThreshold: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetERP(float ERP, int axis /*= 0 */)
{
	n_error("HavokPointToPoint::SetERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetStoppingERP(float ERP, int axis /*= 0*/)
{
	n_error("HavokPointToPoint::SetStoppingERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetStoppingCFM(float CFM, int axis /*= 0*/)
{
	n_error("HavokPointToPoint::SetStoppingCFM: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokPointToPoint::GetERP(int axis /*= 0*/)
{
	n_error("HavokPointToPoint::GetERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokPointToPoint::GetCFM(int axis /*= 0*/)
{
	n_error("HavokPointToPoint::GetCFM: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokPointToPoint::GetStoppingERP(int axis /*= 0*/)
{
	n_error("HavokPointToPoint::GetStoppingERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokPointToPoint::GetStoppingCFM(int axis /*= 0*/)
{
	n_error("HavokPointToPoint::GetStoppingCFM: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokPointToPoint::SetCFM(float CFM, int axis /*= 0*/)
{
	n_error("HavokPointToPoint::SetCFM: Not implemented");
}


}