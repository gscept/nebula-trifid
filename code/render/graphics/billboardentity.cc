//------------------------------------------------------------------------------
//  billboardentity.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "billboardentity.h"
#include "resources/resourcemanager.h"
#include "coregraphics/shaderserver.h"
#include "billboards/billboardnodeinstance.h"
#include "models/nodes/statenodeinstance.h"
#include "models/visresolver.h"

using namespace Resources;
using namespace CoreGraphics;
using namespace Models;
using namespace Materials;
using namespace Models;
using namespace Graphics;
using namespace Messaging;
using namespace Math;
using namespace Billboards;

namespace Graphics
{
__ImplementClass(Graphics::BillboardEntity, 'BLEN', Graphics::GraphicsEntity);

Ptr<Models::Model> BillboardEntity::billboardModel = 0;
Ptr<Billboards::BillboardNode> BillboardEntity::billboardNode = 0;

//------------------------------------------------------------------------------
/**
*/
BillboardEntity::BillboardEntity() : 
	texture(0),
    color(1,1,1,1),
	modelInstance(0),
	viewAligned(false)
{
	this->SetType(GraphicsEntityType::Model);
}

//------------------------------------------------------------------------------
/**
*/
BillboardEntity::~BillboardEntity()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
BillboardEntity::OnActivate()
{
	n_assert(!this->IsActive());
	n_assert(this->resource.IsValid());
	n_assert(!this->texture.isvalid());
	n_assert(!this->modelInstance.isvalid());
	n_assert(!this->textureVariable.isvalid());
	GraphicsEntity::OnActivate();

	ResourceManager* resManager = ResourceManager::Instance();
	ShaderServer* shdServer = ShaderServer::Instance();

	// create texture
	this->texture = resManager->CreateManagedResource(Texture::RTTI, this->resource, NULL, true).downcast<ManagedTexture>();

	// setup base model if needed
	if (!BillboardEntity::billboardModel.isvalid())
	{
		BillboardEntity::billboardModel = Model::Create();
		BillboardEntity::billboardNode = BillboardNode::Create();
		BillboardEntity::billboardNode->SetBoundingBox(Math::bbox(Math::point(0,0,0), Math::vector(1,1,1)));
        BillboardEntity::billboardNode->SetSurfaceName("sur:system/billboard");
		BillboardEntity::billboardNode->SetName("root");
		BillboardEntity::billboardNode->LoadResources(true);
		BillboardEntity::billboardModel->AttachNode(BillboardEntity::billboardNode.upcast<ModelNode>());
	}
	
	// create model instance
	this->modelInstance = BillboardEntity::billboardModel->CreateInstance();
	this->modelInstance->SetTransform(this->transform);
	this->modelInstance->SetPickingId(this->pickingId);

	// get node instance and set the view space aligned flag
	Ptr<BillboardNodeInstance> nodeInstance = this->modelInstance->GetRootNodeInstance().downcast<BillboardNodeInstance>();

    // setup material
    const Ptr<SurfaceInstance>& surface = nodeInstance->GetSurfaceInstance();
    this->textureVariable = surface->GetConstant("AlbedoMap");
    this->colorVariable = surface->GetConstant("Color");

	// create a variable instance and set the texturez
	this->textureVariable->SetTexture(this->texture->GetTexture());
	nodeInstance->SetInViewSpace(this->viewAligned);

	// set to be valid
	this->SetValid(true);
}

//------------------------------------------------------------------------------
/**
*/
void 
BillboardEntity::OnDeactivate()
{
	n_assert(this->IsActive());
	n_assert(this->texture.isvalid());
	n_assert(this->modelInstance.isvalid());
	n_assert(this->textureVariable.isvalid());

	// cleanup resources
	ResourceManager* resManager = ResourceManager::Instance();
	resManager->DiscardManagedResource(this->texture.upcast<ManagedResource>());

	// discard model instance
	this->modelInstance->GetModel()->DiscardInstance(this->modelInstance);
	this->modelInstance = 0;	

	// discard texture variable
    this->textureVariable = 0;
    this->colorVariable = 0;

	// kill model if this is our last billboard entity
	if (BillboardEntity::billboardModel->GetInstances().Size() == 0)
	{
		BillboardEntity::billboardNode->UnloadResources();
		BillboardEntity::billboardNode = 0;
		BillboardEntity::billboardModel->Unload();				
		BillboardEntity::billboardModel = 0;
	}

	// up to parent class
	GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
BillboardEntity::OnHide()
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
BillboardEntity::OnShow()
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
BillboardEntity::OnResolveVisibility(IndexT frameIndex, bool updateLod)
{
	n_assert(this->modelInstance.isvalid());
	VisResolver::Instance()->AttachVisibleModelInstancePlayerCamera(frameIndex, this->modelInstance, updateLod);
}

//------------------------------------------------------------------------------
/**
*/
void 
BillboardEntity::OnTransformChanged()
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
BillboardEntity::OnNotifyCullingVisible( const Ptr<GraphicsEntity>& observer, IndexT frameIndex )
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
BillboardEntity::SetColor(const Math::float4& color)
{
    this->color = color;

    // update billboard color
    scalar biggest = n_max(n_max(this->color.x(), this->color.y()), this->color.z());
    float4 normalizedColor = this->color;
    normalizedColor /= biggest;
    normalizedColor.set_w(1);
    this->colorVariable->SetValue(normalizedColor);
}

//------------------------------------------------------------------------------
/**
*/
void 
BillboardEntity::OnRenderBefore( IndexT frameIndex )
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

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
BillboardEntity::HandleMessage(const Ptr<Message>& msg)
{
    __Dispatch(BillboardEntity, this, msg);
}

} // namespace Graphics