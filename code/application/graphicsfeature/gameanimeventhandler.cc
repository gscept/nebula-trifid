//------------------------------------------------------------------------------
//  gameanimeventhandler.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "gameanimeventhandler.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::GameAnimEventHandler, 'GAEH', Animation::AnimEventHandlerBase);

//------------------------------------------------------------------------------
/**
*/
GameAnimEventHandler::GameAnimEventHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GameAnimEventHandler::~GameAnimEventHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
GameAnimEventHandler::HandleEvent( const Animation::AnimEventInfo& event )
{
    const Graphics::GraphicsEntity::Id& id = event.GetEntityId();
    if (this->entityLookupTable.Contains(id))
    {
        const Ptr<Game::Entity>& entity = this->entityLookupTable[id];
        Ptr<AnimEventTriggered> msg = AnimEventTriggered::Create();

        // get event and feed message
        const CoreAnimation::AnimEvent& ev = event.GetAnimEvent();
        msg->SetCategory(ev.GetCategory().AsString());
        msg->SetName(ev.GetName().AsString());
        msg->SetTime(ev.GetTime());

        // send to entity
        entity->SendSync(msg.upcast<Messaging::Message>());

        return true;
    }    
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
GameAnimEventHandler::AttachEntity( const Graphics::GraphicsEntity::Id graphicsId, const Ptr<Game::Entity>& entity )
{
    n_assert(!this->entityLookupTable.Contains(graphicsId));
    this->entityLookupTable.Add(graphicsId, entity);
}

//------------------------------------------------------------------------------
/**
*/
void 
GameAnimEventHandler::DetachEntity( const Graphics::GraphicsEntity::Id graphicsId )
{
    n_assert(this->entityLookupTable.Contains(graphicsId));
    this->entityLookupTable.Erase(graphicsId);
}
} // namespace GraphicsFeature