//------------------------------------------------------------------------------
//  characterinstance.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characterinstance.h"
#include "characters/character.h"
#include "coreanimation/animsamplebuffer.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/quaternion.h"
#include "models/modelinstance.h"
#include "graphics/modelentity.h"
#include "graphics/graphicsserver.h"
#include "characters/skinnedmeshrenderer.h"

// for debug visualization
#include "coregraphics/shaperenderer.h"
#include "threading/thread.h"

namespace Characters
{
__ImplementClass(Characters::CharacterInstance, 'CRIN', Core::RefCounted);

using namespace Util;
using namespace Math;
using namespace CoreAnimation;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Jobs;

//------------------------------------------------------------------------------
/**
*/
CharacterInstance::CharacterInstance():
	skeletonEvalMode(CharacterSkeletonInstance::BindPose),
    updateFrameIndex(InvalidIndex),
    jointTextureRowIndex(InvalidIndex),
    isValidForRendering(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterInstance::~CharacterInstance()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterInstance::Setup(const Ptr<Character>& origCharacter, const Ptr<Models::ModelInstance>& modelInst)
{
    n_assert(!this->IsValid());
    n_assert(origCharacter.isvalid());
    n_assert(origCharacter->IsValid());
    
    // setup a job port
    this->jobPort = JobPort::Create();
    this->jobPort->Setup();

    this->isValidForRendering = false;
    this->character = origCharacter;
    this->modelInstance = modelInst;
    this->skinSet.Setup(origCharacter->SkinLibrary(), modelInst);
    this->animController.Setup(this->jobPort, origCharacter->AnimationLibrary());
    this->skeletonInst.Setup(origCharacter->Skeleton());
    this->variationSet.Setup("", origCharacter->VariationLibrary());

    // for joint texture use, register our character  instance
    SkinnedMeshRenderer* skinRenderer = SkinnedMeshRenderer::Instance();
    if (SkinningTechnique::GPUTextureSkinning == skinRenderer->GetSkinningTechnique())
    {
        this->jointTextureRowIndex = skinRenderer->AllocJointTextureRow();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterInstance::Discard()
{
    n_assert(this->IsValid());    

    this->skeletonInst.Discard();
    this->skinSet.Discard();
    this->animController.Discard();
    this->modelInstance = 0;
    this->character = 0;

    // for joint texture use, register our character  instance
    SkinnedMeshRenderer* skinRenderer = SkinnedMeshRenderer::Instance();
    if (SkinningTechnique::GPUTextureSkinning == skinRenderer->GetSkinningTechnique())
    {
        skinRenderer->FreeJointTextureRow(this->jointTextureRowIndex);
    }

    this->jobPort->Discard();
    this->jobPort = 0;
}

//------------------------------------------------------------------------------
/**
    Update time for sequencer
*/
void
CharacterInstance::UpdateTime(const Timing::Tick time)
{
    this->animController.AnimSequencer().UpdateTime(time);
}

//------------------------------------------------------------------------------
/**
    Called by CharacterServer when during the culling phase for each
    visible character. This method may be called several times per
    frame, and the method detects whether this character has already
    been prepared for the current frame.
*/
bool
CharacterInstance::PrepareUpdate(IndexT frameIndex)
{
    if (frameIndex == this->updateFrameIndex)
    {
        // we've already been handled this frame
        return false;
    }

    // we've not been handled yet this frame
    this->updateFrameIndex = frameIndex;
    return true;
}

//------------------------------------------------------------------------------
/**
    Start asynchronous update of the character instance skeleton.
*/
void
CharacterInstance::StartUpdate()
{
    n_assert(this->IsValid());
    n_assert(InvalidIndex != this->updateFrameIndex);
    this->isValidForRendering = true;
    
    const float4* samplesPtr = 0;
    SizeT numSamples = 0;
    bool animUpdateValid = this->animController.Update();
    if (animUpdateValid)
    {
        // animation sample result is valid
        const Ptr<AnimSampleBuffer>& animSampleBuffer = this->animController.AnimSequencer().GetResult();
        numSamples = animSampleBuffer->GetNumSamples();
        samplesPtr = animSampleBuffer->GetSamplesPointer();
    }
    else
    {
        // animation sample result is invalid, use jesus pose
        const FixedArray<float4>& defaultSamples = this->character->Skeleton().GetDefaultSamplesArray();
        numSamples = defaultSamples.Size();
        samplesPtr = &(defaultSamples[0]);
    }

    // if GPUTexture skinning is used acquire a pointer into the texture and its row size
    void* jointTextureRowPtr = 0;
    SizeT jointTextureRowSize = 0;
    SkinnedMeshRenderer* skinRenderer = SkinnedMeshRenderer::Instance();
    if (SkinningTechnique::GPUTextureSkinning == skinRenderer->GetSkinningTechnique())
    {
        jointTextureRowPtr = skinRenderer->AcquireJointTextureRowPointer(this, jointTextureRowSize);
    }

    // evaluate the skeleton (updates skinning matrices)
    n_assert(0 != samplesPtr);
    this->skeletonInst.EvaluateAsync(this->jobPort, 
                                     samplesPtr, numSamples, 
                                     jointTextureRowPtr, jointTextureRowSize,
									 animUpdateValid, this->skeletonEvalMode);
}

//------------------------------------------------------------------------------
/**
    Wait until StartUpdate() is done for this character.
*/
void
CharacterInstance::WaitUpdateDone() const
{
    if (!this->jobPort->CheckDone())
    {
        // n_printf("WARNING: CharacterInstance waiting for async update!\n");
        this->jobPort->WaitDone();
    }
}

//------------------------------------------------------------------------------
/**
    Check if StartUpdate() has finished, return immediately.
*/
bool
CharacterInstance::CheckUpdateDone() const
{
    return this->jobPort->CheckDone();
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the character.
*/
void
CharacterInstance::RenderDebug(const matrix44& modelTransform)
{
    if (this->jobPort->CheckDone())
    {
        this->skeletonInst.RenderDebug(modelTransform);
    }       
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterInstance::SetVariationSetName(const Util::StringAtom& variationSetName)
{
    if (this->variationSet.GetActiveVariation() != variationSetName)
    {
        this->variationSet.SetActiveVariation(variationSetName);
        this->skeletonInst.ApplyJointComponents(this->variationSet.GetVariationCharJointComponents());
    }                                                               
}
} // namespace Characters
