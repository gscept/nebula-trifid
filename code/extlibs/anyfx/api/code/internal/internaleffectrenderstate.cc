//------------------------------------------------------------------------------
//  internaleffectrenderstate.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "internaleffectrenderstate.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
InternalEffectRenderState::InternalEffectRenderState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
InternalEffectRenderState::~InternalEffectRenderState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectRenderState::Apply()
{
	// override me
}

//------------------------------------------------------------------------------
/**
*/
void 
InternalEffectRenderState::Reset()
{
    this->renderSettings = this->defaultRenderSettings;
}
} // namespace AnyFX