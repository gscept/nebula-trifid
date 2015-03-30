#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::TreeCluster

    A tree cluster groups tree instances of the different types for efficient
    culling and rendering. A TreeCluster is associated with a 
    TreeClusterGraphicsEntity which links the tree cluster to the Graphics
    subsystem.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "math/bbox.h"
#include "forest/treeinstance.h"
#include "forest/tree.h"
#include "graphics/cameraentity.h"

//------------------------------------------------------------------------------
namespace Forest
{
class TreeCluster : public Core::RefCounted
{
    __DeclareClass(TreeCluster);
public:
    /// constructor
    TreeCluster();
    /// destructor
    virtual ~TreeCluster();
    
    /// setup the tree cluster for a given tree type
    void Setup(const Math::bbox& boundingBox);
    /// discard the tree cluster
    void Discard();
    /// return true if tree cluster has been setup
    bool IsValid() const;

    /// get the bounding box of the tree cluster
    const Math::bbox& GetBoundingBox() const;
    /// add a tree instance to the cluster
    void AddTreeInstance(const Ptr<TreeInstance>& treeInstance);
    /// remove a tree instance from the cluster (SLOW!)
    void RemoveTreeInstance(const Ptr<TreeInstance>& treeInstance);
    /// return true if tree instance is part of this cluster (SLOW!)
    bool HasTreeInstance(const Ptr<TreeInstance>& treeInstance) const;
    /// remove all tree instances (FAST)
    void RemoveAllTreeInstances();
    /// get tree instances, sorted by tree type
    const Util::Dictionary<Ptr<Tree>, Util::Array<Ptr<TreeInstance> > >& GetTreeInstances() const;
    /// count the overall number of tree instances in the cluster
    SizeT CountTreeInstances() const;
    /// returns true if tree istances were added to this tree cluster
    bool HasTreeInstances() const;
    
    /// render the tree cluster
    void OnRenderBefore(const Ptr<Graphics::CameraEntity>& cameraEntity, IndexT frameIndex);

protected:


private:
    Math::bbox box;
    Util::Dictionary<Ptr<Tree>, Util::Array<Ptr<TreeInstance> > > treeInstances;
    bool isValid;
    bool hasTreeInstances;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeCluster::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox&
TreeCluster::GetBoundingBox() const
{
    return this->box;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Dictionary<Ptr<Tree>, Util::Array<Ptr<TreeInstance> > >&
TreeCluster::GetTreeInstances() const
{
    return this->treeInstances;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeCluster::HasTreeInstances() const
{
    return this->hasTreeInstances;
}

} // namespace Forest
//------------------------------------------------------------------------------
