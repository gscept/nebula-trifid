//------------------------------------------------------------------------------
//  postfxeffect.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "postfxeffect.h"
#include "posteffect/posteffectfeatureunit.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::PostFXEffect, 'PXEF',Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
PostFXEffect::PostFXEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PostFXEffect::~PostFXEffect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
    Starts the effect. This is either called directly by OnActivate() if
    no start delay is defined, or after the start delay is over from
    OnFrame().
*/
void
PostFXEffect::OnStart(Timing::Time time)
{
	Effect::OnStart(time);
	Util::String last = PostEffect::PostEffectFeatureUnit::Instance()->GetLastPreset();
	PostEffect::PostEffectFeatureUnit::Instance()->ApplyPreset(this->postEffectPreset);
	this->postEffectPreset = last;
}

//------------------------------------------------------------------------------
/**
    Deactivates the effect and sets its state to finished. This is usually
    called from OnFrame() when the effect has expired.
*/
void
PostFXEffect::OnDeactivate()
{
	PostEffect::PostEffectFeatureUnit::Instance()->ApplyPreset(this->postEffectPreset);
	Effect::OnDeactivate();
}

} // namespace Effect