//------------------------------------------------------------------------------
//  treelustergraphicsentity.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/treeclustergraphicsentity.h"
#include "models/visresolver.h"
#include "math/float4.h"
#include "coregraphics/shaperenderer.h"
#include "threading/thread.h"
#include "graphics/cameraentity.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"

namespace Forest
{
__ImplementClass(Forest::TreeClusterGraphicsEntity, 'TCGE', Graphics::GraphicsEntity);

using namespace Util;
using namespace Math;
using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
TreeClusterGraphicsEntity::TreeClusterGraphicsEntity()
{
    this->SetType(GraphicsEntityType::Tree);
}

//------------------------------------------------------------------------------
/**
*/
TreeClusterGraphicsEntity::~TreeClusterGraphicsEntity()
{
    n_assert(!this->treeCluster.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
TreeClusterGraphicsEntity::SetTreeCluster(const Ptr<TreeCluster>& tc)
{
    this->treeCluster = tc;
    this->SetLocalBoundingBox(tc->GetBoundingBox());
    // set transform changed, to update global bounding box
    this->transformChanged = true;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<TreeCluster>&
TreeClusterGraphicsEntity::GetTreeCluster() const
{
    return this->treeCluster;
}

//------------------------------------------------------------------------------
/**
    Need to override the Graphics entity's activation.
*/
void
TreeClusterGraphicsEntity::OnActivate()
{
    //n_assert(!this->active);
    n_assert(this->treeCluster.isvalid());
    GraphicsEntity::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
TreeClusterGraphicsEntity::OnDeactivate()
{
    n_assert(this->treeCluster.isvalid());
    this->treeCluster = 0;
    
    GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This method is called on the entity from InternalView::Render()
    once per frame for every visible entity.
*/
void
TreeClusterGraphicsEntity::OnRenderBefore(IndexT frameIndex)
{   
    if (this->IsVisible() && this->treeCluster->HasTreeInstances())
    {
        const Ptr<CameraEntity>& camera = GraphicsServer::Instance()->GetDefaultView()->GetCameraEntity();
        this->treeCluster->OnRenderBefore(camera, frameIndex);
    }

    GraphicsEntity::OnRenderBefore(frameIndex);
}

//------------------------------------------------------------------------------
/**
    Render our debug visualization (the bounding box).
*/
void
TreeClusterGraphicsEntity::OnRenderDebug()
{
    // render bounding box
    float4 color(1.0f, 0.0f, 0.0f, 0.15f);
    color.set(1.0f, 0.1f, 0.1f, 0.8f);
    CoreGraphics::ShapeRenderer::Instance()->AddWireFrameBox(
        this->GetGlobalBoundingBox(), 
        color, 
        Threading::Thread::GetMyThreadId());

    // let model instance render its debug stuff
    //this->modelInstance->RenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void 
TreeClusterGraphicsEntity::OnReset()
{
    GraphicsEntity::OnReset();
}

//------------------------------------------------------------------------------
/**
    This method is called whenever the the internalview comes to its Render method. 
    Override this method in a subclass to define the visible objects. Add the 
    visible tree instances to the Vis resolver. If a tree instance should be rendered
    as a billboard, GetModelInstance returns an invalid pointer.
*/
void
TreeClusterGraphicsEntity::OnResolveVisibility(IndexT frameIndex)
{
    // cluster is not visible or has no tree instances
    if (!this->IsVisible() || !this->treeCluster->HasTreeInstances())
    {
        return;
    }

    Util::Dictionary<Ptr<Tree>, Util::Array<Ptr<TreeInstance> > > treeInstanceMap = this->treeCluster->GetTreeInstances();

    // run through all tree types
    for (IndexT treeId = 0; treeId < treeInstanceMap.Size(); treeId++)
    {
        bool hasBillboard = false;
        const Util::Array<Ptr<TreeInstance> >& treeInstances = treeInstanceMap.ValueAtIndex(treeId);
        // run through all tree instances in tree type
        for (IndexT treeInstanceId = 0; treeInstanceId < treeInstances.Size(); treeInstanceId++)
        {
            n_assert(treeInstances[treeInstanceId].isvalid());
            const Ptr<Models::ModelInstance>& treeInst = treeInstances[treeInstanceId]->GetModelInstance();
            // if tree is a billboard GetModelInstance() returns 0
            if(treeInst.isvalid())
            {
                Models::VisResolver::Instance()->AttachVisibleModelInstancePlayerCamera(frameIndex, treeInst, true);
            }
            else
            {
                hasBillboard = true;
            }
        }

        if (hasBillboard)
        {
            // add a billboard model instance for this cluster and this tree type
            const Ptr<Tree>& tree = treeInstanceMap.KeyAtIndex(treeId);
            const Ptr<Models::ModelInstance> billboardInst = tree->GetBillboardLOD()->GetBillboardInstance();
			Models::VisResolver::Instance()->AttachVisibleModelInstancePlayerCamera(frameIndex, billboardInst, true);
        }
    }
}

//------------------------------------------------------------------------------
/**
    In OnTransformChanged() we need to update the transformation
    of our ModelInstance (if it has already been initialised).
*/
void
TreeClusterGraphicsEntity::OnTransformChanged()
{
    // Do it for all trees in the cluster
	/*
    if (this->modelInstance.isvalid())
    {
        if (this->HasCharacter() && this->nebula2CharacterRotationHack)
        {
            // AAARGH implement the age old 180 degree hack for Nebula characters
            matrix44 m = matrix44::multiply(matrix44::rotationy(n_deg2rad(180.0f)), this->GetTransform());
            this->modelInstance->SetTransform(m);
        }
        else
        {
            this->modelInstance->SetTransform(this->GetTransform());
        }
    }
	*/
}

} // namespace Forest
