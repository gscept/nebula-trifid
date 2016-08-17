//------------------------------------------------------------------------------
//  graphicsserver.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "resources/resourcemanager.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/shaperenderer.h"
#include "graphics/globallightentity.h"
#include "coregraphics/shadersemantics.h"
#include "lighting/lightserver.h"
#include "characters/characterserver.h"
#include "framesync/framesynctimer.h"

namespace Graphics
{
__ImplementClass(Graphics::GraphicsServer, 'IGFS', Core::RefCounted);
__ImplementSingleton(Graphics::GraphicsServer);

using namespace Core;
using namespace Util;
using namespace Math;
using namespace Resources;
using namespace CoreGraphics;
using namespace Characters;
using namespace Lighting;
using namespace RenderModules;
using namespace FrameSync;

//------------------------------------------------------------------------------
/**
*/
GraphicsServer::GraphicsServer() :
    isOpen(false),
    renderDebug(false),
    currentView(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GraphicsServer::~GraphicsServer()
{
    n_assert(!this->isOpen);
    n_assert(this->stages.IsEmpty());
    n_assert(this->stageIndexMap.IsEmpty());
    n_assert(this->views.IsEmpty());
    n_assert(this->viewIndexMap.IsEmpty());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;

    // get time shader variable from shader server (only if platform supports it)
    ShaderServer* shdServer = ShaderServer::Instance();
    ShaderVariable::Name timeSem = ShaderVariable::Name(NEBULA3_SEMANTIC_TIME);
    const Ptr<ShaderState>& sharedShader = shdServer->GetSharedShader();
    if (sharedShader->HasVariableByName(timeSem))
    {        
        this->timeShaderVar = sharedShader->GetVariableByName(timeSem);
        n_assert(this->timeShaderVar.isvalid());
    }

    // setup the render-thread-plugin registry
    this->rtPluginRegistry = RTPluginRegistry::Create();
    this->rtPluginRegistry->Setup();

    // setup profiling timers
	_setup_timer(GfxServerPresentFrame);
	_setup_timer(GfxServerRenderViews);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::Close()
{
    n_assert(this->isOpen);
    n_assert(this->entities.IsEmpty());
    this->timeShaderVar = 0;
    this->DiscardAllViews();
    this->DiscardAllStages();
    this->rtPluginRegistry->Discard();
    this->rtPluginRegistry = 0;
    this->isOpen = false;

	_discard_timer(GfxServerPresentFrame);
	_discard_timer(GfxServerRenderViews);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Stage> 
GraphicsServer::CreateStage(const Util::StringAtom& stageName, const Util::Array<Ptr<Visibility::VisibilitySystemBase> >& visSystems)
{
    n_assert(this->isOpen);
    n_assert(!this->stageIndexMap.Contains(stageName));

    Ptr<Stage> newStage = Stage::Create();
    newStage->SetName(stageName);
    // attach visibility systems 
    newStage->AttachVisibilitySystems(visSystems);
    newStage->OnAttachToServer();

    this->stages.Append(newStage);
    this->stageIndexMap.Add(stageName, this->stages.Size() - 1);

    // notify render-thread plugins
    this->rtPluginRegistry->OnStageCreated(newStage);

    return newStage;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::DiscardStage(const Ptr<Stage>& stage)
{
    n_assert(this->stageIndexMap.Contains(stage->GetName()));

    // notify render-thread plugins
    this->rtPluginRegistry->OnDiscardStage(stage);

    this->stages.EraseIndex(this->stageIndexMap[stage->GetName()]);

    // need to rebuild stageIndexMap
    this->stageIndexMap.Clear();
    IndexT i;
    for (i = 0; i < this->stages.Size(); i++)
    {
        this->stageIndexMap.Add(this->stages[i]->GetName(), i);
    }
    stage->OnRemoveFromServer();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::DiscardAllStages()
{
    IndexT i;
    SizeT num = this->stages.Size();
    for (i = 0; i < num; i++)
    {
        this->rtPluginRegistry->OnDiscardStage(this->stages[i]);
        this->stages[i]->OnRemoveFromServer();
    }
    this->stages.Clear();
    this->stageIndexMap.Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsServer::HasStage(const StringAtom& stageName) const
{
    return this->stageIndexMap.Contains(stageName);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Stage>&
GraphicsServer::GetStageByName(const StringAtom& stageName) const
{
    return this->stages[this->stageIndexMap[stageName]];
}

//------------------------------------------------------------------------------
/**
*/
const Array<Ptr<Stage> >&
GraphicsServer::GetStages() const
{
    return this->stages;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<View>
GraphicsServer::CreateView(const Core::Rtti& viewClass, const StringAtom& viewName, bool isDefaultView, bool updatePerFrame)
{
    n_assert(!this->viewIndexMap.Contains(viewName));
    n_assert(viewClass.IsDerivedFrom(View::RTTI));

    Ptr<View> newView = (View*) viewClass.Create();
    newView->SetName(viewName);
	newView->SetUpdatePerFrame(updatePerFrame);
    newView->OnAttachToServer();

    this->views.Append(newView);
    this->viewIndexMap.Add(viewName, this->views.Size() - 1);
    if (isDefaultView)
    {
        this->SetDefaultView(newView);
    }

    // notify render-thread plugins
    this->rtPluginRegistry->OnViewCreated(newView);

    return newView;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::DiscardView(const Ptr<View>& view)
{
    n_assert(this->viewIndexMap.Contains(view->GetName()));
    this->rtPluginRegistry->OnDiscardView(view);
    this->views.EraseIndex(this->viewIndexMap[view->GetName()]);

    // need to rebuild viewIndexMap
    this->viewIndexMap.Clear();
    IndexT i;
    for (i = 0; i < this->views.Size(); i++)
    {
        this->viewIndexMap.Add(this->views[i]->GetName(), i);
    }

    // check if this was the default view
    if (view == this->defaultView)
    {
        this->defaultView = 0;
    }
    view->OnRemoveFromServer();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::DiscardAllViews()
{
    IndexT i;
    SizeT num = this->views.Size();
    for (i = 0; i < num; i++)
    {
        this->rtPluginRegistry->OnDiscardView(this->views[i]);
        this->views[i]->OnRemoveFromServer();
    }
    this->views.Clear();
    this->viewIndexMap.Clear();
    this->defaultView = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsServer::HasView(const StringAtom& viewName) const
{
    return this->viewIndexMap.Contains(viewName);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<View>&
GraphicsServer::GetViewByName(const StringAtom& viewName) const
{
    return this->views[this->viewIndexMap[viewName]];
}

//------------------------------------------------------------------------------
/**
*/
const Array<Ptr<View> >&
GraphicsServer::GetViews() const
{
    return this->views;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::SetDefaultView(const Ptr<View>& defView)
{
    this->defaultView = defView;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<View>&
GraphicsServer::GetDefaultView() const
{
    return this->defaultView;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::RegisterEntity(const Ptr<GraphicsEntity>& entity)
{
    this->entities.Append(entity);
    this->entityIndexMap.Add(entity->GetId(), this->entities.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::UnregisterEntity(const Ptr<GraphicsEntity>& entity)
{
    IndexT mapIndex = this->entityIndexMap.FindIndex(entity->GetId());
    n_assert(InvalidIndex != mapIndex);

    IndexT entityIndex = this->entityIndexMap.ValueAtIndex(mapIndex);
    this->entities.EraseIndexSwap(entityIndex);
    this->entityIndexMap.EraseAtIndex(mapIndex);

    // need to fixup the lookup index of the entity which has been
    // swapped into place of the erased entity, index doesn't
    // need to fixed if the erased element was the last in the array of course
    if (entityIndex < this->entityIndexMap.Size())
    {
        this->entityIndexMap[this->entities[entityIndex]->GetId()] = entityIndex;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsServer::HasEntity(GraphicsEntity::Id id) const
{
    return this->entityIndexMap.Contains(id);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<GraphicsEntity>&
GraphicsServer::GetEntityById(GraphicsEntity::Id id) const
{
    const Ptr<GraphicsEntity>& entity = this->entities[this->entityIndexMap[id]];
    // if this assertion triggers, this means, that UnregisterEntity() failed
    // to fix indices in the entityIndexMap!!!
    n_assert(entity->GetId() == id);
    return entity;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::RegisterRenderModule(const Ptr<RenderModule>& renderModule)
{
    n_assert(InvalidIndex == this->renderModules.FindIndex(renderModule));
    this->renderModules.Append(renderModule);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::UnregisterRenderModule(const Ptr<RenderModule>& renderModule)
{
    IndexT renderModuleIndex = this->renderModules.FindIndex(renderModule);
    n_assert(InvalidIndex != renderModuleIndex);
    this->renderModules.EraseIndex(renderModuleIndex);
}

//------------------------------------------------------------------------------
/**
    FIXME: iterating stuff is terrible!
*/
Ptr<Graphics::GlobalLightEntity>
GraphicsServer::GetCurrentGlobalLightEntity() const
{   
    Ptr<Graphics::GlobalLightEntity> globLight = 0;
    const Ptr<View>& view = this->GetDefaultView();
    if(view.isvalid())
    {
        const Ptr<Stage>& stage = view->GetStage();
        if(stage.isvalid())
        {
            const Util::Array<Ptr<Graphics::GraphicsEntity> >& lights = stage->GetEntitiesByType(GraphicsEntityType::Light);
            IndexT index;
            for(index = 0; index < lights.Size(); index++)
            {
                if(lights[index]->IsA(Graphics::GlobalLightEntity::RTTI))
                {
                    globLight = lights[index].downcast<Graphics::GlobalLightEntity>();
                    break;
                }
            }
        }
    }    
    return globLight;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsServer::OnFrame(Timing::Time curTime, Timing::Time globalTimeFactor)
{
    RenderDevice* renderDevice = RenderDevice::Instance();
    DisplayDevice* displayDevice = DisplayDevice::Instance();
    IndexT frameIndex = FrameSyncTimer::Instance()->GetFrameIndex();

    // call pre-render update on resource manager
    ResourceManager::Instance()->Prepare(false);

	// call rt plugin registry prior to frame update
	this->rtPluginRegistry->OnFrameBefore(frameIndex, curTime);

    // process window messages
    //displayDevice->ProcessWindowMessages();

    // notify render modules
    IndexT i;
    for (i = 0; i < this->renderModules.Size(); i++)
    {
        this->renderModules[i]->OnFrame();
    }

	// update time
	if (this->timeShaderVar.isvalid())
	{
		this->timeShaderVar->SetFloat((float)curTime);
	}

	// render if we have a view
	if (this->views.Size() > 0)
	{
		_start_timer(GfxServerRenderViews);
		for (i = 0; i < this->views.Size(); i++)
		{
			// get view
			Ptr<View> view = this->views[i];

			// if view is tagged to update per frame update through view
			if (view->GetUpdatePerFrame())
			{
				this->currentView = view;

				// render frame
				view->OnFrame(this->rtPluginRegistry, curTime, globalTimeFactor, this->renderDebug);
			}            
		}
		_stop_timer(GfxServerRenderViews);

        // finally present frame, this should then be the joined result of all views
		_start_timer(GfxServerPresentFrame);
        renderDevice->Present();
		_stop_timer(GfxServerPresentFrame);
	}
    else 
    {
        this->rtPluginRegistry->OnRenderWithoutView(frameIndex, curTime);
    }

	// call rt plugin registry after rendering
	this->rtPluginRegistry->OnFrameAfter(frameIndex, curTime);
    
    // call post-render update on Resource Manager
    ResourceManager::Instance()->Update(frameIndex);

    // if we're running in windowed mode, give up time-slice
    if (!displayDevice->IsFullscreen())
    {
        Timing::Sleep(0.0);
    }
}

} // namespace Graphics
