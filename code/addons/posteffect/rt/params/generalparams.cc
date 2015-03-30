//------------------------------------------------------------------------------
//  generalparams.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "generalparams.h"

namespace PostEffect
{

__ImplementClass(PostEffect::GeneralParams, 'PEGP', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
GeneralParams::GeneralParams() :
name("Default"),
blendTime(0.0f)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GeneralParams::~GeneralParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
GeneralParams::Copy(const Ptr<GeneralParams>& rhs)
{
	this->name = rhs->name;
	this->blendTime = rhs->blendTime;
}


} // namespace PostEffects
