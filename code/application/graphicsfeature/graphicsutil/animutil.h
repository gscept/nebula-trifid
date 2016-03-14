#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::AnimUtil
  
    Helper for quickly queing animations 
        
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/    
#include "graphics/graphicsprotocol.h"
#include "game/entity.h"
#include "graphics/graphicsentity.h"


//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class AnimUtil
{
public:
    /// enqueue animation on a game entity
    static void QueueAnimation(const Ptr<Game::Entity> & entity, 
                               const Util::String & clip,
                               int trackindex,
                               float loopcount,
                               const Timing::Tick& fadein,
                               const Timing::Tick& fadeout,
                               const Animation::AnimJobEnqueueMode::Code& mode,
                               float timeFactor = 1.0f,
                               float blendWeight = 1.0f,
                               const Timing::Tick& startTime = 0.0f);

    /// same as above but directly talks to a grpahics entity instead of a game entity
    static void QueueAnimation(const Ptr<Graphics::GraphicsEntity> & entity, 
                               const Util::String & clip,
                               int trackindex,
                               float loopcount,
                               const Timing::Tick& fadein,
                               const Timing::Tick& fadeout,
                               const Animation::AnimJobEnqueueMode::Code& mode,
                               float timeFactor = 1.0f,
                               float blendWeight = 1.0f,
                               const Timing::Tick& startTime = 0.0f);
};

}