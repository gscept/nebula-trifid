//------------------------------------------------------------------------------
//  animeventvibrationhandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effects/animeventhandler/animeventvibrationhandler.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "effects/effects/vibrationeffect.h"
#include "effects/animeventregistry.h"
#include "../effectsfeatureunit.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::AnimEventVibrationHandler, 'AEVH', Animation::AnimEventHandlerBase);


using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
AnimEventVibrationHandler::AnimEventVibrationHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AnimEventVibrationHandler::~AnimEventVibrationHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimEventVibrationHandler::HandleEvent(const Animation::AnimEventInfo& event)
{
    // first check if we got that attachment event
    if (AnimEventRegistry::Instance()->HasVibrationEvent(event.GetAnimEvent().GetName()))
    {
        // get data
        const AnimEventRegistry::VibrationEvent& vibEvent = AnimEventRegistry::Instance()->GetVibrationEvent(event.GetAnimEvent().GetName());

        // create fx
        Ptr<EffectsFeature::VibrationEffect> vibFx = EffectsFeature::VibrationEffect::Create();
        vibFx->SetHighFreqDuration(vibEvent.highFreqDuration);
        vibFx->SetHighFreqIntensity(vibEvent.highFreqIntensity);
        vibFx->SetLowFreqDuration(vibEvent.lowFreqDuration);
        vibFx->SetLowFreqIntensity(vibEvent.lowFreqIntensity);
        vibFx->SetPlayerIndex(vibEvent.playerIndex);

        // attach to server, fire and forget!
        EffectsFeature::EffectsFeatureUnit::Instance()->AttachEffect(vibFx.cast<EffectsFeature::Effect>());            
    }    
    return false;
}
} // namespace FX
