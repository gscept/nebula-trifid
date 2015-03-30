//------------------------------------------------------------------------------
//  animeventshakehandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effects/animeventhandler/animeventshakehandler.h"
#include "effects/effects/camerashakeeffect.h"
#include "effects/animeventregistry.h"
#include "graphics/graphicsserver.h"
#include "../effectsfeatureunit.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::AnimEventShakeHandler, 'ASHA', Animation::AnimEventHandlerBase);

using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
AnimEventShakeHandler::AnimEventShakeHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AnimEventShakeHandler::~AnimEventShakeHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimEventShakeHandler::HandleEvent(const Animation::AnimEventInfo& event)
{
    //// first check if we got that attachment event    
    if (AnimEventRegistry::Instance()->HasCamShakeEvent(event.GetAnimEvent().GetName()))
    {
        // get data
        const AnimEventRegistry::ShakeEvent& shakeEvent = AnimEventRegistry::Instance()->GetCamShakeEvent(event.GetAnimEvent().GetName());

        // get position of shake
        Math::matrix44 transform;
        transform = Graphics::GraphicsServer::Instance()->GetEntityById(event.GetEntityId())->GetTransform();
        
        // create fx
        Ptr<EffectsFeature::CameraShakeEffect> shakeFx = EffectsFeature::CameraShakeEffect::Create();
        shakeFx->SetIntensity(shakeEvent.intensity);
        shakeFx->SetRotation(shakeEvent.rotation);
        shakeFx->SetRange(shakeEvent.range);
        shakeFx->SetDuration(shakeEvent.duration);
        shakeFx->SetTransform(transform);        

        // attach to server, fire and forget!
        EffectsFeature::EffectsFeatureUnit::Instance()->AttachEffect(shakeFx.cast<EffectsFeature::Effect>());            
    }    
    return false;
}
} // namespace EffectsFeature
