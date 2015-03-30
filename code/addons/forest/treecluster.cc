//------------------------------------------------------------------------------
//  treecluster.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/treecluster.h"

namespace Forest
{
__ImplementClass(Forest::TreeCluster, 'TRCL', Core::RefCounted);

using namespace Util;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TreeCluster::TreeCluster() :
    isValid(false),
    hasTreeInstances(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TreeCluster::~TreeCluster()
{
    if (this->IsValid())
    {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeCluster::Setup(const Math::bbox& boundingBox)
{
    n_assert(!this->IsValid());
    this->isValid = true;
    this->box = boundingBox;
}

//------------------------------------------------------------------------------
/**
*/
void
TreeCluster::Discard() 
{
    n_assert(this->IsValid());
    this->RemoveAllTreeInstances();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
TreeCluster::AddTreeInstance(const Ptr<TreeInstance>& treeInstance)
{
    n_assert(this->IsValid());
    
    // insert the tree instance by tree type
    const Ptr<Tree>& tree = treeInstance->GetTree();
    IndexT index = this->treeInstances.FindIndex(tree);
    if (index == InvalidIndex)
    {
        // add a new bucket
        Array<Ptr<TreeInstance> > dummyArray;
        this->treeInstances.Add(tree, dummyArray);
        index = this->treeInstances.Size() - 1;
    }

    // add tree instance to the right bucket
    this->treeInstances.ValueAtIndex(index).Append(treeInstance);
    this->hasTreeInstances = true;
}

//------------------------------------------------------------------------------
/**
    Remove tree instance. THIS IS A SLOW OPERATION!
*/
void
TreeCluster::RemoveTreeInstance(const Ptr<TreeInstance>& treeInstance)
{
    n_assert(this->IsValid());

    const Ptr<Tree>& tree = treeInstance->GetTree();
    n_assert(this->treeInstances.Contains(tree));
    IndexT index = this->treeInstances[tree].FindIndex(treeInstance);
    n_assert(InvalidIndex != index);
    this->treeInstances[tree].EraseIndex(index);

    if(this->CountTreeInstances() == 0)
    {
        this->hasTreeInstances = false;
    }
}

//------------------------------------------------------------------------------
/**
    Check if tree instance is contained. THIS IS A SLOW OPERATION!
*/
bool
TreeCluster::HasTreeInstance(const Ptr<TreeInstance>& treeInstance) const
{
    n_assert(this->IsValid());
    
    const Ptr<Tree>& tree = treeInstance->GetTree();
    IndexT index = this->treeInstances[tree].FindIndex(treeInstance);
    return (InvalidIndex != index);
}
    
//------------------------------------------------------------------------------
/**
    Remove all tree instances from the cluster.
*/
void
TreeCluster::RemoveAllTreeInstances()
{
    n_assert(this->IsValid());
    this->treeInstances.Clear();
    this->hasTreeInstances = false;
}

//------------------------------------------------------------------------------
/**
    Count the number of tree instances in the cluster.
*/
SizeT
TreeCluster::CountTreeInstances() const
{
    n_assert(this->IsValid());
    IndexT numTreeInstances = 0;
    IndexT i;
    for (i = 0; i < this->treeInstances.Size(); i++)
    {
        numTreeInstances += this->treeInstances.ValueAtIndex(i).Size();
    }
    return numTreeInstances;
}

//------------------------------------------------------------------------------
/**
    Render the tree instances in the tree cluster.
*/
void
TreeCluster::OnRenderBefore(const Ptr<CameraEntity>& cameraEntity, IndexT frameIndex)
{
    n_assert(cameraEntity.isvalid());

    // first check whether the tree cluster is unclipped, if yes we can
    // skip the bounding box visibility check per tree instance
    Math::ClipStatus::Type clusterClipStatus = cameraEntity->ComputeClipStatus(this->box);

    IndexT treeIndex;
    for (treeIndex = 0; treeIndex < this->treeInstances.Size(); treeIndex++)
    {
        const Array<Ptr<TreeInstance>>& treeInstArray = this->treeInstances.ValueAtIndex(treeIndex);
        IndexT treeInstIndex;
        for (treeInstIndex = 0; treeInstIndex < treeInstArray.Size(); treeInstIndex++)
        {
            const Ptr<TreeInstance>& treeInst = treeInstArray[treeInstIndex];
            bool isVisible = true;
            if (Math::ClipStatus::Inside != clusterClipStatus)
            {
                // perform visible-check for current tree instance
                Math::ClipStatus::Type instClipStatus = cameraEntity->ComputeClipStatus(treeInst->GetGlobalBoundingBox());
                isVisible = (Math::ClipStatus::Outside != instClipStatus);
            }
            if (isVisible)
            {
                treeInst->OnRenderBefore(cameraEntity->GetTransform().get_position(), frameIndex);
            }
        }
    }
}


} // namespace Forest
