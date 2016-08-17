//------------------------------------------------------------------------------
//  billboardnodeinstance.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "billboardnodeinstance.h"
#include "models/modelnode.h"
#include "models/nodes/transformnode.h"
#include "models/nodes/statenodeinstance.h"
#include "models/modelnodeinstance.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/vertexlayoutserver.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "resources/resourceloader.h"
#include "coregraphics/renderdevice.h"

using namespace CoreGraphics;
using namespace Util;
using namespace Resources;
using namespace Models;
namespace Billboards
{
__ImplementClass(Billboards::BillboardNodeInstance, 'BINI', Models::StateNodeInstance);

//------------------------------------------------------------------------------
/**
*/
BillboardNodeInstance::BillboardNodeInstance() : 
	vb(0),
	ib(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BillboardNodeInstance::~BillboardNodeInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
BillboardNodeInstance::OnVisibilityResolve(IndexT resolveIndex, float distToViewer)
{
	// check if node is inside lod distances or if no lod is used
	const Ptr<TransformNode>& transformNode = this->modelNode.downcast<TransformNode>();
	if (transformNode->CheckLodDistance(distToViewer))
	{
        this->modelNode->AddVisibleNodeInstance(resolveIndex, this->surfaceInstance->GetCode(), this);
		ModelNodeInstance::OnVisibilityResolve(resolveIndex, distToViewer);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
BillboardNodeInstance::Setup(const Ptr<ModelInstance>& inst, const Ptr<ModelNode>& node, const Ptr<ModelNodeInstance>& parentNodeInst)
{
	n_assert(!this->vb.isvalid());
	n_assert(!this->ib.isvalid());

	// up to parent class
	StateNodeInstance::Setup(inst, node, parentNodeInst);

	// setup the corner vertex buffer
	Array<VertexComponent> components;
	components.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float2, 0));
	components.Append(VertexComponent(VertexComponent::TexCoord1, 0, VertexComponent::Float2, 0));
	float cornerVertexData[] = { -0.5, -0.5, 0, 1,  -0.5, 0.5, 0, 0,  0.5, 0.5, 1, 0,  0.5, -0.5, 1, 1 };
	Ptr<MemoryVertexBufferLoader> vbLoader = MemoryVertexBufferLoader::Create();
	vbLoader->Setup(components, 4, cornerVertexData, sizeof(cornerVertexData), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);

	this->vb = VertexBuffer::Create();
	this->vb->SetLoader(vbLoader.upcast<ResourceLoader>());
	this->vb->SetAsyncEnabled(false);
	this->vb->Load();
	if (!this->vb->IsLoaded())
	{
		n_error("BillboardNodeInstance: Failed to setup billboard vertex buffer!");
	}
	this->vb->SetLoader(0);

	// setup the corner index buffer
	ushort cornerIndexData[] = { 0, 1, 2, 2, 3, 0 };
	Ptr<MemoryIndexBufferLoader> ibLoader = MemoryIndexBufferLoader::Create();
	ibLoader->Setup(IndexType::Index16, 6, cornerIndexData, sizeof(cornerIndexData), IndexBuffer::UsageImmutable, IndexBuffer::AccessNone);

	this->ib = IndexBuffer::Create();
	this->ib->SetLoader(ibLoader.upcast<ResourceLoader>());
	this->ib->SetAsyncEnabled(false);
	this->ib->Load();
	if (!this->ib->IsLoaded())
	{
		n_error("BillboardNodeInstance: Failed to setup billboard index buffer!");
	}
	this->ib->SetLoader(0);

	// setup the primitive group
	this->primGroup.SetBaseVertex(0);
	this->primGroup.SetNumVertices(4);
	this->primGroup.SetBaseIndex(0);
	this->primGroup.SetNumIndices(6);
	this->primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
}

//------------------------------------------------------------------------------
/**
*/
void 
BillboardNodeInstance::Discard()
{
	n_assert(this->vb->IsLoaded());
	n_assert(this->ib->IsLoaded());

	this->vb->Unload();
	this->vb = 0;
	this->ib->Unload();
	this->ib = 0;

	StateNodeInstance::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void 
BillboardNodeInstance::Render()
{
	StateNodeInstance::Render();
	RenderDevice* renderDevice = RenderDevice::Instance();

	// setup rendering
	renderDevice->SetVertexLayout(this->vb->GetVertexLayout());
	renderDevice->SetPrimitiveGroup(this->primGroup);
	renderDevice->SetStreamVertexBuffer(0, this->vb, 0);
	renderDevice->SetIndexBuffer(this->ib);

	// draw geometry
	renderDevice->Draw();
}
} // namespace Models