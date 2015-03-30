//------------------------------------------------------------------------------
//  animeventshakehandler.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/fxanimeventhandler/animeventshakehandler.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/internalgraphicsentity.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "fx/rt/effects/camerashakeeffect.h"
#include "fx/rt/fxserver.h"
#include "fx/rt/animeventregistry.h"

namespace FX
{
__ImplementClass(FX::AnimEventShakeHandler, 'ASHA', Animation::AnimEventHandlerBase);

using namespace InternalGraphics;
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
        transform = InternalGraphics::InternalGraphicsServer::Instance()->GetEntityById(event.GetEntityId())->GetTransform();
        
        // create fx
        Ptr<FX::CameraShakeEffect> shakeFx = FX::CameraShakeEffect::Create();
        shakeFx->SetIntensity(shakeEvent.intensity);
        shakeFx->SetRotation(shakeEvent.rotation);
        shakeFx->SetRange(shakeEvent.range);
        shakeFx->SetDuration(shakeEvent.duration);
        shakeFx->SetTransform(transform);        

        // attach to server, fire and forget!
        FX::FxServer::Instance()->AttachEffect(shakeFx.cast<FX::Effect>());            
    }    
    return false;
}
} // namespace FX
