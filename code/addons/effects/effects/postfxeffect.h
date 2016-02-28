#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::PostFXEffect

    Creates PostEffect effect by applying a new preset for a set time       

    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "timing/time.h"
#include "util/string.h"
#include "effects/effect.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
{
class PostFXEffect : public Effect
{
	__DeclareClass(PostFXEffect);
public:
    /// constructor
	PostFXEffect();
    /// destructor
    virtual ~PostFXEffect();
      
    /// start the effect, called by OnFrame() after delay is over
    void OnStart(Timing::Time time);  
    /// deactivate the effect
    void OnDeactivate();
	/// set preset
	void SetPreset(const Util::String & effectPreset);

protected:
	Util::String postEffectPreset;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
PostFXEffect::SetPreset(const Util::String & effectPreset)
{
	this->postEffectPreset = effectPreset;
}

};