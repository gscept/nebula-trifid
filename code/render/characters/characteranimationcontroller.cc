//------------------------------------------------------------------------------
//  characteranimationcontroller.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characteranimationcontroller.h"
#include "animation/playclipjob.h"

namespace Characters
{
using namespace Math;
using namespace Util;
using namespace Timing;
using namespace Animation;
using namespace CoreAnimation;

//------------------------------------------------------------------------------
/**
*/
CharacterAnimationController::CharacterAnimationController() :
    animDrivenMotionJointIndex(InvalidIndex),
    animDrivenMotionVector(vector::nullvec()),
    animDrivenMotionVectorDirty(false),
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterAnimationController::~CharacterAnimationController()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationController::Setup(const Ptr<Jobs::JobPort>& jobPort, const CharacterAnimationLibrary& animLib)
{
    n_assert(!this->IsValid());
    this->isValid = true;
    this->extJobPort = jobPort;
   
    // setup embedded animation controller
    this->animSequencer.Setup(animLib.GetAnimResource());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationController::SetupAnimDrivenMotion(IndexT jointIndex)
{
    n_assert(this->IsValid());
    this->animDrivenMotionJointIndex = jointIndex;
    const Ptr<AnimResource>& animRes = this->animSequencer.GetAnimResource();

    // first check if the anim resource actually contains velocity curves,
    // basically, the 4th curve must be a Velocity curve
    if (animRes->GetClipByIndex(0).CurveByIndex(3).GetCurveType() != CurveType::Velocity)
    {
        n_error("CharacterAnimationController::SetupAnimDrivenMotion(): anim resource '%s' has no velocity curves!\n",
            animRes->GetResourceId().Value());
    }

    // need to set the translation curve of this joint index to 0
    // NOTE: we're overwriting the same anim resource several 
    // times here, it would be better to handle this offline
    // during asset export!
    float4* keys = animRes->GetKeyBuffer()->GetKeyBufferPointer();
    IndexT translationCurveIndex = this->animDrivenMotionJointIndex * 4;
    float4 nullVec(0.0f, 0.0f, 0.0f, 0.0f);
    IndexT clipIndex;
    for (clipIndex = 0; clipIndex < animRes->GetNumClips(); clipIndex++)
    {
        const AnimClip& clip = animRes->GetClipByIndex(clipIndex);
        AnimCurve& curve = clip.CurveByIndex(translationCurveIndex);
        if (curve.IsStatic())
        {
            curve.SetStaticKey(nullVec);
        }
        else
        {
            IndexT firstKeyIndex = curve.GetFirstKeyIndex();
            SizeT keyStride = clip.GetKeyStride();
            SizeT numKeys = clip.GetNumKeys();
            IndexT i;
            for (i = 0; i < numKeys; i++)
            {
                keys[firstKeyIndex + i * keyStride] = nullVec;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationController::Discard()
{
    n_assert(this->IsValid());
    this->animSequencer.Discard();
    this->extJobPort = 0;
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
    Update the character's animation. This method must be called once
    per frame *after* starting or stopping animations! The method will
    return false if no animation is currently active.
*/
bool
CharacterAnimationController::Update()
{
    if (this->animSequencer.StartAsyncEvaluation(this->extJobPort))
    {
        this->animDrivenMotionVectorDirty = true;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationController::EnqueueAnimJob(const Ptr<AnimJob>& animJob)
{
    this->animSequencer.EnqueueAnimJob(animJob);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationController::StopTrack(IndexT trackIndex, bool allowFadeOut)
{
    this->animSequencer.StopTrack(trackIndex, allowFadeOut);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationController::StopAllTracks(bool allowFadeOut)
{
    this->animSequencer.StopAllTracks(allowFadeOut);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterAnimationController::PauseTrack( IndexT trackIndex )
{
	this->animSequencer.PauseTrack(trackIndex);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterAnimationController::PauseAllTracks()
{
	this->animSequencer.PauseAllTracks();
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterAnimationController::SetTime( Timing::Tick time )
{
	this->animSequencer.SetTime(time);
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationController::UpdateAnimDrivenMotionVector()
{
    n_assert(this->animDrivenMotionVectorDirty);

    if (InvalidIndex != this->animDrivenMotionJointIndex)
    {        
        this->animDrivenMotionVectorDirty = false;

        // layout of the samples is: translation/rotation/scale/velocity,
        // we need the velocity sample
        const Ptr<AnimSampleBuffer>& finalSampleBuffer = this->animSequencer.GetResult();
        IndexT velocitySampleIndex = this->animDrivenMotionJointIndex * 4 + 3;

        float4* samples = finalSampleBuffer->GetSamplesPointer();
        this->animDrivenMotionVector = samples[velocitySampleIndex];
        
        // FIXME: do this offline in anim converter tool?
        float4 invertXZ(-1.0f, 1.0f, -1.0f, 1.0f);
        this->animDrivenMotionVector = float4::multiply(this->animDrivenMotionVector, invertXZ);
    }
    else
    {
        // anim driven motion sampling disabled
        this->animDrivenMotionVector = vector::nullvec();
    }
}


} // namespace Characters
