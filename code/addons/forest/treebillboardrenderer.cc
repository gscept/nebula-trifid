//------------------------------------------------------------------------------
//  treebillboardrenderer.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/treebillboardrenderer.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "resources/resourceloader.h"
#include "coregraphics/vertexlayoutserver.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "models/nodes/shapenode.h"
#include "resources/managedmesh.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/memoryindexbufferloader.h"

namespace Forest
{
__ImplementClass(Forest::TreeBillboardRenderer, 'FOTR', Core::RefCounted);
__ImplementSingleton(Forest::TreeBillboardRenderer);

using namespace Util;
using namespace Math;
using namespace CoreGraphics;
using namespace Resources;
using namespace Graphics;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
TreeBillboardRenderer::TreeBillboardRenderer() :
    isValid(false),
    inBegin(false),
	vertexBuffer(0),
	indexBuffer(0),
	vertexLayout(0),
    treeInstances(1024, 1024)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
TreeBillboardRenderer::~TreeBillboardRenderer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
TreeBillboardRenderer::Setup()
{
    n_assert(!this->IsValid());
    n_assert(!this->inBegin);
    n_assert(this->treeInstances.IsEmpty());

    this->isValid = true;

    // create dynamic vertex buffer
    Array<VertexComponent> vertexComponents;
    vertexComponents.Append(VertexComponent((VertexComponent::SemanticName)0, 0, VertexComponent::Float4, 1));   // Particle::position
    vertexComponents.Append(VertexComponent((VertexComponent::SemanticName)1, 0, VertexComponent::Float4, 1));   // Particle::stretchPosition
    vertexComponents.Append(VertexComponent((VertexComponent::SemanticName)2, 0, VertexComponent::Float4, 1));      // Particle::color
    vertexComponents.Append(VertexComponent((VertexComponent::SemanticName)3, 0, VertexComponent::Float4, 1));  // Particle::uvMinMax
    vertexComponents.Append(VertexComponent((VertexComponent::SemanticName)4, 0, VertexComponent::Float4, 1));  // x: Particle::rotation, y: Particle::size

    Ptr<MemoryVertexBufferLoader> vbL = MemoryVertexBufferLoader::Create();
    vbL->Setup(vertexComponents, MaxNumVertices, NULL, 0, VertexBuffer::UsageDynamic, VertexBuffer::AccessWrite);

    this->vertexBuffer = VertexBuffer::Create();
    this->vertexBuffer->SetLoader(vbL.upcast<ResourceLoader>());
    this->vertexBuffer->SetAsyncEnabled(false);
    this->vertexBuffer->Load();
    if (!this->vertexBuffer->IsLoaded())
    {
        n_error("TreeBillboardRenderer: Failed to setup tree billboard vertex buffer!");
    }
    this->vertexBuffer->SetLoader(0);

	// create dynamic index pointer
	Ptr<MemoryIndexBufferLoader> cornerIBLoader = MemoryIndexBufferLoader::Create();
	cornerIBLoader->Setup(IndexType::Index16, MaxNumIndices, 0, 0, IndexBuffer::UsageDynamic, IndexBuffer::AccessWrite);

	this->indexBuffer = IndexBuffer::Create();
	this->indexBuffer->SetLoader(cornerIBLoader.upcast<ResourceLoader>());
	this->indexBuffer->SetAsyncEnabled(false);
	this->indexBuffer->Load();
	if (!this->indexBuffer->IsLoaded())
	{
		n_error("TreeBillboardRenderer: Failed to setup corner index buffer!");
	}
	this->indexBuffer->SetLoader(0);

	// we need to setup a common vertex layout which describes both streams
    this->vertexLayout = this->vertexBuffer->GetVertexLayout();
}

//------------------------------------------------------------------------------
/**
*/
void
TreeBillboardRenderer::Discard()
{
    n_assert(this->IsValid());

    this->vertexBuffer->Unload();
    this->vertexBuffer = 0;
	this->indexBuffer->Unload();
	this->indexBuffer = 0;
	this->vertexLayout->Discard();
	this->vertexLayout = 0;

    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
    We need to register a TreeBillboardGraphicsEntity which notifies
    us about the currently used directional light source at render time.
*/
void
TreeBillboardRenderer::LoadLevel()
{
    n_assert(this->IsValid());
    this->graphicsEntity = TreeBillboardGraphicsEntity::Create();
	GraphicsServer::Instance()->GetDefaultView()->GetStage()->AttachEntity(this->graphicsEntity.upcast<GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
    Called when the current level is left, we need to unlink our 
    graphics entity here.
*/
void
TreeBillboardRenderer::ExitLevel()
{
    n_assert(this->IsValid());

    // cleanup tree arrays
    this->CleanupTrees();

    // unlink our graphics entity
	GraphicsServer::Instance()->GetDefaultView()->GetStage()->RemoveEntity(this->graphicsEntity.upcast<GraphicsEntity>());
    this->graphicsEntity = 0;
}

//------------------------------------------------------------------------------
/**
    Cleanup the tree dictionary. This method must be called from
    TreeServer::CleanupTrees().
*/
void
TreeBillboardRenderer::CleanupTrees()
{
    n_assert(this->IsValid());
    this->treeInstances.Clear();
}

//------------------------------------------------------------------------------
/**
    Call this method once per frame before rendering tree instances.
*/
void
TreeBillboardRenderer::Begin()
{
    n_assert(this->IsValid());
    n_assert(!this->inBegin);

    this->inBegin = true;
    this->treeInstances.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
TreeBillboardRenderer::AddTreeInstance(const Ptr<TreeInstance>& treeInstance)
{
    n_assert(this->inBegin);

    // get the distance to the viewer for sorting
    const vector& treePos = treeInstance->GetTransform().get_position();
    float distToViewer = vector(treePos - this->viewerPos).length();
    float invDistToViewer = 1.0f / n_max(distToViewer, 1.0f);

    // add to instance array
    this->treeInstances.Append(KeyValuePair<float, TreeInstance*>(invDistToViewer, treeInstance));
}

//------------------------------------------------------------------------------
/**
    Populates the dynamic mesh with the collected tree instance billboards.
*/
void
TreeBillboardRenderer::End()
{
    n_assert(this->inBegin);
    this->inBegin = false;

    // sort the tree instance array by distance to viewer
    this->treeInstances.Sort();

    // populate vertex and index buffers
    this->UpdateDynamicMesh();
    
    // render the billboards
    this->Render();
}

//------------------------------------------------------------------------------
/**
*/
void
TreeBillboardRenderer::UpdateDynamicMesh()
{
    if (this->treeInstances.IsEmpty())
    {
        return;
    }

	float* vertices = (float*)this->vertexBuffer->Map(VertexBuffer::MapWriteDiscard);
	ushort* indices = (ushort*)this->indexBuffer->Map(IndexBuffer::MapWriteDiscard);
    float* dstVertexPtr = vertices;
    ushort* dstIndexPtr = indices;
    ushort dstVertexIndex = 0;
    IndexT dstIndexIndex = 0;

    IndexT treeIndex;
    for (treeIndex = 0; treeIndex < this->treeInstances.Size(); treeIndex++)
    {
        TreeInstance* treeInst = this->treeInstances[treeIndex].Value();
        const Ptr<Tree>& tree = treeInst->GetTree();

        // get world transform, lod index and fade alpha value from the tree instance
        const matrix44& tform = treeInst->GetTransform();
        IndexT lodIndex;
        float fadeAlpha, bbScale;
        treeInst->GetBillboardData(lodIndex, fadeAlpha, bbScale);
        n_assert(InvalidIndex != lodIndex);

        // get the right TreeLOD object
        const Ptr<TreeLOD>& treeLod = tree->GetLODs()[lodIndex];

        // update the per-instance data, so that we can simply
        // copy over a chunk of vertices into our dynamic vertex buffer
        SizeT srcNumVertices = treeLod->GetNumVertices();
        SizeT srcVertexWidth = treeLod->GetVertexWidth();
        const float* srcVertexPtr  = treeLod->GetVertices();
        SizeT vertexChunkSize = srcNumVertices * srcVertexWidth * sizeof(float);
        treeLod->UpdateInstanceData(treeInst->GetTransform(), fadeAlpha, bbScale);
                
        // copy over the vertex data
        n_assert((dstVertexIndex + srcNumVertices) < MaxNumVertices);
        Memory::Copy(srcVertexPtr, dstVertexPtr, vertexChunkSize);

        // copy over index data, add vertex base index
        SizeT srcNumIndices = treeLod->GetNumIndices();
        const ushort* srcIndexPtr = treeLod->GetIndices();
        IndexT i;
        for (i = 0; i < srcNumIndices; i++)
        {
            *dstIndexPtr = *srcIndexPtr + dstVertexIndex;
            dstIndexPtr++;
            srcIndexPtr++;
        }

        // update destination pointers and index trackers
        dstVertexPtr += srcNumVertices * srcVertexWidth;
        dstVertexIndex += srcNumVertices;
        dstIndexIndex  += srcNumIndices;
    }
    this->vertexBuffer->Unmap();
    this->indexBuffer->Unmap();

	PrimitiveGroup& meshGroup = this->group;
	meshGroup.SetBaseVertex(0);
	meshGroup.SetBaseIndex(0);
	meshGroup.SetNumVertices(dstVertexIndex);
	meshGroup.SetNumIndices(dstIndexIndex);
	Array<PrimitiveGroup> groups;
	groups.Append(this->group);

    // setup the shape node of the first tree instance for rendering 
    // the entire billboard set
    const Ptr<ShapeNode>& shapeNode = this->treeInstances[0].Value()->GetTree()->GetBillboardLOD()->GetBillboardShapeNode();
	this->vertexBuffer->SetVertexLayout(this->vertexLayout);
	shapeNode->GetManagedMesh()->GetMesh()->SetVertexBuffer(this->vertexBuffer);
	shapeNode->GetManagedMesh()->GetMesh()->SetIndexBuffer(this->indexBuffer);
	shapeNode->GetManagedMesh()->GetMesh()->SetPrimitiveGroups(groups);    
}

//------------------------------------------------------------------------------
/**
*/
void
TreeBillboardRenderer::Render()
{
	/*
	n_assert(this->vertexBuffer.isvalid());
	n_assert(this->indexBuffer.isvalid());
	RenderDevice* renderDevice = RenderDevice::Instance();

	renderDevice->SetStreamSource(0, this->vertexBuffer, 0);
	renderDevice->SetIndexBuffer(this->indexBuffer);
	renderDevice->SetVertexLayout(this->vertexLayout);
	renderDevice->SetPrimitiveGroup(this->group);
	renderDevice->Draw();
	*/
}

} // namespace Forest
