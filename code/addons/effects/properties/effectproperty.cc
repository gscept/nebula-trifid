//------------------------------------------------------------------------------
//  effectproperty.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effects/properties/effectproperty.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegamefeature/basegametiming/timemanager.h"
#include "../effectsfeatureunit.h"
#include "faudio/audiodevice.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::EffectProperty, 'FXPR', Game::Property);

using namespace EffectsFeature;

//------------------------------------------------------------------------------
/**
*/
EffectProperty::EffectProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EffectProperty::~EffectProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
EffectProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg != 0);
    
    // override in subclass
    Game::Property::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProperty::EmitAttachmentEvent(const Util::StringAtom& eventName) const
{
    // check if event exists
    if (!AnimEventRegistry::Instance()->HasAttachmentEvent(eventName))
    {
        return;
    }

	// get event
    const AnimEventRegistry::AttachmentEvent& event = AnimEventRegistry::Instance()->GetAttachmentEvent(eventName);

	// emit
	EffectsFeatureUnit::Instance()->EmitAttachmentEvent(this->GetEntity(), event.resource, event.jointName, event.duration, 0, event.keepLocal, event.rotation);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProperty::EmitCameraShakeEvent(const Util::StringAtom& eventName) const
{
    // check if event exists
    if (!AnimEventRegistry::Instance()->HasCamShakeEvent(eventName))
    {
        return;
    }

	// get event
    const AnimEventRegistry::ShakeEvent& event = AnimEventRegistry::Instance()->GetCamShakeEvent(eventName);

    // emit
    EffectsFeatureUnit::Instance()->EmitCameraShakeEffect(event.intensity,
                                                 event.rotation,
                                                 event.duration, 
                                                 event.range);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProperty::EmitSoundEvent(const Util::StringAtom& eventName) const
{
    // check if event exists
    if (!AnimEventRegistry::Instance()->HasSoundEvent(eventName))
    {
        return;
    }

	// get event
    const FAudio::EventId &eventId = AnimEventRegistry::Instance()->GetSoundEvent(eventName);

    // emit
	FAudio::AudioDevice::Instance()->EventPlayFireAndForget3D(eventId, this->GetEntity()->GetMatrix44(Attr::Transform), Math::vector::nullvec(),1.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
EffectProperty::EmitTimingEvent(const Util::StringAtom &eventName) const
{
    // check if event exists
    if (!AnimEventRegistry::Instance()->HasTimingEvent(eventName))
    {
        return;
    }

	// get event
    const AnimEventRegistry::TimingEvent& event = AnimEventRegistry::Instance()->GetTimingEvent(eventName);

    if(BaseGameFeature::TimeManager::Instance()->IsTimeEffectActive())
    {
        BaseGameFeature::TimeManager::Instance()->StopTimeEffect(true);
    }
    BaseGameFeature::TimeManager::Instance()->StartTimeEffect(event.factor, event.sustain, event.attack, event.release);    
}

//------------------------------------------------------------------------------
/**
*/
void
EffectProperty::EmitVibrationEvent(const Util::StringAtom& eventName, IndexT playerIndex) const
{
    // check if event exists
    if (!AnimEventRegistry::Instance()->HasVibrationEvent(eventName))
    {
        return;
    }

	// get event
    const AnimEventRegistry::VibrationEvent& event = AnimEventRegistry::Instance()->GetVibrationEvent(eventName);

    // emit
    EffectsFeatureUnit::Instance()->EmitVibrationEffect( event.highFreqIntensity,
                                                event.highFreqDuration,
                                                event.lowFreqIntensity,
                                                event.lowFreqDuration,
                                                playerIndex);
}


}; // namespace Game
