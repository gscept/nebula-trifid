//------------------------------------------------------------------------------
//  treeinstance.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/treeinstance.h"
#include "forest/tree.h"
#include "forest/rt/forestserver.h"

namespace Forest
{
__ImplementClass(Forest::TreeInstance, 'TREI', Core::RefCounted);

using namespace Util;
using namespace Math;
using namespace Models;

const float TreeInstance::LODTransitionDuration = 1.0f;

//------------------------------------------------------------------------------
/**
*/
TreeInstance::TreeInstance() :
    transStartTime(0.0),
    curLodIndex(InvalidIndex),
    dstLodIndex(InvalidIndex),
    curAlpha(1.0f),
    dstAlpha(1.0f),
    lastVisibleFrameId(0xfffffffe),
    billboardScale(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TreeInstance::~TreeInstance()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
TreeInstance::Setup(const Ptr<Tree>& tree_, const StringAtom& id_, const matrix44& tform_)
{
    n_assert(!this->IsValid());

    this->tree = tree_;
    this->id = id_;
    this->transform = tform_;
    
    // create for each lod level one partial model instance
    const Array<Ptr<TreeLOD> > lods = this->tree->GetLODs();
    const matrix44& identity = matrix44::identity();
    for (IndexT index = 0; index < (lods.Size() - 1); index++)
    {   
        const StringAtom& nodePath = lods[index]->GetNodePath();
        const Ptr<Models::ModelInstance>& newInstance = this->GetTree()->GetModel()->CreatePartialInstance(nodePath, identity); 
        newInstance->SetTransform(this->transform);
        this->lodModelInstances.Append(newInstance);
    }

    this->invTransform = Math::matrix44::inverse(tform_);
    this->globalBox = this->tree->GetBoundingBox();
    this->globalBox.transform(this->transform);
    this->lastVisibleFrameId = 0xfffffffe;
    
    // extract billboard scaling and make sure that the tree is uniformly scaled
    float xScale = this->transform.get_xaxis().length();
    float yScale = this->transform.get_yaxis().length();
    float zScale = this->transform.get_zaxis().length();
    if (n_fequal(xScale, yScale, 0.05f) && n_fequal(xScale, zScale, 0.05f))
    {
        this->billboardScale = xScale;
    }
    else
    {
        n_error("Tree '%s' has non-uniform scale!");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeInstance::Discard() 
{
    n_assert(this->IsValid());

    this->tree = 0;
}

//------------------------------------------------------------------------------
/**o
*/
const Ptr<Tree>&
TreeInstance::GetTree() const
{
    return this->tree;
}

//------------------------------------------------------------------------------
/**
*/
void
TreeInstance::StartTransition(IndexT lodIndex, bool smoothTransition)
{
    n_assert(!this->IsTransitionActive());

    if (smoothTransition)
    {
        // do a smoothly blending transition
        this->transStartTime = 
        this->dstLodIndex = lodIndex;    
        this->dstAlpha = 0.0f;
    }
    else
    {
        // instant transition
        this->curAlpha = 1.0f;
        this->curLodIndex = lodIndex;
        this->dstAlpha = 0.0f;
        this->dstLodIndex = InvalidIndex;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeInstance::ContinueTransition()
{
    n_assert(this->IsTransitionActive());

    // time since start of transition
    Timing::Time curTime = ForestServer::Instance()->GetTime();
    float diffTime = float(curTime - this->transStartTime);
    if (diffTime > LODTransitionDuration)
    {
        // transition is finished
        this->curAlpha = 1.0f;
        this->curLodIndex = this->dstLodIndex;
        this->dstAlpha = 0.0f;
        this->dstLodIndex = InvalidIndex;
    }
    else
    {
        // fade in in the first 2/3 of the transition time
        float fadeTime   = LODTransitionDuration * 0.6667f;
        float fadeOffset = LODTransitionDuration * 0.3334f;

        // first fade in the new LOD level
        if (diffTime < fadeTime)
        {
            this->dstAlpha = n_saturate(diffTime / fadeTime);
        }
        else
        {
            this->dstAlpha = 1.0f;
        }

        // a bit later, fade out the current LOD level
        if (diffTime >= fadeOffset)
        {
            this->curAlpha = 1.0f - n_saturate((diffTime - fadeOffset) / fadeTime);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeInstance::OnRenderBefore(const Math::vector& viewerPos, IndexT frameIndex)
{
    const Array<Ptr<TreeLOD>>& lods = this->tree->GetLODs();

    // check if we've been continously visible, don't do smooth 
    // transition if we suddenly became visible...
    bool smoothTransition = false;
    if ((this->lastVisibleFrameId + 1) == frameIndex)
    {
        // yes we've been continously visible
        smoothTransition = true;
    }
    this->lastVisibleFrameId = frameIndex;
    
    // if an LOD transition is currently running, continue the transition
    if (this->IsTransitionActive())
    {
        this->ContinueTransition();
    }
   else
    {
        // no transition active at the moment, check if 
        // we need to start one
        vector viewerVec = viewerPos - this->transform.get_position();
        float distToViewer = viewerVec.length();

        // check if we need to start a LOD transition, this is the case
        // if the current LOD level is no longer in range
        bool startTransition = false;
        if (InvalidIndex == this->curLodIndex)
        {
            startTransition = true;
        }
        else if (!lods[this->curLodIndex]->IsInRange(distToViewer))
        {
            startTransition = true;
        }
        if (startTransition)
        {
            // start a new transition, find the "right" LOD level
            n_assert(InvalidIndex == this->dstLodIndex);
            IndexT lodIndex;
            for (lodIndex = 0; lodIndex < (lods.Size() - 1); lodIndex++)
            {
                if (lods[lodIndex]->IsInRange(distToViewer))
                {
                    break;
                }
            }
            this->StartTransition(lodIndex, smoothTransition);
        }
    }

    // get the current model-space point-of-interest (used to stamp a hole through
    // the trees for the camera)
    /*const vector& poi = ForestServer::Instance()->GetPointOfInterest();
    vector4 worldPoi4(poi.x, poi.y, poi.z, 1.0f);
    vector4 modelPoi4;
    this->invTransform.mult(worldPoi4, modelPoi4);
*/
    // render LODs
    if (this->curLodIndex != InvalidIndex)
    {
        if (lods[this->curLodIndex]->IsBillboard())
        {
            // render as billboard
            TreeBillboardRenderer::Instance()->AddTreeInstance(this);
            this->lodModelInstance = 0;
        }
        else
        {
            this->lodModelInstance = this->lodModelInstances[this->curLodIndex];
            this->lodModelInstance->OnRenderBefore(frameIndex, 0);
        }
    }

    if (this->dstLodIndex != InvalidIndex)
    {
        if (lods[this->dstLodIndex]->IsBillboard())
        {
            // render as billboard
            TreeBillboardRenderer::Instance()->AddTreeInstance(this);
            this->lodModelInstance = 0;
        }
        else
        {
            this->lodModelInstance = this->lodModelInstances[this->dstLodIndex];
            this->lodModelInstance->OnRenderBefore(frameIndex, 0);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeInstance::GetBillboardData(IndexT& outLodIndex, float& outFadeAlpha, float& outScale) const
{
    outLodIndex = InvalidIndex;
    outScale = this->billboardScale;
    outFadeAlpha = 0.0f;
    const Array<Ptr<TreeLOD> >& lods = this->tree->GetLODs();
    if ((InvalidIndex != this->curLodIndex) && lods[this->curLodIndex]->IsBillboard())
    {
        outLodIndex = this->curLodIndex;
        outFadeAlpha = this->curAlpha;
    }
    else if ((InvalidIndex != this->dstLodIndex) && lods[this->dstLodIndex]->IsBillboard())
    {
        outLodIndex = this->dstLodIndex;
        outFadeAlpha = this->dstAlpha;
    }
}

} // namespace Forest
