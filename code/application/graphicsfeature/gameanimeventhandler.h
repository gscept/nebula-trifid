#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::GameAnimEventHandler
    
    Handles anim events coupled with a game entity, sending a message to the entity whenever an anim event gets triggered.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "animation/animeventhandlerbase.h"
#include "game/entity.h"
#include "graphics/graphicsentity.h"
namespace GraphicsFeature
{
class GameAnimEventHandler : public Animation::AnimEventHandlerBase
{
	__DeclareClass(GameAnimEventHandler);
public:
	/// constructor
	GameAnimEventHandler();
	/// destructor
	virtual ~GameAnimEventHandler();

    /// handle a event
    virtual bool HandleEvent(const Animation::AnimEventInfo& event);

    /// registers a game entity with a model entity id to the handler
    void AttachEntity(const Graphics::GraphicsEntity::Id graphicsId, const Ptr<Game::Entity>& entity);
    /// detaches a game entity from the handler
    void DetachEntity(const Graphics::GraphicsEntity::Id graphicsId);

private:
    Util::Dictionary<Graphics::GraphicsEntity::Id, Ptr<Game::Entity>> entityLookupTable;
}; 
} // namespace GraphicsFeature
//------------------------------------------------------------------------------