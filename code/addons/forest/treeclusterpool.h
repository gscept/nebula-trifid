#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::TreeClusterPool

    A pool for tree clusters. Creates new tree clusters on demand when a 
    tree instance is added to the pool.
        
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/quadtree.h"
#include "forest/treeclusterpool.h"
#include "forest/treeinstance.h"
#include "forest/treecluster.h"
#include "forest/treeclustergraphicsentity.h"

//------------------------------------------------------------------------------
namespace Forest
{
class TreeClusterPool : public Core::RefCounted
{
    __DeclareClass(TreeClusterPool);

public:
    /// constructor
    TreeClusterPool();
    /// destructor
    virtual ~TreeClusterPool();  

    /// setup the tree cluster pool with the level's bounding box
    void Setup(const Math::bbox& levelBoundingBox);
    /// discard the tree cluster pool
    void Discard();
    /// return true if tree cluster has been setup
    bool IsValid() const;

    /// add a tree instance to the pool
    void AddTreeInstance(const Ptr<TreeInstance>& treeInstance);
    /// remove a tree instance from the pool
    void RemoveTreeInstance(const Ptr<TreeInstance>& treeInstance);
    /// remove all tree instances from the pool
    void RemoveAllTreeInstances();

    /// access to quad tree which contains tree clusters
    const Util::QuadTree<Ptr<TreeCluster> >& GetQuadTree() const;

private:
    bool isValid;
    Util::Array<Ptr<TreeCluster> > treeClusters;
    Util::QuadTree<Ptr<TreeCluster> > quadTree;
    Util::Array<Ptr<TreeInstance> > treeInstances;
    Util::Array<Ptr<TreeClusterGraphicsEntity> > treeClusterGraphicsEntities;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeClusterPool::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::QuadTree<Ptr<TreeCluster> >&
TreeClusterPool::GetQuadTree() const
{
    return this->quadTree;
}
} 