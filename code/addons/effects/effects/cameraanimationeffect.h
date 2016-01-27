#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::CameraAnimationEffect

    This effect type applies an animation to the active camera.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "effect.h"
#include "timing/timer.h"
#include "coreanimation/animresource.h"
#include "coreanimation/animsamplebuffer.h"

//------------------------------------------------------------------------------
namespace EffectsFeature
{
class CameraAnimationEffect: public Effect
{
	__DeclareClass(CameraAnimationEffect);
public:
    /// constructor
    CameraAnimationEffect();
    /// destructor
    virtual ~CameraAnimationEffect();

    /// activate the effect
    virtual void OnActivate(Timing::Time time);
    /// start the effect, called by OnFrame() after delay is over
    virtual void OnStart(Timing::Time time);
    /// trigger the effect
    virtual void OnFrame(Timing::Time time);
    /// deactivate the effect
    virtual void OnDeactivate();

    /// set animation filepath
	void SetAnimationFilePath(const Resources::ResourceId& path);
    /// set the animation positions relative
    void SetAnimationRelative(bool b);

private:
    /// setup animation resource
    bool SetupAnimation();
    /// discard animation resource
    void DiscardAnimation();
    /// start playback
    void Play(Timing::Time time);
    /// stop playback
    void Stop(Timing::Time time);

    Resources::ResourceId animPath;
    bool isRelative;
    bool isPlaying;

    Math::matrix44 initialTransform;
    Timing::Time animTime;
    Timing::Time animStartTime;
    Timing::Time animEndTime;
    Ptr<CoreAnimation::AnimResource> animResource;
    Ptr<CoreAnimation::AnimSampleBuffer> result;
};

//------------------------------------------------------------------------------
/**
*/
inline void
CameraAnimationEffect::SetAnimationFilePath(const Resources::ResourceId &path)
{
    this->animPath = path;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
CameraAnimationEffect::SetAnimationRelative( bool b )
{
	this->isRelative = b;
}

}; // namespace FX
//------------------------------------------------------------------------------