//------------------------------------------------------------------------------
//  treeclusterpool.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/treeclusterpool.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "graphics/stage.h"

namespace Forest
{
__ImplementClass(Forest::TreeClusterPool, 'TRCP', Core::RefCounted);

using namespace Util;
using namespace Math;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TreeClusterPool::TreeClusterPool() :
    isValid(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
TreeClusterPool::~TreeClusterPool()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
TreeClusterPool::Setup(const bbox& levelBoundingBox)
{
    n_assert(!this->IsValid());
    this->isValid = true;

    // first setup a quad tree from the bounding box, choose a reasonable 
    // depth
    uchar depth;
    float levelSize = 2.0f * n_max(levelBoundingBox.extents().x(), levelBoundingBox.extents().z());
    if (levelSize < 250.0f)
    {
        depth = 1;
    }
    else if (levelSize < 750.0f)
    {
        depth = 2;
    }
    else
    {
        depth = 3;
    }
    this->quadTree.Setup(levelBoundingBox, depth);

    // traverse the quad tree and create a tree cluster for every node
    SizeT numNodes = this->quadTree.GetNumNodesInTree();
    IndexT nodeIndex;
    for (nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
    {
        QuadTree<Ptr<TreeCluster> >::Node& node = this->quadTree.NodeByIndex(nodeIndex);
        Ptr<TreeCluster> treeCluster = TreeCluster::Create();
        treeCluster->Setup(node.GetBoundingBox());
        this->treeClusters.Append(treeCluster);
        node.SetElement(treeCluster);

        // create a TreeClusterGraphicsEntity and add it to the graphics world
        Ptr<TreeClusterGraphicsEntity> graphicsEntity = TreeClusterGraphicsEntity::Create();
        graphicsEntity->SetTreeCluster(treeCluster);
        const Ptr<Stage>& stage = GraphicsServer::Instance()->GetDefaultView()->GetStage();
        stage->AttachEntity(graphicsEntity.upcast<GraphicsEntity>());
        this->treeClusterGraphicsEntities.Append(graphicsEntity);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeClusterPool::Discard()
{
    n_assert(this->IsValid());

    // discard tree cluster graphics entities
    IndexT i;
    for (i = 0; i < this->treeClusterGraphicsEntities.Size(); i++)
    {
        const Ptr<Stage>& stage = GraphicsServer::Instance()->GetDefaultView()->GetStage();
        stage->RemoveEntity(this->treeClusterGraphicsEntities[i].upcast<GraphicsEntity>());
    }
    this->treeClusterGraphicsEntities.Clear();

    // remove all tree instances
    this->RemoveAllTreeInstances();

    // discard all tree clusters
    for (i = 0; i < this->treeClusters.Size(); i++)
    {
        this->treeClusters[i]->Discard();
    }
    this->treeClusters.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
TreeClusterPool::AddTreeInstance(const Ptr<TreeInstance>& treeInstance)
{
    n_assert(this->IsValid());

    // add the tree instance to the tree instance array first
    this->treeInstances.Append(treeInstance);

    // first find the tree cluster this entity should be included in
    const bbox& box = treeInstance->GetGlobalBoundingBox();
    QuadTree<Ptr<TreeCluster>>::Node* nodePtr = this->quadTree.FindContainmentNode(box);
    if (0 == nodePtr)
    {
        // outside of level, use the root node
        nodePtr = &(this->quadTree.NodeByIndex(0));
    }
    n_assert(0 != nodePtr);

    // add the tree instance to the right TreeCluster
    nodePtr->GetElement()->AddTreeInstance(treeInstance);
}

//------------------------------------------------------------------------------
/**
    Remove a single tree instance, this is a slow operation!
*/
void
TreeClusterPool::RemoveTreeInstance(const Ptr<TreeInstance>& treeInstance)
{
    n_assert(this->IsValid());

    // remove the tree instance from the global array (slow)
    IndexT index = this->treeInstances.FindIndex(treeInstance);
    n_assert(InvalidIndex != index);
    this->treeInstances.EraseIndex(index);

    // find the cluster this tree instance is attached to (slow)
    IndexT i;
    for (i = 0; i < this->treeClusters.Size(); i++)
    {
        if (this->treeClusters[i]->HasTreeInstance(treeInstance))
        {
            this->treeClusters[i]->RemoveTreeInstance(treeInstance);
            break;
        }
    }
    treeInstance->Discard();
}

//------------------------------------------------------------------------------
/**
    Remove all tree instances, this is fast.
*/
void
TreeClusterPool::RemoveAllTreeInstances()
{
    n_assert(this->IsValid());

    // discard all trees
    this->treeInstances.Clear();

    // clear tree clusters
    IndexT i;
    for (i = 0; i < this->treeClusters.Size(); i++)
    {
        this->treeClusters[i]->RemoveAllTreeInstances();
    }
}

} // namespace Forest
