//------------------------------------------------------------------------------
//  cameraanimationeffect.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "cameraanimationeffect.h"
#include "coreanimation/animresource.h"
#include "coreanimation/streamanimationloader.h"
#include "coreanimation/animutil.h"
#include "math/vector.h"
#include "math/quaternion.h"
#include "resources/resourcemanager.h"
#include "graphicsfeature/graphicsfeatureunit.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::CameraAnimationEffect, 'CAFX', EffectsFeature::Effect);

using namespace CoreAnimation;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
CameraAnimationEffect::CameraAnimationEffect() :
    isRelative(false),
    isPlaying(false),
    animTime(0.0),
    animStartTime(0.0),
    animEndTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CameraAnimationEffect::~CameraAnimationEffect()
{
	if(this->result.isvalid())
	{
		this->OnDeactivate();
	}
    n_assert(!this->animResource.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
bool
CameraAnimationEffect::SetupAnimation()
{
    n_assert(this->animPath.IsValid());

    this->animResource = Resources::ResourceManager::Instance()->CreateUnmanagedResource(animPath, AnimResource::RTTI, StreamAnimationLoader::Create()).cast<AnimResource>();
    this->animResource->Load();
    this->result->Setup(this->animResource);

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
CameraAnimationEffect::DiscardAnimation()
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
CameraAnimationEffect::OnActivate(Timing::Time time)
{
    Effect::OnActivate(time);

    // save initial position
	this->initialTransform = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView()->GetCameraEntity()->GetTransform();

    // and setup rest
    this->result = AnimSampleBuffer::Create();
    this->SetupAnimation();
}

//------------------------------------------------------------------------------
/**
*/
void
CameraAnimationEffect::OnDeactivate()
{
    this->DiscardAnimation();
    if (this->result.isvalid())
    {
        this->result = 0;
    }
    Effect::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
CameraAnimationEffect::OnStart(Timing::Time time)
{
    this->Play(time);
}

//------------------------------------------------------------------------------
/**
    Start animation playback at the current time position.
*/
void
CameraAnimationEffect::Play(Timing::Time time)
{
    n_assert(!this->isPlaying);
    n_assert(0 == this->startTime)
    this->isPlaying = true;
    this->startTime = time;
    this->animTime = 0;
}

//------------------------------------------------------------------------------
/**
    Stop animation playback.
*/
void
CameraAnimationEffect::Stop(Timing::Time time)
{
    n_assert(this->isPlaying);
    this->isPlaying = false;    
}

//------------------------------------------------------------------------------
/**
*/
void
CameraAnimationEffect::OnFrame(Timing::Time time)
{
    this->animTime = time - this->startTime;

    // handle past duration case
    if (this->animTime > this->animEndTime)
    {
        this->Stop(time);
        return;
    }

    // find clip index by time
    Timing::Tick ticks = Timing::SecondsToTicks(this->animTime);
    IndexT clipIndex;
    for (clipIndex = 0; clipIndex < this->animResource->GetNumClips(); clipIndex++)
    {
        const AnimClip& clip = this->animResource->GetClipByIndex(clipIndex);
        if ((ticks >= clip.GetStartTime()) && (ticks < (clip.GetStartTime() + clip.GetClipDuration())))
        {
            break;
        }
    }

    // get the end time of the last anim group, this is our overall runtime
    //SizeT numAnimGroups = this->refAnimation->GetNumGroups();
    Timing::Tick relTicks = ticks - this->animResource->GetClipByIndex(clipIndex).GetStartTime();
    AnimUtil::Sample(this->animResource, clipIndex, SampleType::Linear, relTicks, 1.0f, result);

    // build matrix from translation and rotation
    n_assert(result->GetNumSamples() == 3);
    float4* keys = result->MapSamples();
    vector translate(keys[0].x(), keys[0].y(), keys[0].z());
    quaternion rotate(keys[1].x(), keys[1].y(), keys[1].z(), keys[1].w());
    matrix44 m = matrix44::rotationquaternion(rotate);    
    m.translate(translate);
    if (this->isRelative)
    {
        // TODO: test!!!
        m = Math::matrix44::multiply(m, this->initialTransform);
    }

    result->UnmapSamples();

    // update cam transform
    GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView()->GetCameraEntity()->SetTransform(m);
}
}; // namespace FX