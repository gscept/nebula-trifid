//------------------------------------------------------------------------------
//  forestserver.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/rt/forestserver.h"
#include "forest/tree.h"

namespace Forest
{
__ImplementClass(Forest::ForestServer, 'FOSR', Core::RefCounted);
__ImplementSingleton(ForestServer);

using namespace Util;
using namespace Math;


//------------------------------------------------------------------------------
/**
*/
ForestServer::ForestServer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ForestServer::~ForestServer()
{
    this->Close();

    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void 
ForestServer::Open()
{
    n_assert(!this->treeClusterPool.isvalid());

    this->hasPendingTrees = false;

    // setup the tree billboard renderer
    this->treeBillboardRenderer = TreeBillboardRenderer::Create();
    this->treeBillboardRenderer->Setup();

    this->timer.Start();
}

//------------------------------------------------------------------------------
/**
*/
void 
ForestServer::Close()
{
    n_assert(!this->treeClusterPool.isvalid());
    n_assert(this->treeInstances.IsEmpty());
    n_assert(this->treeRegistry.IsEmpty());

    // discard the tree billboard renderer
    this->treeBillboardRenderer->Discard();
    this->treeBillboardRenderer = 0;

    this->hasPendingTrees = false;

    if (this->timer.Running())
    {
        this->timer.Stop();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ForestServer::LoadLevel(const Math::bbox& boundingBox)
{
    n_assert(!this->treeClusterPool.isvalid());

    this->hasPendingTrees = false;

    // setup a new tree cluster pool
    this->treeClusterPool = TreeClusterPool::Create();
    this->treeClusterPool->Setup(boundingBox);

    // notify tree billboard renderer
    this->treeBillboardRenderer->LoadLevel();
}

//------------------------------------------------------------------------------
/**
*/
void 
ForestServer::ExitLevel()
{
    n_assert(this->treeClusterPool.isvalid());

    this->hasPendingTrees = false;

    // notify tree billboard renderer
    this->treeBillboardRenderer->ExitLevel();

    // delete all trees 
    this->CleanupTrees();

    // discard the tree cluster pool
    this->treeClusterPool->Discard();
    this->treeClusterPool = 0;

    // discard the collide shape
   /* if (this->collideShape.isvalid())
    {
        if (Physics::Server::Instance()->GetLevel())
        {
            Physics::Server::Instance()->GetLevel()->RemoveStaticShape(this->collideShape);
        }
        this->collideShape = 0;
    }*/
}

//------------------------------------------------------------------------------
/**
*/
void 
ForestServer::CreateTreeInstance(const StringAtom& resourceId, const StringAtom& id, const matrix44& transform, const StringAtom& collideFile)
{
    n_assert(this->treeClusterPool.isvalid());
    this->hasPendingTrees = true;

    // lookup tree template
    Ptr<Tree> tree;
    if (this->treeRegistry.Contains(resourceId))
    {
        tree = this->treeRegistry[resourceId];
    }
    else
    {
        tree = Tree::Create();
        tree->Setup(resourceId, collideFile);
        this->treeRegistry.Add(resourceId, tree);
        // create dummy entry
        this->pendingTreeInstances.Add(tree, Array<TreeInstanceInfo>());
    }

    // add tree instance to the pending instance dicitonary
    TreeInstanceInfo info;
    info.treeId = id;
    info.transform = transform;
    this->pendingTreeInstances[tree].Append(info);
}


//------------------------------------------------------------------------------
/**
*/
void
ForestServer::CleanupTrees()
{
    n_assert(this->treeClusterPool.isvalid());

    this->pendingTreeInstances.Clear();

    // discard tree instances
    this->treeClusterPool->RemoveAllTreeInstances();
    IndexT i;
    for (i = 0; i < this->treeInstances.Size(); i++)
    {
        this->treeInstances.ValueAtIndex(i)->Discard();
    }
    this->treeInstances.Clear();

    // discard tree templates
    for (i = 0; i < this->treeRegistry.Size(); i++)
    {
        this->treeRegistry.ValueAtIndex(i)->Discard();
    }
    this->treeRegistry.Clear();
}

//------------------------------------------------------------------------------
/**
    THIS IS A SLOW OPERATION!
*/
void
ForestServer::DeleteTreeInstance(const StringAtom& id)
{
    n_assert(this->HasTreeInstance(id));
    this->treeInstances[id]->Discard();
    this->treeInstances.Erase(id);
}

//------------------------------------------------------------------------------
/**
*/
bool
ForestServer::HasTreeInstance(const StringAtom& id) const
{
    return this->treeInstances.Contains(id);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<TreeInstance>
ForestServer::LookupTreeInstance(const StringAtom& id) const
{
    return this->treeInstances[id];
}

//------------------------------------------------------------------------------
/**
    Setup the tree collides. Call this method once after trees have been
    loaded into the level. All collide geometry will be merged into
    a single collide mesh.
*/
void
ForestServer::SetupCollideMesh()
{
    /*
    n_assert(!this->collideShape.isvalid());

    // drop out early if no trees exist
    if (this->treeInstances.IsEmpty())
    {
        return;
    }

    // first count the overall number of vertices and indices
    SizeT allNumVertices = 0;
    SizeT allNumIndices = 0;
    SizeT vertexWidth = 0;
    IndexT i;
    for (i = 0; i < this->treeInstances.Size(); i++)
    {
        const Ptr<Tree>& tree = this->treeInstances.ValueAtIndex(i)->GetTree();
        const Ptr<Physics::Mesh>& collMesh = tree->GetCollideMesh();
        if (collMesh.isvalid())
        {
            allNumVertices += collMesh->GetNumVertices();
            allNumIndices += collMesh->GetNumIndices();
            if (0 == vertexWidth)
            {
                vertexWidth = collMesh->GetVertexWidth();
            }
            else
            {
                n_assert(collMesh->GetVertexWidth() == vertexWidth);
            }
        }
    }

    // if none of the trees has a collision, return early
    if (0 == allNumVertices)
    {
        return;
    }

    // create a compound physics mesh with the entire collide geometry
    Ptr<Physics::Mesh> compoundMesh = Physics::Mesh::Create();
    compoundMesh->BeginAppendMeshes(allNumVertices, allNumIndices, vertexWidth);
    for (i = 0; i < this->treeInstances.Size(); i++)
    {
        const Ptr<TreeInstance>& treeInst = this->treeInstances.ValueAtIndex(i);
        const Ptr<Tree> tree = treeInst->GetTree();
        const Ptr<Physics::Mesh>& collMesh = tree->GetCollideMesh();
        if (collMesh.isvalid())
        {
            compoundMesh->AppendMesh(collMesh, treeInst->GetTransform());
        }
    }
    compoundMesh->EndAppendMeshes();
    
    // create a physics mesh and add it to the physics world
    this->collideShape = Physics::MeshShape::Create();
    this->collideShape->SetMesh(compoundMesh);
    this->collideShape->SetCategory(Physics::ShapeCategory::Forest);
    this->collideShape->Setup();
    Physics::Server::Instance()->GetLevel()->AttachStaticShape(this->collideShape);
    */
}

//------------------------------------------------------------------------------
/**
	Called before rendering
*/
void
ForestServer::OnRenderBefore(IndexT frameId, Timing::Time time)
{
	// begin rendering
    TreeBillboardRenderer::Instance()->Begin();

    if (!this->hasPendingTrees)
    {
        return;
    }

    this->hasPendingTrees = false;

    // check all pending trees
    for (IndexT index = 0; index < this->pendingTreeInstances.Size(); index++)
    {
        // if tree is valid create all pending tree instances for this tree type
        // and add them to the tree cluster
        const Ptr<Tree>& tree = this->pendingTreeInstances.KeyAtIndex(index);
        tree->ValidateModelInstance();
        if (tree->IsValid())
        {
            Array<TreeInstanceInfo>& instanceInfos = this->pendingTreeInstances.ValueAtIndex(index);
            for (IndexT infoId = 0; infoId < instanceInfos.Size(); infoId++)
            {
                // create a tree instance from the tree template
                TreeInstanceInfo info = instanceInfos[infoId];
                Ptr<TreeInstance> treeInstance = TreeInstance::Create();
                treeInstance->Setup(tree, info.treeId, info.transform);
                this->treeInstances.Add(info.treeId, treeInstance);

                // register the new tree instance with the tree cluster pool
                this->treeClusterPool->AddTreeInstance(treeInstance);
            }
            instanceInfos.Clear();

            // hmm, .. do not delete, keep dummy entry see CreateTreeInstance()
            //this->pendingTreeInstances.EraseAtIndex(index);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ForestServer::OnRenderAfter(IndexT frameId, Timing::Time time)
{
    // all forest entities rendered
    // render tree billboards now
    TreeBillboardRenderer::Instance()->End();
}

} // namespace Forest
