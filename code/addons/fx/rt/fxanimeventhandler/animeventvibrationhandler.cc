//------------------------------------------------------------------------------
//  animeventvibrationhandler.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/fxanimeventhandler/animeventvibrationhandler.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "fx/rt/effects/vibrationeffect.h"
#include "fx/rt/fxserver.h"
#include "fx/rt/animeventregistry.h"

namespace FX
{
__ImplementClass(FX::AnimEventVibrationHandler, 'AEVH', Animation::AnimEventHandlerBase);

using namespace InternalGraphics;
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
        Ptr<FX::VibrationEffect> vibFx = FX::VibrationEffect::Create();
        vibFx->SetHighFreqDuration(vibEvent.highFreqDuration);
        vibFx->SetHighFreqIntensity(vibEvent.highFreqIntensity);
        vibFx->SetLowFreqDuration(vibEvent.lowFreqDuration);
        vibFx->SetLowFreqIntensity(vibEvent.lowFreqIntensity);
        vibFx->SetPlayerIndex(vibEvent.playerIndex);

        // attach to server, fire and forget!
        FX::FxServer::Instance()->AttachEffect(vibFx.cast<FX::Effect>());            
    }    
    return false;
}
} // namespace FX
