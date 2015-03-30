#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::ForestServer
        
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "timing/time.h"
#include "forest/treeclusterpool.h"
#include "forest/treeinstance.h"
#include "forest/treebillboardrenderer.h"
#include "math/bbox.h"

//------------------------------------------------------------------------------
namespace Forest
{
class ForestServer : public Core::RefCounted
{
    __DeclareClass(ForestServer);
    __DeclareSingleton(ForestServer);

public:

    /// constructor
    ForestServer();
    /// destructor
    virtual ~ForestServer();  
    
    /// open the posteffect server
    void Open();
    /// close the posteffect server
    void Close();

    /// prepare forest server for a new level
    void LoadLevel(const Math::bbox& boundingBox);
    /// cleanup forest server
    void ExitLevel();
    /// prepare forest server for a new level
    void CreateTreeInstance(const Util::StringAtom& resourceId, const Util::StringAtom& id, const Math::matrix44& transform, const Util::StringAtom& collideFile);

    /// set the current point of interest, update per frame!
    void SetPointOfInterest(const Math::point& p);
    /// get current point of interest
    const Math::point& GetPointOfInterest() const;

    /// remove a tree instance from the world
    void DeleteTreeInstance(const Util::StringAtom& id);
    /// return true if a tree instance exists by id
    bool HasTreeInstance(const Util::StringAtom& id) const;
    /// lookup tree instance by id
    Ptr<TreeInstance> LookupTreeInstance(const Util::StringAtom& id) const;
    /// cleanup all trees and tree instances (call when level is finished)
    void CleanupTrees();
    /// setup tree collides, call once after all tree instances have been created
    void SetupCollideMesh();

    /// get number of tree types
    SizeT GetNumTrees() const;
    /// get tree by index
    const Ptr<Tree>& GetTreeAtIndex(IndexT index) const;
    /// get number of tree instances
    SizeT GetNumTreeInstances() const;
    /// get tree instance by index
    const Ptr<TreeInstance>& GetTreeInstanceAtIndex(IndexT index) const;

    /// get the current time (for smooth LOD transitions)
    Timing::Time GetTime() const;

    /// Called before rendering
    virtual void OnRenderBefore(IndexT frameId, Timing::Time time);
    /// called after rendering entities
    virtual void OnRenderAfter(IndexT frameId, Timing::Time time);

protected:
    struct TreeInstanceInfo
    {
        Util::StringAtom treeId;
        Math::matrix44 transform;
    };

    Ptr<TreeClusterPool> treeClusterPool;
    Ptr<TreeBillboardRenderer> treeBillboardRenderer;
    Util::Dictionary<Util::StringAtom, Ptr<Tree> > treeRegistry;
    Util::Dictionary<Util::StringAtom, Ptr<TreeInstance> > treeInstances;
    Util::Dictionary<Ptr<Tree>, Util::Array<TreeInstanceInfo> > pendingTreeInstances;

    bool hasPendingTrees;

    Timing::Timer timer;
};

//------------------------------------------------------------------------------
/**
*/
inline Timing::Time
ForestServer::GetTime() const
{
    return this->timer.GetTime();
}
} 