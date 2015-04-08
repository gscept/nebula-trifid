#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::AnimEventRegistry
    
    Central registry for all anim event tables.

    NOTE: the AnimEventRegistry will be setup in the main thread, but 
    mainly be called from the render thread from inside the
    anim event handlers.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "threading/criticalsection.h"
#include "core/singleton.h"
#include "util/stringatom.h"
#include "util/dictionary.h"
#include "timing/time.h"
#include "graphics/attachmentserver.h"
#include "attr/attribute.h"
#include "faudio/eventid.h"
#include "animation/animeventhandlerbase.h"
#include "graphicsfeature/managers/attachmentmanager.h"


//------------------------------------------------------------------------------
namespace EffectsFeature
{

class AnimEventRegistry : public Core::RefCounted
{
    __DeclareClass(AnimEventRegistry);
    __DeclareInterfaceSingleton(AnimEventRegistry);
public:

    /// ---------------- structures for the discrete anim event types
    
    /// anim event attachment information 
    struct AttachmentEvent
    {
        Util::StringAtom name;
        Util::StringAtom resource;
        Util::StringAtom jointName;
        Timing::Time duration;        
		GraphicsFeature::AttachmentManager::AttachmentRotation rotation;

        AttachmentEvent()
		{
			duration = 0;             
			rotation = GraphicsFeature::AttachmentManager::Joint;
		};
    };

    /* Functionality of the TimingEvent structure

        factor
		^
        |           _____________________
        |          /.                   .\
        |         / .                   . \
        |        /  .                   .  \
        |       /   .                   .   \
        |      /    .                   .    \
        |     /     .                   .     \
        |    /      .                   .      \
        +--------------------------------------------> time
            [======][===================][======]
             Attack       Sustain        Release             
	*/

    struct TimingEvent
    {
        float factor;
        Timing::Tick attack;
        Timing::Tick sustain;
        Timing::Tick release;
        Timing::Tick startTime;
    };

    /// structure for vibration information
    struct VibrationEvent
    {
        Timing::Time highFreqDuration;
        Timing::Time lowFreqDuration;
        float highFreqIntensity;
        float lowFreqIntensity;
        IndexT playerIndex;
    };


    /// shake structure
    struct ShakeEvent
    {
        Timing::Time duration;
        Math::vector intensity;
        Math::vector rotation;
        float range;
    };

    /// constructor
    AnimEventRegistry();
    /// destructor
    virtual ~AnimEventRegistry();
    
    /// setup the anim event registry and register animeventhandlers
    void Setup();
    /// discard the anim event registry
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;    

    /// checks if exists
    bool HasAttachmentEvent(const Util::StringAtom& eventName) const;
    /// checks if exists
    bool HasCamShakeEvent(const Util::StringAtom& eventName) const;
    /// checks if exists
    bool HasSoundEvent(const Util::StringAtom& eventName) const;
    /// checks if exists
    bool HasTimingEvent(const Util::StringAtom& eventName) const;
    /// checks if exists
    bool HasVibrationEvent(const Util::StringAtom& eventName) const;

    /// return requested event
    const AttachmentEvent& GetAttachmentEvent(const Util::StringAtom& eventName) const;
    /// return requested event
    const ShakeEvent& GetCamShakeEvent(const Util::StringAtom& eventName) const;
    /// return requested event
    const FAudio::EventId& GetSoundEvent(const Util::StringAtom& eventName) const;
    /// return requested event
    const TimingEvent& GetTimingEvent(const Util::StringAtom& eventName) const;
    /// return requested event
    const VibrationEvent& GetVibrationEvent(const Util::StringAtom& eventName) const;

private:
    /// load attachment data
    void LoadAttachmentEvents(const Util::StringAtom& tableName);
    /// load shake data
    void LoadCamShakeEvents(const Util::StringAtom& tableName);
    /// load sound data
    void LoadSoundEvents(const Util::StringAtom& tableName);
    /// load timing data
    void LoadTimingEvents(const Util::StringAtom& tableName);
    /// load vibration data
    void LoadVibrationEvents(const Util::StringAtom& tableName);

    bool isValid;

    /// the data
    Util::Dictionary<Util::StringAtom, AttachmentEvent> animEventAttachments;
    Util::Dictionary<Util::StringAtom, ShakeEvent> animEventCamShakes;
	Util::Dictionary<Util::StringAtom, FAudio::EventId> animEventSounds;
    Util::Dictionary<Util::StringAtom, TimingEvent> animEventTimer;
    Util::Dictionary<Util::StringAtom, VibrationEvent> animEventVibs;

	/// default animevent handlers
	Ptr<Animation::AnimEventHandlerBase> timingHandler;
	Ptr<Animation::AnimEventHandlerBase> vibrationHandler;
	Ptr<Animation::AnimEventHandlerBase> soundHandler;
	Ptr<Animation::AnimEventHandlerBase> shakeHandler;
	Ptr<Animation::AnimEventHandlerBase> attachmentHandler;

};

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimEventRegistry::HasAttachmentEvent(const Util::StringAtom& eventName) const
{
    return this->animEventAttachments.Contains(eventName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimEventRegistry::HasCamShakeEvent(const Util::StringAtom& eventName) const
{
    return this->animEventCamShakes.Contains(eventName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimEventRegistry::HasSoundEvent(const Util::StringAtom& eventName) const
{
    return this->animEventSounds.Contains(eventName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimEventRegistry::HasTimingEvent(const Util::StringAtom& eventName) const
{
    return this->animEventTimer.Contains(eventName);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimEventRegistry::HasVibrationEvent(const Util::StringAtom& eventName) const
{
    return this->animEventVibs.Contains(eventName);
}

//------------------------------------------------------------------------------
/**
*/
inline
const AnimEventRegistry::AttachmentEvent&
AnimEventRegistry::GetAttachmentEvent(const Util::StringAtom &eventName) const
{
    IndexT indeX = this->animEventAttachments.FindIndex(eventName);
    return this->animEventAttachments.ValueAtIndex(indeX);
}

//------------------------------------------------------------------------------
/**
*/
inline
const AnimEventRegistry::ShakeEvent&
AnimEventRegistry::GetCamShakeEvent(const Util::StringAtom &eventName) const
{
    IndexT indeX = this->animEventCamShakes.FindIndex(eventName);
    return this->animEventCamShakes.ValueAtIndex(indeX);
}

//------------------------------------------------------------------------------
/**
*/
inline
const FAudio::EventId&
AnimEventRegistry::GetSoundEvent(const Util::StringAtom &eventName) const
{
    IndexT indeX = this->animEventSounds.FindIndex(eventName);
    return this->animEventSounds.ValueAtIndex(indeX);
}

//------------------------------------------------------------------------------
/**
*/
inline
const AnimEventRegistry::TimingEvent&
AnimEventRegistry::GetTimingEvent(const Util::StringAtom &eventName) const
{
    IndexT indeX = this->animEventTimer.FindIndex(eventName);
    return this->animEventTimer.ValueAtIndex(indeX);
}

//------------------------------------------------------------------------------
/**
*/
inline
const AnimEventRegistry::VibrationEvent&
AnimEventRegistry::GetVibrationEvent(const Util::StringAtom &eventName) const
{
    IndexT indeX = this->animEventVibs.FindIndex(eventName);
    return this->animEventVibs.ValueAtIndex(indeX);
}
} // namespace FX
//------------------------------------------------------------------------------
