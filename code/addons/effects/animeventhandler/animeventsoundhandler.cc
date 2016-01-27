//------------------------------------------------------------------------------
//  animeventsoundhandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effects/animeventhandler/animeventsoundhandler.h"
#include "effects/animeventregistry.h"
#include "graphics/graphicsserver.h"
#include "faudio/audiodevice.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::AnimEventSoundHandler, 'AESH', Animation::AnimEventHandlerBase);

using namespace Graphics;

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
		Ptr<Graphics::GraphicsEntity> entity = Graphics::GraphicsServer::Instance()->GetEntityById(event.GetEntityId());

		// get sound event id from registry
		const FAudio::EventId &eventId = AnimEventRegistry::Instance()->GetSoundEvent(event.GetAnimEvent().GetName());		
		FAudio::AudioDevice::Instance()->EventPlayFireAndForget3D(eventId, entity->GetTransform(), Math::vector::nullvec(), 1.0f);
    }
    
    return false;
}
} // namespace EffectsFeature
