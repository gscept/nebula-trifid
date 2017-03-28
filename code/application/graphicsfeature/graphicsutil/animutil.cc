//------------------------------------------------------------------------------
//  animutil.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsutil/animutil.h"

namespace GraphicsFeature
{

//------------------------------------------------------------------------------
/**
*/
void
AnimUtil::QueueAnimation(const Ptr<Game::Entity> & entity, const Util::String & clip,
    int trackindex,
    float loopcount,
    const Timing::Tick& fadein,
    const Timing::Tick& fadeout,
    const Animation::AnimJobEnqueueMode::Code& mode,
    float timeFactor ,
    float blendWeight,
    const Timing::Tick& startTime)
{
    Ptr<Graphics::AnimPlayClip> animMsg = Graphics::AnimPlayClip::Create();
    animMsg->SetClipName(clip);
    animMsg->SetTrackIndex(trackindex);
    animMsg->SetLoopCount(loopcount);
    animMsg->SetFadeInTime(fadein);
    animMsg->SetFadeOutTime(fadeout);
    animMsg->SetTimeFactor(timeFactor);
    animMsg->SetEnqueueMode(mode);
    animMsg->SetBlendWeight(blendWeight);
    animMsg->SetStartTime(startTime);
    entity->SendSync(animMsg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
AnimUtil::QueueAnimation(const Ptr<Graphics::GraphicsEntity> & entity, const Util::String & clip,
    int trackindex,
    float loopcount,
    const Timing::Tick& fadein,
    const Timing::Tick& fadeout,
    const Animation::AnimJobEnqueueMode::Code& mode,
    float timeFactor ,
    float blendWeight,
    const Timing::Tick& startTime)
{
    Ptr<Graphics::AnimPlayClip> animMsg = Graphics::AnimPlayClip::Create();
    animMsg->SetClipName(clip);
    animMsg->SetTrackIndex(trackindex);
    animMsg->SetLoopCount(loopcount);
    animMsg->SetFadeInTime(fadein);
    animMsg->SetFadeOutTime(fadeout);
    animMsg->SetTimeFactor(timeFactor);
    animMsg->SetEnqueueMode(mode);
    animMsg->SetBlendWeight(blendWeight);
    animMsg->SetStartTime(startTime);
    entity->HandleMessage(animMsg.upcast<Messaging::Message>());
}

} // namespace GraphicsFeature