//------------------------------------------------------------------------------
//  havokjoint.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokjoint.h"

namespace Havok
{
using namespace Physics;

__ImplementClass(Havok::HavokJoint,'HKJT', Physics::BaseJoint);

//------------------------------------------------------------------------------
/**
*/
HavokJoint::HavokJoint()
{
}

//------------------------------------------------------------------------------
/**
*/
HavokJoint::~HavokJoint()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::UpdateTransform(const Math::matrix44& m)
{
	n_error("HavokJoint::UpdateTransform: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::RenderDebug()
{
	n_error("HavokJoint::RenderDebug: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::Attach(Physics::BaseScene* world)
{
	n_error("HavokJoint::Attach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::Detach()
{
	n_error("HavokJoint::Detach: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokJoint::IsAttached() const
{
	n_error("HavokJoint::IsAttached: Not implemented");
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::SetEnabled(bool b)
{
	n_error("HavokJoint::SetEnabled: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::SetBreakThreshold(float threshold)
{
	n_error("HavokJoint::SetBreakThreshold: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokJoint::GetBreakThreshold()
{
	n_error("HavokJoint::GetBreakThreshold: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::SetERP(float ERP, int axis /*= 0*/)
{
	n_error("HavokJoint::SetERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokJoint::GetERP(int axis /*= 0*/)
{
	n_error("HavokJoint::GetERP: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::SetCFM(float CFM, int axis /*= 0*/)
{
	n_error("HavokJoint::SetCFM: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokJoint::GetCFM(int axis /*= 0*/)
{
	n_error("HavokJoint::GetCFM: Not implemented");
	return -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::SetStoppingERP(float ERP, int axis /*= 0*/)
{
	n_error("HavokJoint::SetStoppingERP: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokJoint::GetStoppingERP(int axis /*= 0*/)
{
	n_error("HavokJoint::GetStoppingERP: Not implemented");
	return 1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokJoint::SetStoppingCFM(float CFM, int axis /*= 0*/)
{
	n_error("HavokJoint::SetStoppingCFM: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokJoint::GetStoppingCFM(int axis /*= 0*/)
{
	n_error("HavokJoint::GetStoppingCFM: Not implemented");
	return -1.0f;
}

}