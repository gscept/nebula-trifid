#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::EffectMixer

    This is the base class which should be attached to the FxServer. It handles 
    specific effect mixing and effect applying!

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "effects/effects/effect.h"
#include "util/array.h"
#include "core/refcounted.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
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
