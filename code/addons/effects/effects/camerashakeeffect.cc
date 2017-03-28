//------------------------------------------------------------------------------
//  camerashakeeffect.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "camerashakeeffect.h"
#include "math/vector.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::CameraShakeEffect, 'FXSH', EffectsFeature::Effect);

//------------------------------------------------------------------------------
/**
*/
CameraShakeEffect::CameraShakeEffect() :
    range(1.0f),
    intensity(Math::vector::nullvec()),
    curIntensity(Math::vector::nullvec())
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CameraShakeEffect::OnStart(Timing::Time time)
{
    Effect::OnStart(time);
    this->curIntensity = this->intensity;
    this->curRotation = this->rotation;
}

//------------------------------------------------------------------------------
/**
*/
void
CameraShakeEffect::OnFrame(Timing::Time time)
{
    Effect::OnFrame(time);

    if (this->IsPlaying())
    {
        // drop off shake intensity by time
        Timing::Time age = time - this->startTime;
        if (age <= this->GetDuration())
        {
            this->curIntensity = this->intensity * (1.0f - Math::n_saturate(float(age / this->duration)));
            this->curRotation = this->rotation * (1.0f - Math::n_saturate(float(age / this->duration)));
        }
    }
}
}; // namespace FX