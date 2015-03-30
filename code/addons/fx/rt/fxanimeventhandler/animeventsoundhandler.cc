//------------------------------------------------------------------------------
//  animeventsoundhandler.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/fxanimeventhandler/animeventsoundhandler.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/internalgraphicsentity.h"
#include "audio2/audio2interface.h"
#include "audio2protocol.h"
#include "fx/rt/animeventregistry.h"

namespace FX
{
__ImplementClass(FX::AnimEventSoundHandler, 'AESH', Animation::AnimEventHandlerBase);

using namespace InternalGraphics;

//------------------------------------------------------------------------------
/**
*/
AnimEventSoundHandler::AnimEventSoundHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AnimEventSoundHandler::~AnimEventSoundHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimEventSoundHandler::HandleEvent(const Animation::AnimEventInfo& event)
{
    // first check if we got that attachment event    
    if (AnimEventRegistry::Instance()->HasSoundEvent(event.GetAnimEvent().GetName()))
    {        
        // get entity
        Ptr<InternalGraphicsEntity> entity = InternalGraphicsServer::Instance()->GetEntityById(event.GetEntityId());

		const Audio2::FmodEventId &eventId = AnimEventRegistry::Instance()->GetSoundEvent(event.GetAnimEvent().GetName());
        // play 
        Ptr<Audio2::EventPlayFireAndForget3D> msg = Audio2::EventPlayFireAndForget3D::Create();
        msg->SetEventId(eventId);
        msg->SetTransform(entity->GetTransform());
        msg->SetVelocity(Math::vector::nullvec());      // TODO <- velocity
        Audio2::Audio2Interface::Instance()->Send(msg);
    }
    
    return false;
}
} // namespace FX
