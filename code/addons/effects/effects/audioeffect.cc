//------------------------------------------------------------------------------
//  audioeffect.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "audioeffect.h"
#include "faudio\audiodevice.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::AudioEffect, 'AUEF', EffectsFeature::Effect);

//------------------------------------------------------------------------------
/**
*/
AudioEffect::AudioEffect()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AudioEffect::~AudioEffect()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
AudioEffect::OnStart(Timing::Time time)
{
	FAudio::AudioDevice::Instance()->EventPlayFireAndForget3D(this->event, this->transform, this->velocity, this->volume);	
}

} // namespace FX