//------------------------------------------------------------------------------
//  fxfeature/properties/effectproperty.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/fxproperties/effectproperty.h"
#include "fx/fxfeature/fxmanager.h"
#include "game/entity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "audio2/audio2player.h"
#include "basegamefeature/basegametiming/timemanager.h"

namespace FxFeature
{
__ImplementClass(EffectProperty, 'FXPR', Game::Property);

using namespace FX;

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
    // get event    
    if (!AnimEventRegistry::Instance()->HasAttachmentEvent(eventName))
    {
        return;
    }
    //const AnimEventRegistry::AttachmentEvent& event = AnimEventRegistry::Instance()->GetAttachmentEvent(eventName);

    // TODO -> implement
    n_error("EffectProperty::EmitAttachmentEvent: NOT YET IMPLEMENTED\n");
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProperty::EmitCameraShakeEvent(const Util::StringAtom& eventName) const
{
    // get event
    if (!AnimEventRegistry::Instance()->HasCamShakeEvent(eventName))
    {
        return;
    }
    const AnimEventRegistry::ShakeEvent& event = AnimEventRegistry::Instance()->GetCamShakeEvent(eventName);

    // emit
    FxManager::Instance()->EmitCameraShakeEffect(event.intensity,
                                                 event.rotation,
                                                 event.duration, 
                                                 event.range, 
                                                 this->GetEntity()->GetMatrix44(Attr::Transform));
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProperty::EmitSoundEvent(const Util::StringAtom& eventName) const
{
    // get event
    if (!AnimEventRegistry::Instance()->HasSoundEvent(eventName))
    {
        return;
    }
    const Audio2::FmodEventId &eventId = AnimEventRegistry::Instance()->GetSoundEvent(eventName);
    // emit
    Audio2::Audio2Player::Instance()->PlayEvent(eventId, this->GetEntity()->GetMatrix44(Attr::Transform), Math::vector::nullvec());
}

//------------------------------------------------------------------------------
/**
*/
void
EffectProperty::EmitTimingEvent(const Util::StringAtom &eventName) const
{
    // get event
    if (!AnimEventRegistry::Instance()->HasTimingEvent(eventName))
    {
        return;
    }
    const AnimEventRegistry::TimingEvent& event = AnimEventRegistry::Instance()->GetTimingEvent(eventName);

    // 
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
    // get event
    if (!AnimEventRegistry::Instance()->HasVibrationEvent(eventName))
    {
        return;
    }
    const AnimEventRegistry::VibrationEvent& event = AnimEventRegistry::Instance()->GetVibrationEvent(eventName);

    // emit
    FxManager::Instance()->EmitVibrationEffect( event.highFreqIntensity,
                                                event.highFreqDuration,
                                                event.lowFreqIntensity,
                                                event.lowFreqDuration,
                                                playerIndex);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectProperty::OnBeginFrame()
{
	Property::OnBeginFrame();
}

}; // namespace Game
