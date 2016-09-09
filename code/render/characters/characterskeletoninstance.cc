//------------------------------------------------------------------------------
//  characterskeletoninstance.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characterskeletoninstance.h"
#include "characters/skinnedmeshrenderer.h"

// for debug visualization
#include "coregraphics/shaperenderer.h"
#include "coregraphics/textelement.h"
#include "coregraphics/textrenderer.h"
#include "threading/thread.h"
#include "debugrender/debugrender.h"
#include "graphics/cameraentity.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"

namespace Characters
{
using namespace Math;
using namespace Util;
using namespace CoreGraphics;
using namespace Graphics;
using namespace Jobs;

// job function declaration
#if __PS3__
extern "C" {
    extern const char _binary_jqjob_render_charevalskeletonjob_ps3_bin_start[];
    extern const char _binary_jqjob_render_charevalskeletonjob_ps3_bin_size[];
}
#else
extern void CharEvalSkeletonJobFunc(const JobFuncContext& ctx);
#endif

//------------------------------------------------------------------------------
/**
*/
CharacterSkeletonInstance::CharacterSkeletonInstance() :
    skeletonPtr(0),
    isValid(false),
    jointComponentsDirty(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterSkeletonInstance::~CharacterSkeletonInstance()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**    
    NOTE: sampleBuffer is a pointer to animation samples which are the
    result of the character's animation.
*/
void
CharacterSkeletonInstance::Setup(const CharacterSkeleton& skeleton)
{
    n_assert(!this->IsValid());
    n_assert(skeleton.IsValid());

    this->isValid = true;
    this->skeletonPtr = &skeleton;
    
    SizeT numJoints = skeleton.GetNumJoints();
    this->startJointComponentsArray.SetSize(numJoints);
    this->scaledMatrixArray.SetSize(numJoints);
    this->skinMatrixArray.SetSize(numJoints);
    this->jointComponentsArrayPtr = &this->startJointComponentsArray;

    // setup instance joints
    IndexT i;
    for (i = 0; i < numJoints; i++)
    {
        this->SetupJoint(skeleton, i);
    }

    // setup a job object to asynchronously evaluate the skeleton
    this->SetupEvalJob();
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterSkeletonInstance::Discard()
{
    n_assert(this->IsValid());
    this->skeletonPtr = 0;
    this->startJointComponentsArray.Clear();
    this->scaledMatrixArray.Clear();
    this->skinMatrixArray.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
    This setups the job object to asynchronously evaluate the skeleton.
*/
void
CharacterSkeletonInstance::SetupEvalJob()
{
    n_assert(0 != this->skeletonPtr);
    n_assert(this->jointComponentsArrayPtr->Size() > 0);
    n_assert(this->scaledMatrixArray.Size() > 0);
    n_assert(this->skinMatrixArray.Size() > 0);

    #if __PS3__
    JobFuncDesc jobFunc(_binary_jqjob_render_charevalskeletonjob_ps3_bin_start, _binary_jqjob_render_charevalskeletonjob_ps3_bin_size);
    #else
    JobFuncDesc jobFunc(CharEvalSkeletonJobFunc);
    #endif

    // NOTE: all input and output element sizes are 64 bytes, and
    // jobs only use 1 data slice (this limits the number of joints 
    // to 255 (256 * 64 -> 16 kByte);
    const SizeT elmSize = 64;
    const SizeT numElements = this->jointComponentsArrayPtr->Size();
    SizeT compBufferSize = numElements * sizeof(CharJointComponents);
    SizeT outBufSize = numElements * elmSize;

    // setup the job, patch the sample buffer data later
    this->evalJob = Job::Create();
    const FixedArray<matrix44>& invPoseMatrixArray = this->skeletonPtr->GetInvPoseMatrixArray();
    JobUniformDesc uniform(invPoseMatrixArray.Begin(), invPoseMatrixArray.Size() * sizeof(matrix44), outBufSize);
    JobDataDesc input(this->jointComponentsArrayPtr->Begin(), compBufferSize, compBufferSize,
                      0, 16, 16);

    if (SkinnedMeshRenderer::Instance()->GetSkinningTechnique() == SkinningTechnique::GPUTextureSkinning)
    {
        // when GPUTexture skinning, need to provide dummy output for the
        // texture row in slot 4
        JobDataDesc output(this->scaledMatrixArray.Begin(), outBufSize, outBufSize,
                           this->skinMatrixArray.Begin(), outBufSize, outBufSize,
                           0, 16, 16);
        this->evalJob->Setup(uniform, input, output, jobFunc);
    }
    else
    {
        JobDataDesc output(this->scaledMatrixArray.Begin(), outBufSize, outBufSize,
                           this->skinMatrixArray.Begin(), outBufSize, outBufSize);
        this->evalJob->Setup(uniform, input, output, jobFunc);
    }
}

//------------------------------------------------------------------------------
/**
    Evaluate the joints in the skeleton. Must be called after joints
    have been updated with animation data (usually called by 
    the CharacterNodeInstance which owns this skeleton instance).

    NOTE that this is an asynchronous message, evaluation isn't finished
    when this method returns!
*/
void
CharacterSkeletonInstance::EvaluateAsync(const Ptr<Jobs::JobPort>& jobPort,
                                         const Math::float4* sampleBuffer, SizeT numSamples, 
                                         void* jointTextureRowPtr, SizeT jointTextureRowSize,
                                         bool waitAnimJobsDone)
{
    // patch input and optional joint texture row pointers into the eval job
    // NOTE: The sampleBuffer pointer may be 0, if this is the case, there is
    // no animation sample data avaliable, and the skeleton should simply set
    // itself to the jesus pose
    // the jointTexture data will only be valid if a GPU texture skinning is used
   
    // patch the input buffer into the job
    SizeT sampleBufferSize = numSamples * sizeof(float4);
    JobDataDesc input = this->evalJob->GetInputDesc();
    input.Update(1, (void*)sampleBuffer, sampleBufferSize, sampleBufferSize);
    if (this->jointComponentsDirty)
    {
        input.Update(0, this->jointComponentsArrayPtr->Begin(), input.GetBufferSize(0), input.GetSliceSize(0));
        this->jointComponentsDirty = false;                
    }
    this->evalJob->PatchInputDesc(input);

    // patch the joint texture pointer into the output desc
    if (SkinnedMeshRenderer::Instance()->GetSkinningTechnique() == SkinningTechnique::GPUTextureSkinning)
    {
        n_assert(0 != jointTextureRowPtr);
        JobDataDesc output = this->evalJob->GetOutputDesc();
        output.Update(2, jointTextureRowPtr, jointTextureRowSize, jointTextureRowSize);
        this->evalJob->PatchOutputDesc(output);
    }

    if (waitAnimJobsDone)
    {
        // need to wait until animation system jobs are done
        jobPort->PushSync();
    }
    jobPort->PushJob(this->evalJob);
}

//------------------------------------------------------------------------------
/**
    Setup a single joint in the skeleton instance.
*/
void
CharacterSkeletonInstance::SetupJoint(const CharacterSkeleton& skeleton, IndexT jointIndex)
{
    const CharacterJoint& joint = skeleton.GetJoint(jointIndex);
    
    // setup joint components
    CharJointComponents& comps = this->startJointComponentsArray[jointIndex];
    comps.parentJointIndex = joint.GetParentJointIndex();
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the character.
    Note: The debug visualization isn't up-to-date, one frame after skinning
    cause RenderDebug is called after view->Render where all debug shapes are rendered (see framebatch)
*/
void
CharacterSkeletonInstance::RenderDebug(const matrix44& modelTransform)
{
	Array<RenderShape::RenderShapeVertex> scaledSkeleton;
    IndexT i;
    SizeT numJoints = this->GetNumJoints();
    for (i = 0; i < numJoints; i++)
    {
        IndexT parentJointIndex = (*this->jointComponentsArrayPtr)[i].parentJointIndex;
        if (InvalidIndex != parentJointIndex)
        {   
            RenderShape::RenderShapeVertex vert;
            vert.pos = this->scaledMatrixArray[parentJointIndex].get_position();
			scaledSkeleton.Append(vert);
            vert.pos = this->scaledMatrixArray[i].get_position();
			scaledSkeleton.Append(vert);

			// copy scaled matrix
			float4 position = this->scaledMatrixArray[i].get_position();
			matrix44 unscaledMatrix = matrix44::scaling(0.1f);
			unscaledMatrix.set_position(position);
            unscaledMatrix = matrix44::multiply(unscaledMatrix, modelTransform);

			// setup joint shape
			RenderShape jointShape;
			jointShape.SetupSimpleShape(Threading::Thread::GetMyThreadId(),
				RenderShape::Sphere,
				RenderShape::AlwaysOnTop,
				unscaledMatrix,
				float4(0.65f, 0.0f, 0.0f, 0.8f));
			ShapeRenderer::Instance()->AddShape(jointShape);
        }
    }

    if (!scaledSkeleton.IsEmpty())
    {
		RenderShape shape;
		shape.SetupPrimitives(Threading::Thread::GetMyThreadId(),
			modelTransform,
			PrimitiveTopology::LineList,
			scaledSkeleton.Size() / 2,
			&(scaledSkeleton.Front()),
			float4(1.0f, 0.0f, 0.0f, 0.6f),
            CoreGraphics::RenderShape::CheckDepth);

		ShapeRenderer::Instance()->AddShape(shape);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterSkeletonInstance::ApplyJointComponents(const Util::FixedArray<CharJointComponents>& set)
{
    n_assert(this->jointComponentsArrayPtr->Size() == set.Size());
    this->jointComponentsArrayPtr = const_cast<Util::FixedArray<CharJointComponents>*>(&set);
    this->jointComponentsDirty = true;
}
} // namespace Characters