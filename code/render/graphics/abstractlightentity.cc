//------------------------------------------------------------------------------
//  abstractlightentity.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/abstractlightentity.h" 
#include "coregraphics/shaperenderer.h"
#include "threading/thread.h"
#include "lighting/lightserver.h"
#include "lighting/shadowserver.h"
#include "resources/resourcemanager.h"
#include "graphics/graphicsinterface.h"
#include "algorithm/algorithmprotocol.h"


namespace Graphics
{
__ImplementClass(Graphics::AbstractLightEntity, 'ALIE', Graphics::GraphicsEntity);

using namespace Math;        
using namespace CoreGraphics;
using namespace Threading;
using namespace Messaging;
using namespace Resources;
using namespace Lighting;

//------------------------------------------------------------------------------
/**
*/
AbstractLightEntity::AbstractLightEntity() :
    lightType(LightType::InvalidLightType),
    invTransform(matrix44::identity()),
    projTransform(matrix44::identity()),
	projectionTexture(0),
    invLightProjTransform(matrix44::identity()),
    color(1.0f, 1.0f, 1.0f, 1.0f),
    projMapUvOffsetAndScale(0.5f, 0.5f, 0.5f, -0.5f),
    shadowBufferUvOffsetAndScale(0.5f, 0.5f, 0.5f, -0.5f),
	volumetric(false),
	volumetricScale(100.0f),
	volumetricIntensity(16.0f),
    castShadows(false),
    castShadowsThisFrame(false),
	shadowCastingFrequency(1),
	shadowCastingFrame(0),
    shadowIntensity(0.5f),
	shadowBias(0.0f)
{
    this->SetType(Graphics::GraphicsEntityType::Light);
}

//------------------------------------------------------------------------------
/**
*/
AbstractLightEntity::~AbstractLightEntity()
{
	// discard any reminding resources
	if (this->projectionTexture.isvalid())
	{
		// discard texture
		ResourceManager::Instance()->DiscardManagedResource(this->projectionTexture.upcast<ManagedResource>());
		this->projectionTexture = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AbstractLightEntity::TouchProjectionTexture()
{
	n_assert(this->IsAttachedToStage());
	if (this->projectionTexture.isvalid())
	{
		if (!this->projectionTextureId.IsValid())
		{
			// discard managed resource
			ResourceManager::Instance()->DiscardManagedResource(this->projectionTexture.downcast<ManagedResource>());
			this->projectionTexture = 0;
		}
		else if (this->projectionTexture->GetResourceId() != this->projectionTextureId)
		{
			// discard managed resource
			ResourceManager::Instance()->DiscardManagedResource(this->projectionTexture.downcast<ManagedResource>());

			// allocate new
			this->projectionTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, this->projectionTextureId).downcast<ManagedTexture>();
		}
	}
	else if (this->projectionTextureId.IsValid())
	{
		// allocate new
		this->projectionTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, this->projectionTextureId).downcast<ManagedTexture>();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AbstractLightEntity::SetVolumetric(bool b)
{
	this->volumetric = b;
	if (b)
	{
		Ptr<Algorithm::AddVolumetricLight> message = Algorithm::AddVolumetricLight::Create();
		message->SetLight(this);
		Graphics::GraphicsInterface::Instance()->Send(message.upcast<Messaging::Message>());
	}
	else
	{
		Ptr<Algorithm::RemoveVolumetricLight> message = Algorithm::RemoveVolumetricLight::Create();
		message->SetLight(this);
		Graphics::GraphicsInterface::Instance()->Send(message.upcast<Messaging::Message>());
	}
}

//------------------------------------------------------------------------------
/**
    This method is called whenever the the view comes to its Render method. 
    Add light entities to the LightServer or to the ShadowServer.
*/
void
AbstractLightEntity::OnResolveVisibility(IndexT frameIndex, bool updateLod)
{
	// update shadow casting this frame
	this->UpdateFrame();

    if (this->GetCastShadowsThisFrame())
    {
        // maybe cast shadows
        ShadowServer::Instance()->AttachVisibleLight(this);
    }
    else
    {
        // casts no shadows by default, can go directly into lightserver
        LightServer::Instance()->AttachVisibleLight(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AbstractLightEntity::OnTransformChanged()
{
    GraphicsEntity::OnTransformChanged();

    // update inverse transform
    this->invTransform = matrix44::inverse(this->transform);
    this->shadowTransform = this->transform;

    this->shadowTransformsDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
AbstractLightEntity::OnRenderDebug()
{
    if (this->GetCastShadowsThisFrame())
    {      
        // render shadow frustum
        float4 color(0.5f, 0.5f, 0.5f, 0.2f);

        // remove scaling, its also considered in projection matrix
        matrix44 unscaledTransform = this->shadowTransform;
        unscaledTransform.set_xaxis(float4::normalize(unscaledTransform.get_xaxis()));
        unscaledTransform.set_yaxis(float4::normalize(unscaledTransform.get_yaxis()));
        unscaledTransform.set_zaxis(float4::normalize(unscaledTransform.get_zaxis()));
        matrix44 frustum = matrix44::multiply(matrix44::inverse(this->GetShadowProjTransform()), unscaledTransform);
        ShapeRenderer::Instance()->AddShape(RenderShape(Thread::GetMyThreadId(), RenderShape::Box, RenderShape::CheckDepth, frustum, color));
                                   
        const point unitCube[24] = {point(1,1,1),  point(1,1,-1),  point(1,1,-1),  point(-1,1,-1),  point(-1,1,-1),  point(-1,1,1),   point(-1,1,1),  point(1,1,1),
                                    point(1,-1,1), point(1,-1,-1), point(1,-1,-1), point(-1,-1,-1), point(-1,-1,-1), point(-1,-1,1),  point(-1,-1,1), point(1,-1,1),
                                    point(1,1,1),  point(1,-1,1),  point(1,1,-1),  point(1,-1,-1),  point(-1,1,-1),  point(-1,-1,-1), point(-1,1,1),  point(-1,-1,1)};

        CoreGraphics::RenderShape::RenderShapeVertex verts[24];
        for (IndexT i = 0; i < 24; i++) verts[i].pos = unitCube[i];
        
        RenderShape shape;
        color.set(0.5f, 0.5f, 0.5f, 0.9f);
        shape.SetupPrimitives(Threading::Thread::GetMyThreadId(),
                            frustum,
                            PrimitiveTopology::LineList,
                            12,
                            verts,
                            color,
                            CoreGraphics::RenderShape::CheckDepth);
        ShapeRenderer::Instance()->AddShape(shape); 
    }
}

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44&
AbstractLightEntity::GetShadowTransform()
{
	if (this->shadowTransformsDirty)
	{
		this->UpdateShadowTransforms();
	}
	return this->shadowTransform;
}

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44& 
AbstractLightEntity::GetShadowInvTransform()
{
    if (this->shadowTransformsDirty)
    {
        this->UpdateShadowTransforms();
    }
    return this->shadowInvTransform;
}

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44& 
AbstractLightEntity::GetShadowProjTransform()
{
    if (this->shadowTransformsDirty)
    {
        this->UpdateShadowTransforms();
    }
    return this->shadowProjTransform;
}   

//------------------------------------------------------------------------------
/**
*/
const Math::matrix44& 
AbstractLightEntity::GetShadowInvLightProjTransform()
{       
    if (this->shadowTransformsDirty)
    {
        this->UpdateShadowTransforms();
    }
    return this->shadowInvLightProjTransform;
}

//------------------------------------------------------------------------------
/**
*/
void
AbstractLightEntity::UpdateFrame()
{
	this->shadowCastingFrame++;
	this->castShadowsThisFrame = ((this->shadowCastingFrame % this->shadowCastingFrequency) == 0) && this->castShadows;
}

//------------------------------------------------------------------------------
/**
*/
void 
AbstractLightEntity::UpdateShadowTransforms()
{
    this->shadowInvTransform = matrix44::inverse(this->shadowTransform);
    this->shadowInvLightProjTransform = matrix44::multiply(this->shadowInvTransform, this->projTransform);

    // @todo set extra optimal near and far plane
    this->shadowProjTransform = this->GetProjTransform();
    this->shadowTransformsDirty = false;
}

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
AbstractLightEntity::HandleMessage(const Ptr<Message>& msg)
{
    __Dispatch(AbstractLightEntity, this, msg);
}



} // namespace Graphics
