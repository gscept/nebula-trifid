//------------------------------------------------------------------------------
//  effectmixer.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effectmixer.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::EffectMixer, 'CFEM', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
EffectMixer::EffectMixer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
EffectMixer::Mix(const Util::Array<Ptr<Effect> >& effects, Timing::Time time)
{
    // override in subclass
    n_error("FxMixerBase::Mix -> Mix-method seems not to be overwritten!");
}

//------------------------------------------------------------------------------
/**
*/
void
EffectMixer::Apply() const
{
    // optionally override in subclass
    n_warning("EffectMixer::Apply() called! (.. as intended?)\n");
}
};
