//------------------------------------------------------------------------------
//  properties/pathanimproperty.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/pathanimproperty.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphics/graphicsentity.h"
#include "game/entity.h"
#include "math/quaternion.h"
#include "basegametiming/gametimesource.h"
#include "coreanimation/animutil.h"
#include "coreanimation/sampletype.h"
#include "coreanimation/streamanimationloader.h"
#include "coreanimation/infinitytype.h"
#include "resources/resourcemanager.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::PathAnimProperty, 'PAPR', Game::Property);

using namespace Game;
using namespace Math;
using namespace Util;
using namespace CoreAnimation;

//------------------------------------------------------------------------------
/**
*/
PathAnimProperty::PathAnimProperty() :
    animTime(0.0),
    animStartTime(0.0),
    animEndTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PathAnimProperty::~PathAnimProperty()
{
    n_assert(!this->animResource.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, MoveBefore);
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(AnimPlay::Id);
    this->RegisterMessage(AnimStop::Id);
    this->RegisterMessage(AnimRewind::Id);
    this->RegisterMessage(AnimUpdate::Id);
    Game::Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::OnActivate()
{
    Property::OnActivate();

    // save initial position
    this->initialTransform = this->entity->GetMatrix44(Attr::Transform);

    // check if we actually should do some animation...
    String animPath = this->entity->GetString(Attr::AnimPath);
    if (animPath.IsValid())
    {
        this->animSampleBuffer = AnimSampleBuffer::Create();
        this->SetupAnimation(animPath);
        //this->Rewind();
        //this->Play();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::OnStart()
{
    Property::OnStart();
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::OnDeactivate()
{
    this->DiscardAnimation();
    if (this->animSampleBuffer.isvalid())
    {
        this->animSampleBuffer = 0;
    }
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
bool
PathAnimProperty::SetupAnimation(const Util::String& path)
{
    // FIXME!
    String animPath = path;
    if (animPath.GetFileExtension() == "nax2")
    {
        animPath.StripFileExtension();
        animPath.Append(".nax3");
    }
    this->animResource = Resources::ResourceManager::Instance()->CreateUnmanagedResource(animPath, AnimResource::RTTI, StreamAnimationLoader::Create()).cast<AnimResource>();
    if (!this->animResource->Load())
    {
        n_error("PathAnimProperty: Failed to load animation '%s'!\n", animPath.AsCharPtr());
    }
    this->animSampleBuffer->Setup(this->animResource);
    
    // setup the animation's end time
    this->animEndTime = 0.0;
    IndexT clipIndex;
    for (clipIndex = 0; clipIndex < this->animResource->GetNumClips(); clipIndex++)
    {
        const AnimClip& clip = this->animResource->GetClipByIndex(clipIndex);
        Timing::Time curEndTime = Timing::TicksToSeconds(clip.GetStartTime() + clip.GetClipDuration());
        if (curEndTime > this->animEndTime)
        {
            this->animEndTime = curEndTime;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::DiscardAnimation()
{
    if (this->animResource.isvalid())
    {
        Resources::ResourceManager::Instance()->UnregisterUnmanagedResource(this->animResource.cast<Resources::Resource>());    
        this->animResource = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PathAnimProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{    
    if (this->animResource.isvalid())
    {
        if (msg->CheckId(AnimPlay::Id))
        {
            this->Play();
        }
        else if (msg->CheckId(AnimStop::Id))
        {
           this->Stop();
        }
        else if (msg->CheckId(AnimRewind::Id))
        {
            this->Rewind();
        }
    }
    if (msg->CheckId(AnimUpdate::Id))
    {
        if (this->animResource.isvalid())
        {
            this->DiscardAnimation();
        }
        this->SetupAnimation((msg.cast<AnimUpdate>())->GetAnimPath());
    }
}

//------------------------------------------------------------------------------
/**
    This method actually moves the entity around.
*/
void
PathAnimProperty::OnMoveBefore()
{
    if (this->animResource.isvalid() && this->entity->GetBool(Attr::AnimPlaying))
    {
        // handle animation playback
        this->UpdateAnimation();
    }
}

//------------------------------------------------------------------------------
/**
    Start animation playback at the current time position.
*/
void
PathAnimProperty::Play()
{
    this->entity->SetBool(Attr::AnimPlaying, true);
    this->animStartTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();
}

//------------------------------------------------------------------------------
/**
    Stop animation playback.
*/
void
PathAnimProperty::Stop()
{
    this->entity->SetBool(Attr::AnimPlaying, false);    
}

//------------------------------------------------------------------------------
/**
    Rewind animation cursor to beginning.
*/
void
PathAnimProperty::Rewind()
{
    this->UpdateAnimation();
}

//------------------------------------------------------------------------------
/**
    Handle animation playback. 
*/
void
PathAnimProperty::UpdateAnimation()
{
    this->animTime = BaseGameFeature::GameTimeSource::Instance()->GetTime() - this->animStartTime;

    //n_printf("PathAnimProperty: Time: %f \n", this->animTime);

    // handle past duration case
    if (this->animTime > this->animEndTime)
    {
        if (this->entity->GetBool(Attr::AnimLoop))
        {
            // looping: rewind to beginning
            this->animTime = fmod(this->animTime, this->animEndTime);
        }
        else
        {
            // not looping: stop
            this->Stop();
            return;
        }
    }

    // find clip index by time
    Timing::Tick ticks = Timing::SecondsToTicks(this->animTime);
    IndexT clipIndex;
    for (clipIndex = 0; clipIndex < this->animResource->GetNumClips(); ++clipIndex)
    {
        const AnimClip& clip = this->animResource->GetClipByIndex(clipIndex);
        if ((ticks >= clip.GetStartTime()) && (ticks < (clip.GetStartTime() + clip.GetClipDuration())))
        {
            break;
        }
    }

    if (clipIndex < this->animResource->GetNumClips())
    {
        // get the end time of the last anim group, this is our overall runtime
        //SizeT numAnimGroups = this->refAnimation->GetNumGroups();
        Timing::Tick relTicks = ticks - this->animResource->GetClipByIndex(clipIndex).GetStartTime();
        AnimUtil::Sample(this->animResource, clipIndex, SampleType::Linear, relTicks, 1.0f, this->animSampleBuffer);

        // build matrix from translation and rotation
        n_assert(this->animSampleBuffer->GetNumSamples() == 3);
        Math::float4* keys = this->animSampleBuffer->MapSamples();
        vector translate(keys[0].x(), keys[0].y(), keys[0].z());
        Math::quaternion rotate(keys[1].x(), keys[1].y(), keys[1].z(), keys[1].w());
        Math::matrix44 m = matrix44::rotationquaternion(rotate);    
        m.translate(translate);
        if (this->entity->GetBool(Attr::AnimRelative))
        {
            // TODO: test!!!
            m = Math::matrix44::multiply(m, this->initialTransform);
        }
        m.translate(this->GetEntity()->GetFloat4(Attr::AnimPathOffset));

        this->animSampleBuffer->UnmapSamples();

        // update entity's transformation
        Ptr<BaseGameFeature::UpdateTransform> msg = BaseGameFeature::UpdateTransform::Create();
        msg->SetMatrix(m);
        this->entity->SendSync(msg.cast<Messaging::Message>());
    }
}

}; // namespace Properties