#pragma once
//------------------------------------------------------------------------------
/**
    @class FX::FxMixerBase

    This is the base class which should be attached to the FxServer. It handles 
    specific effect mixing and effect applying!

    (C) 2009 Radon Labs GmbH
*/
#include "fx/rt/effects/effect.h"
#include "util/array.h"
#include "core/refcounted.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace FX
{
class EffectMixer : public Core::RefCounted
{
    __DeclareClass(EffectMixer);
public:
    /// constructor
    EffectMixer();

    /// mix the incoming effects
    virtual void Mix(const Util::Array<Ptr<Effect> >& effects, Timing::Time time);
    
    /// optionally apply mixed values 
    virtual void Apply() const;
};

}; // namespace VfxFeature
//------------------------------------------------------------------------------
