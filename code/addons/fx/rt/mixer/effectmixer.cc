//------------------------------------------------------------------------------
//  fx/rt/mixer/effectmixer.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/mixer/effectmixer.h"

namespace FX
{
__ImplementClass(FX::EffectMixer, 'CFEM', Core::RefCounted);

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
