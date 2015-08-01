//------------------------------------------------------------------------------
//  view.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/view.h"
#include "graphics/modelentity.h"
#include "models/visresolver.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/shaperenderer.h"
#include "models/modelserver.h"
#include "lighting/lightserver.h"
#include "lighting/shadowserver.h"
#include "particles/particlerenderer.h"
#include "picking/pickingserver.h"
#include "environment/environmentserver.h"
#include "graphicsserver.h"
#include "coregraphics/renderdevice.h"

namespace Graphics
{
__ImplementClass(Graphics::View, 'IGVW', Core::RefCounted);

using namespace Util;
using namespace Models;
using namespace CoreGraphics;
using namespace Lighting;
using namespace Characters;
using namespace Math;
using namespace Picking;
using namespace Environment;

//------------------------------------------------------------------------------
/**
*/
View::View() :
    isAttachedToServer(false),
	resolveRectValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
View::~View()
{
    // check that we've been properly cleaned up
    n_assert(!this->isAttachedToServer);
    n_assert(!this->stage.isvalid());
    n_assert(!this->camera.isvalid());
    n_assert(!this->frameShader.isvalid());
    n_assert(this->dependencies.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
void
View::OnAttachToServer()
{
    n_assert(!this->IsAttachedToServer());
    this->isAttachedToServer = true;

#if NEBULA3_ENABLE_PROFILING
	this->resolveVisibleShadowCasters = Debug::DebugTimer::Create();
	String name("View_" + this->GetName().AsString() + "_ResolveShadowCasters");
	this->resolveVisibleShadowCasters->Setup(name);

    this->resolveVisibleModelNodeInstances = Debug::DebugTimer::Create();
	name.Format("View_%s_ResolveModelInstanceNodes", this->name.Value());
    this->resolveVisibleModelNodeInstances->Setup(name);

	this->updateShadowBuffers = Debug::DebugTimer::Create();
	name.Format("View_%s_UpdateShadowBuffers", this->name.Value());
	this->updateShadowBuffers->Setup(name);

	this->picking = Debug::DebugTimer::Create();
	name.Format("View_%s_UpdatePicking", this->name.Value());
	this->picking->Setup(name);

	this->render = Debug::DebugTimer::Create();
	name.Format("View_%s_RenderFrameShader", this->name.Value());
	this->render->Setup(name);
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
View::OnRemoveFromServer()
{
    n_assert(this->IsAttachedToServer());
    if (this->camera.isvalid())
    {
        this->camera->OnRemoveFromView(this);
        this->camera = 0;
    }
    this->stage = 0;
    this->frameShader = 0;
    this->dependencies.Clear();
    this->isAttachedToServer = false;

	_discard_timer(resolveVisibleShadowCasters);
    _discard_timer(resolveVisibleModelNodeInstances);
	_discard_timer(updateShadowBuffers);
	_discard_timer(picking);
	_discard_timer(render);
}

//------------------------------------------------------------------------------
/**
    This method updates the visibility links for the view's camera.
    This method should usually be called before Render().
*/
void
View::UpdateVisibilityLinks()
{
    n_assert(this->camera.isvalid());
    this->stage->UpdateCameraLinks(this->camera);
}

//------------------------------------------------------------------------------
/**
    This attaches visible lights to the light and shadow server.
    @todo: currently this methods needs to go over all visible
    graphics entities to find the lights... 
*/
void
View::ResolveVisibleLights(IndexT frameIndex)
{
    n_assert(this->camera.isvalid());
    LightServer* lightServer = LightServer::Instance();
    ShadowServer* shadowServer = ShadowServer::Instance();

    lightServer->BeginAttachVisibleLights();
    shadowServer->BeginAttachVisibleLights();
    const Array<Ptr<GraphicsEntity> >& visLinks = this->camera->GetLinks(GraphicsEntity::CameraLink);
    IndexT i;
    SizeT num = visLinks.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<GraphicsEntity>& curEntity = visLinks[i];
        if (GraphicsEntityType::Light == curEntity->GetType())
        {
			// notify this light source is visible
			curEntity->OnResolveVisibility(frameIndex);

			// cast to light source
			const Ptr<AbstractLightEntity>& lightEntity = curEntity.downcast<AbstractLightEntity>();

			// also resolve visible models seen by this light, if this light casts shadows
			if (lightEntity->GetCastShadowsThisFrame())
			{
				const Array<Ptr<GraphicsEntity>>& shadowLinks = lightEntity->GetLinks(GraphicsEntity::LightLink);
				SizeT numShadowLinks = shadowLinks.Size();
				IndexT j;
				for (j = 0; j < numShadowLinks; j++)
				{
					const Ptr<GraphicsEntity>& curEntity = shadowLinks[j];
					n_assert(GraphicsEntityType::Model == curEntity->GetType());
					curEntity->OnRenderBefore(frameIndex);
				}
			}
        }
		else if (GraphicsEntityType::LightProbe == curEntity->GetType())
		{
			curEntity->OnResolveVisibility(frameIndex);
		}
    }    
    shadowServer->EndAttachVisibleLights();
    lightServer->EndAttachVisibleLights();
}

//------------------------------------------------------------------------------
/**
    Resolve all visible ModelNodeInstances by following the visibility
    links of our camera. This is necessary as preparation for rendering.
    This is also where the OnRenderBefore() callback will be invoked on
    entities.
*/
void
View::ResolveVisibleModelNodeInstances(IndexT frameIndex)
{
    n_assert(this->camera.isvalid());
    const matrix44& cameraTransform = this->camera->GetTransform();
    VisResolver* visResolver = VisResolver::Instance();
    visResolver->BeginResolve(cameraTransform);
    const Array<Ptr<GraphicsEntity> >& visLinks = this->camera->GetLinks(GraphicsEntity::CameraLink);
    IndexT i;
    SizeT num = visLinks.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<GraphicsEntity>& curEntity = visLinks[i];
        if (GraphicsEntityType::Model == curEntity->GetType())
        {
            curEntity->OnRenderBefore(frameIndex);
            curEntity->OnResolveVisibility(frameIndex, true);
        }
    }
    visResolver->EndResolve();
}

//------------------------------------------------------------------------------
/**
	Resolves entities seen by the global light
*/
void 
View::ResolveVisibleShadowCasters( IndexT frameIndex )
{
	// resolve visibility for the global light
	const Ptr<GlobalLightEntity>& globalLight = GraphicsServer::Instance()->GetCurrentGlobalLightEntity();
	if (globalLight.isvalid() && globalLight->GetCastShadowsThisFrame())
	{
		const Array<Ptr<GraphicsEntity> >& visLinks = globalLight->GetLinks(GraphicsEntity::LightLink);
		IndexT i;
		SizeT num = visLinks.Size();
		for (i = 0; i < num; i++)
		{
			const Ptr<GraphicsEntity>& curEntity = visLinks[i];
			n_assert(GraphicsEntityType::Model == curEntity->GetType());
			curEntity->OnRenderBefore(frameIndex);
		}
	}
}

//------------------------------------------------------------------------------
/**
    This method sets the new camera transforms valid for this frame in the transformdevice
*/
void
View::ApplyCameraSettings()
{
    TransformDevice* transformDevice = TransformDevice::Instance();
    transformDevice->SetProjTransform(this->camera->GetProjTransform());
    transformDevice->SetViewTransform(this->camera->GetViewTransform());  
    transformDevice->SetFocalLength(this->camera->GetCameraSettings().GetFocalLength());
    transformDevice->ApplyViewSettings();
}

//------------------------------------------------------------------------------
/**
    This method renders the current view into the render target. This method
    must be called sometimes after UpdateVisibilityLinks()
*/
void
View::Render(IndexT frameIndex)
{
    n_assert(this->frameShader.isvalid());      
    n_assert(this->camera.isvalid());

    // reset model node instance index for a new render frame (view)
    ModelServer::Instance()->ResetModelNodeInstanceIndex();
	RenderDevice* renderDev = RenderDevice::Instance();
    LightServer* lightServer = LightServer::Instance();
    ShadowServer* shadowServer = ShadowServer::Instance();
	PickingServer* pickingServer = PickingServer::Instance();

    lightServer->BeginFrame(this->camera);
    shadowServer->BeginFrame(this->camera);
	pickingServer->BeginFrame(this->camera);
	this->frameShader->Begin();

	Particles::ParticleRenderer::Instance()->BeginAttach();

	_start_timer(resolveVisibleShadowCasters);
	// resolve visible light source
	this->ResolveVisibleLights(frameIndex);

	// resolve visible shadow casters for global light
	//this->ResolveVisibleShadowCasters(frameIndex);
	_stop_timer(resolveVisibleShadowCasters);

	_start_timer(updateShadowBuffers);
	// update local shadows
	shadowServer->UpdateShadowBuffers();
	_stop_timer(updateShadowBuffers);

	_start_timer(resolveVisibleModelNodeInstances);
	// resolve visible ModelNodeInstances
	this->ResolveVisibleModelNodeInstances(frameIndex);
	_stop_timer(resolveVisibleModelNodeInstances);

    Particles::ParticleRenderer::Instance()->EndAttach();

	// if we have a resolve rect, we set the resolve rectangle 
	if (this->resolveRectValid)
	{
		renderDev->GetDefaultRenderTarget()->SetResolveRect(this->resolveRect);
	}

    // render the world...
	_start_timer(render);
    this->frameShader->Render(frameIndex);
	_stop_timer(render);

	// render picking
	_start_timer(picking);
	pickingServer->Render(frameIndex);
	_stop_timer(picking);

    // tell main frame shader, light and shadow servers that rendering is finished
	this->frameShader->End();
	pickingServer->EndFrame();
    shadowServer->EndFrame();
    lightServer->EndFrame();	
}

//------------------------------------------------------------------------------
/**
    Renders a debug visualization. Can be called alone or after Render().
*/
void
View::RenderDebug()
{
    n_assert(this->frameShader.isvalid());

    // setup global transforms...
    TransformDevice* transformDevice = TransformDevice::Instance();
    transformDevice->SetProjTransform(this->camera->GetProjTransform());
    transformDevice->SetViewTransform(this->camera->GetViewTransform());

    // just call OnRenderDebug on all graphics entities visible through our camera
    const Array<Ptr<GraphicsEntity> >& visLinks = this->camera->GetLinks(GraphicsEntity::CameraLink);
    IndexT i;
    SizeT num = visLinks.Size();
    for (i = 0; i < num; i++)
    {
        const Ptr<GraphicsEntity>& curEntity = visLinks[i];
        curEntity->OnRenderDebug();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
View::SetCameraEntity(const Ptr<CameraEntity>& newCameraEntity)
{
    // assert if cam is not attached to a stage
    if (this->camera.isvalid())
    {
        this->camera->OnRemoveFromView(this);
        this->camera = 0;
    }
    if (newCameraEntity.isvalid())
    {
		n_assert2(newCameraEntity->IsAttachedToStage(), "Camera has to be attached to a stage first!");
        this->camera = newCameraEntity;
        this->camera->OnAttachToView(this);
    }
}

} // namespace Graphics
