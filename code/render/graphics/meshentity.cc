//------------------------------------------------------------------------------
// meshentity.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "meshentity.h"
#include "models/modelinstance.h"
#include "models/nodes/meshnode.h"
#include "models/visresolver.h"
#include "coregraphics/renderdevice.h"

using namespace Models;
namespace Graphics
{

__ImplementClass(Graphics::MeshEntity, 'MSEN', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
MeshEntity::MeshEntity() :
	model(0),
	modelInstance(0)
{
	this->SetType(GraphicsEntityType::Model);
}

//------------------------------------------------------------------------------
/**
*/
MeshEntity::~MeshEntity()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::AddNode(const Util::StringAtom& name, const CoreGraphics::PrimitiveGroup& group, const Resources::ResourceId& surface, const Math::bbox& box)
{
	__StagingModelNode node;
	node.name = name;
	node.group = group;
	node.surface = surface;
	node.boundingbox = box;
	this->stagingNodes.Append(node);
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::ApplyState(const CoreGraphics::PrimitiveGroup& prim)
{
	CoreGraphics::RenderDevice* renderDev = CoreGraphics::RenderDevice::Instance();

	// apply buffers
	IndexT i;
	for (i = 0; i < this->vbos.Size(); i++)
	{
		const VboBinding& binding = this->vbos[i];
		renderDev->SetStreamVertexBuffer(binding.Key(), binding.Value(), 0);
	}
	renderDev->SetVertexLayout(this->layout);

	if (this->ibo.isvalid())
	{
		renderDev->SetIndexBuffer(this->ibo);
	}

	// apply layout and node primitive
	renderDev->SetPrimitiveGroup(prim);
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnActivate()
{
	n_assert(!this->IsActive());
	n_assert(!this->modelInstance.isvalid());
	n_assert(this->components.Size() > 0);
	GraphicsEntity::OnActivate();

	this->model = Model::Create();
	this->layout = CoreGraphics::VertexLayout::Create();

	IndexT i;
	for (i = 0; i < this->vbos.Size(); i++)
	{
		const VboBinding& binding = this->vbos[i];
		this->layout->SetStreamBuffer(binding.Key(), binding.Value());
	}
	this->layout->Setup(this->components);
	this->components.Clear();

	Ptr<Models::TransformNode> rootNode = Models::TransformNode::Create();
	rootNode->SetName("root");

	this->model->AttachNode(rootNode.upcast<ModelNode>());

	Math::bbox globalBox;
	globalBox.begin_extend();
	
	for (i = 0; i < this->stagingNodes.Size(); i++)
	{
		const __StagingModelNode& data = this->stagingNodes[i];

		// setup node
		Ptr<Models::MeshNode> node = Models::MeshNode::Create();
		node->SetEntity(this);
		node->SetPrimitiveGroup(data.group);
		node->SetName(data.name);
		node->SetSurface(data.surface);
		node->SetBoundingBox(data.boundingbox);
		node->LoadResources(true);

		globalBox.extend(data.boundingbox);
		
		// add node to model
		rootNode->AddChild(node.upcast<ModelNode>());
		this->model->AttachNode(node.upcast<ModelNode>());
	}	
	this->stagingNodes.Clear();
	globalBox.end_extend();
	rootNode->SetBoundingBox(globalBox);
	
	this->modelInstance = this->model->CreateInstance();
	this->modelInstance->SetPickingId(this->pickingId);
	this->modelInstance->SetTransform(this->transform);
	this->transformChanged = true;	
	this->SetValid(true);	
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnDeactivate()
{
	this->ibo = 0;
	this->vbos.Clear();
	this->layout->Discard();
	this->layout = 0;

	this->model->DiscardInstance(this->modelInstance);
	this->model->Unload();

	// up to parent class
	GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnHide()
{
	if (this->modelInstance.isvalid())
	{
		this->modelInstance->OnHide(this->entityTime);
	}
	GraphicsEntity::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnShow()
{
	if (this->modelInstance.isvalid())
	{
		this->modelInstance->OnShow(this->entityTime);
	}
	GraphicsEntity::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnResolveVisibility(IndexT frameIndex, bool updateLod /*= false*/)
{
	n_assert(this->modelInstance.isvalid());
	VisResolver::Instance()->AttachVisibleModelInstancePlayerCamera(frameIndex, this->modelInstance, updateLod);
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnTransformChanged()
{
	// set transform of model instance
	if (this->modelInstance.isvalid())
	{
		this->modelInstance->SetTransform(this->transform);
	}

	GraphicsEntity::OnTransformChanged();
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnNotifyCullingVisible(const Ptr<GraphicsEntity>& observer, IndexT frameIndex)
{
	if (this->IsVisible())
	{
		// call back our model-instance
		if (this->modelInstance.isvalid())
		{
			this->modelInstance->OnNotifyCullingVisible(frameIndex, this->entityTime);
		}
	}

	// call parent-class
	GraphicsEntity::OnNotifyCullingVisible(observer, frameIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
MeshEntity::OnRenderBefore(IndexT frameIndex)
{
	if (this->renderBeforeFrameIndex != frameIndex)
	{
		// if our model instance is valid, let it update itself
		if (this->modelInstance.isvalid())
		{
			this->modelInstance->OnRenderBefore(frameIndex, this->entityTime);
		}
		GraphicsEntity::OnRenderBefore(frameIndex);
	}
}

} // namespace Graphics