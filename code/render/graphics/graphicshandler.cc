//------------------------------------------------------------------------------
//  graphicshandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicshandler.h"
#include "graphics/graphicsinterface.h"
#include "coregraphics/shaperenderer.h"
#include "coregraphics/base/shaperendererbase.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "debugrender/debugshaperenderer.h"
#include "coregraphics/debug/displaypagehandler.h"
#include "coregraphics/debug/texturepagehandler.h"
#include "coregraphics/debug/meshpagehandler.h"
#include "coregraphics/debug/shaderpagehandler.h"
#include "graphics/debug/graphicspagehandler.h"
#include "visibility/visibilityprotocol.h"
#include "characters/characterserver.h"
#if __WIN32__                                                
#include "resources/streaming/streamingtexturepagehandler.h"
#include "resources/streaming/poolresourcemapper.h"
#endif
#if __PS3__
#include "coregraphics/ps3/ps3dynamicgeometryserver.h"
#endif
#include "models/managedmodel.h"
#include "models/modelserver.h"
#include "graphics/graphicsprotocol.h"
#include "models/nodes/shapenode.h"
#include "resources/resourcemanager.h"
#include "resources/managedmesh.h"
#include "coregraphics/base/resourcebase.h"
#include "particles/particlesystemnode.h"
#include "framesync/framesynctimer.h"

namespace Graphics
{
__ImplementClass(Graphics::GraphicsHandler, 'GHDL', Messaging::Handler);

using namespace IO;
using namespace CoreGraphics;
using namespace CoreAnimation;
using namespace Resources;
using namespace Math;
using namespace Models;
using namespace Graphics;
using namespace Lighting;
using namespace Frame;
using namespace Interface;
using namespace Util;
using namespace Debug;
using namespace Messaging;
using namespace Animation;
using namespace Timing;
using namespace Characters;
using namespace FrameSync;
using namespace Particles;
using namespace Visibility;
using namespace Materials;
using namespace Instancing;
using namespace Picking;
using namespace Environment;

//------------------------------------------------------------------------------
/**
*/
GraphicsHandler::GraphicsHandler() :
    isGraphicsRuntimeValid(false),
	useMultithreadedRendering(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GraphicsHandler::~GraphicsHandler()
{
}

//------------------------------------------------------------------------------
/**
    Waits for all resources to be loaded.
*/
void
GraphicsHandler::WaitForPendingResources()
{
    // loop until resource manager is done
    while (!ResourceManager::Instance()->WaitForPendingResources(0));
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsHandler::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->isGraphicsRuntimeValid);
    InterfaceHandlerBase::Open();

    // setup frame-synced timer
	this->frameSyncTimer = FrameSyncTimer::Create();
    this->frameSyncTimer->Setup();

	// setup core runtime
	this->ioServer = IO::IoServer::Instance();

#if __NEBULA3_HTTP_FILESYSTEM__
    // setup http client registry for shared HTTP connections
	this->httpClientRegistry = Http::HttpClientRegistry::Instance();
	
#endif

#if __NEBULA3_HTTP__

	// setup http page handlers
	this->httpServerProxy = Http::HttpServerProxy::Instance();

	this->httpServerProxy->AttachRequestHandler(Debug::DisplayPageHandler::Create());
	this->httpServerProxy->AttachRequestHandler(Debug::MeshPageHandler::Create());
	this->httpServerProxy->AttachRequestHandler(Debug::ShaderPageHandler::Create());
	this->httpServerProxy->AttachRequestHandler(Debug::TexturePageHandler::Create());
	this->httpServerProxy->AttachRequestHandler(Debug::GraphicsPageHandler::Create());

	#if __WIN32__
		if (this->useMultithreadedRendering) this->httpServerProxy->AttachRequestHandler(Debug::StreamingTexturePageHandler::Create());
	#endif

#endif

    // setup the required objects, but do not open them, this will
    // happen at a later time when the SetupGrapics message is received
    this->renderDevice = RenderDevice::Create();
    this->displayDevice = DisplayDevice::Create();
    this->transformDevice = TransformDevice::Create();
    this->shaderServer = ShaderServer::Create();
    this->shapeRenderer = ShapeRenderer::Create();
    this->textRenderer = TextRenderer::Create();
    this->vertexLayoutServer = VertexLayoutServer::Create();
    this->modelServer = ModelServer::Create();
    this->graphicsServer = GraphicsServer::Create();
    this->lightServer = LightServer::Create();
    this->shadowServer = ShadowServer::Create();
	this->instanceServer = InstanceServer::Create();
    this->frameServer = FrameServer::Create();
	this->frame2Server = Frame2::FrameServer::Create();
	this->materialServer = MaterialServer::Create();
    this->animEventServer = Animation::AnimEventServer::Create();
    this->characterServer = Characters::CharacterServer::Create();
    this->mouseRenderDevice = MouseRenderDevice::Create();
    this->particleServer = ParticleServer::Create();

	// setup resource manager
	this->resourceManager = ResourceManager::Create();

    // setup debug timers and counters
    _setup_timer(GraphicsFrameTime);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsHandler::Close()
{
    n_assert(this->IsOpen());

    _discard_timer(GraphicsFrameTime);
    
    if (this->isGraphicsRuntimeValid)
    {
        this->ShutdownGraphicsRuntime();
    }

    this->particleServer = 0;
    this->mouseRenderDevice = 0;
    this->characterServer = 0;
    this->frameServer = 0;
	this->frame2Server = 0;
	this->materialServer = 0;
    this->shadowServer = 0;
    this->lightServer = 0;
	this->instanceServer = 0;
    this->graphicsServer = 0;
    this->modelServer = 0;
    this->resourceManager = 0;
    this->vertexLayoutServer = 0;
    this->shapeRenderer = 0;
    this->shaderServer = 0;
    this->transformDevice = 0;
    this->displayDevice = 0;
    this->renderDevice = 0;
    this->textRenderer = 0;
    this->animEventServer = 0;

#if __NEBULA3_HTTP__
	this->httpServerProxy = 0;
#endif
#if __NEBULA3_HTTP_FILESYSTEM__
    this->httpClientRegistry = 0;
#endif 

    this->ioServer = 0;
	this->frameSyncTimer = 0;

    InterfaceHandlerBase::Close();
}

//------------------------------------------------------------------------------
/**
    Setup the graphics runtime, this method is called when the
    SetupGraphics message is received from the main thread. The method
    expects that the DisplayDevice has been configured with the 
    desired display settings.
*/
void
GraphicsHandler::SetupGraphicsRuntime(const Ptr<SetupGraphics>& msg)
{
    n_assert(!this->isGraphicsRuntimeValid);
    n_assert(!this->displayDevice->IsOpen());
    n_assert(!this->renderDevice->IsOpen());
    n_assert(!this->transformDevice->IsOpen());
    n_assert(!this->shaderServer->IsOpen());
    n_assert(!this->shapeRenderer->IsOpen());
    n_assert(!this->vertexLayoutServer->IsOpen());
	n_assert(!this->resourceManager->IsOpen());
    n_assert(!this->modelServer->IsOpen());
    n_assert(!this->graphicsServer->IsOpen());
    n_assert(!this->lightServer->IsOpen());
    n_assert(!this->shadowServer->IsOpen());
    n_assert(!this->frameServer->IsOpen());
	n_assert(!this->instanceServer->IsOpen());
	n_assert(!this->materialServer->IsOpen());
    n_assert(!this->characterServer->IsValid());
    
    if (!this->displayDevice->Open())
    {
        n_error("GraphicsHandler: failed to open display device!");
    }
    #if __WII__
    this->displayDevice->GetScreenShotHelper().EnableScreenShotMode();
    #endif
    
    if (!this->renderDevice->Open())
    {
        n_error("GraphicsHandler: failed to open render device!");
    }
    this->vertexLayoutServer->Open();
    this->shaderServer->Open();
    this->transformDevice->Open();
	this->resourceManager->Open();
    this->textRenderer->Open();
    this->characterServer->Setup();
    this->mouseRenderDevice->Setup();

    // setup resource mappers
    IndexT i;
    for (i = 0; i < msg->GetResourceMappers().Size(); i++)
    {
        this->resourceManager->AttachMapper(msg->GetResourceMappers()[i]);
    }

	this->materialServer->Open();
    this->modelServer->Open();
	this->shapeRenderer->Open();
    this->graphicsServer->Open();
    this->lightServer->Open();
    this->shadowServer->Open();    
    this->particleServer->Open();    
	this->frameServer->Open();	
	this->instanceServer->Open();

    // HACK - pin placeholders and system stuff so they will not be automatically
    // removed or replaced on LOD-management
    this->resourceManager->AutoManageManagedResource("tex:system/black.dds", false);
    this->resourceManager->AutoManageManagedResource("tex:system/white.dds", false);
    this->resourceManager->AutoManageManagedResource("tex:system/nobump.dds", false);
    this->resourceManager->AutoManageManagedResource("tex:lighting/lightcones.dds", false);
    this->resourceManager->AutoManageManagedResource("tex:system/placeholder.dds", false);
	this->resourceManager->AutoManageManagedResource("sur:system/placeholder.sur", false);

    this->isGraphicsRuntimeValid = true;
}

//------------------------------------------------------------------------------
/**
    Shutdown the graphics runtime, this is called from the Close() method.
*/
void
GraphicsHandler::ShutdownGraphicsRuntime()
{
    n_assert(this->isGraphicsRuntimeValid);
    this->isGraphicsRuntimeValid = false;

    this->particleServer->Close();      
    this->mouseRenderDevice->Discard();
    this->characterServer->Discard();
    this->textRenderer->Close();
    this->shadowServer->Close();
    this->lightServer->Close();
    this->graphicsServer->Close();
    this->modelServer->Close();
	this->materialServer->Close();
	this->shapeRenderer->Close();
    this->frameServer->Close();	
	this->resourceManager->Close();
	this->instanceServer->Close();
    this->shaderServer->Close();
    this->vertexLayoutServer->Close();
    this->renderDevice->Close();
    this->displayDevice->Close();
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());

	if (msg->IsA(Graphics::GraphicsEntityMessage::RTTI))
	{
		// handle graphics entity message		
        const Ptr<Graphics::GraphicsEntityMessage>& gfxMsg = msg.downcast<Graphics::GraphicsEntityMessage>();
        Ptr<GraphicsEntity> entity = gfxMsg->GetEntity();
        entity->HandleMessage(msg);		
		return true;
	}
    else if (msg->CheckId(SetupGraphics::Id))
    {
        this->OnSetupGraphics(msg.cast<SetupGraphics>());
        return true;
    }
	else if (msg->CheckId(UpdateDisplay::Id))
	{
		this->OnUpdateDisplay(msg.cast<UpdateDisplay>());
		return true;
	}

    __StaticHandle(CreateModelEntity);
    __StaticHandle(CreateCameraEntity);
    __StaticHandle(CreateGlobalLightEntity);
    __StaticHandle(CreatePointLightEntity);
    __StaticHandle(CreateSpotLightEntity);
	__StaticHandle(CreateBillboardEntity);
    __StaticHandle(AdapterExists);
    __StaticHandle(GetAvailableDisplayModes);
    __StaticHandle(GetCurrentAdapterDisplayMode);
    __StaticHandle(GetAdapterInfo);
    __StaticHandle(AttachDisplayEventHandler);
    __StaticHandle(RemoveDisplayEventHandler);
    __StaticHandle(AttachRenderEventHandler);
    __StaticHandle(RemoveRenderEventHandler);
    __StaticHandle(CreateGraphicsStage);
    __StaticHandle(DiscardGraphicsStage);
	__StaticHandle(EnableResourcesAsync);
	__StaticHandle(ReloadResource);
	__StaticHandle(ReloadResourceIfExists);
	__StaticHandle(EnableWireframe);
	__StaticHandle(ItemAtPosition);
	__StaticHandle(ItemsAtPosition);
	__StaticHandle(DepthAtPosition);
	__StaticHandle(NormalAtPosition);
	__StaticHandle(SetModelNodeTexture);
	__StaticHandle(SetModelNodeVariable);
    __StaticHandle(CreateGraphicsView);
    __StaticHandle(DiscardGraphicsView);
    __StaticHandle(SetDefaultGraphicsView);
    __StaticHandle(UpdateSharedShaderVariable);
    __StaticHandle(RegisterAnimEventHandler);
    __StaticHandle(UnregisterAnimEventHandler);
    __StaticHandle(PreloadMousePointerTextures);
    __StaticHandle(UpdateMousePointers);
    __StaticHandle(RegisterRTPlugin);
    __StaticHandle(UnregisterRTPlugin);
    __StaticHandle(HoldSharedResources);
    __StaticHandle(ReleaseSharedResources);
    __StaticHandle(SetTimeFactor);
    __StaticHandle(SetShadowPointOfInterest);
    __StaticHandle(CreateVisibilityCluster);
    __StaticHandle(CreateVisibilityBoxes);
	__StaticHandle(ChangeVisibilityBounds);
	__StaticHandle(GetRenderMaterials);	
	__StaticHandle(ShowSystemCursor);
	
    // unhandled message
    return false;
}

//------------------------------------------------------------------------------
/**
    This is the per-frame method which implements the asynchronous render-loop.
*/
void
GraphicsHandler::DoWork()
{
    if (this->isGraphicsRuntimeValid)
    {
        _start_timer(GraphicsFrameTime);
        n_assert(this->IsOpen());

#if __PS3__
        PS3::PS3DynamicGeometryServer::Instance()->Flip();
#endif

        // debug render
        // n_printf("--> graphics time: %f\n", this->frameSyncTimer->GetTime());

        // FIXME: handle anim events
        this->animEventServer->OnFrame(this->frameSyncTimer->GetScaledTime());
      
		// finally, render the frame
		this->graphicsServer->OnFrame(this->frameSyncTimer->GetScaledTime(), this->frameSyncTimer->GetTimeFactor());
       
        _stop_timer(GraphicsFrameTime);
    }

#if __NEBULA3_HTTP__
    // allow render-thread HttpRequests to be processed
    this->httpServerProxy->HandlePendingRequests();
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsHandler::OnSetupGraphics(const Ptr<SetupGraphics>& msg)
{
    n_assert(!this->isGraphicsRuntimeValid);

    // configure the display device and setup the graphics runtime
    DisplayDevice* disp = this->displayDevice;
    disp->SetAdapter(msg->GetAdapter());
    disp->SetDisplayMode(msg->GetDisplayMode());
    disp->SetAntiAliasQuality(msg->GetAntiAliasQuality());
    disp->SetFullscreen(msg->GetFullscreen());
    disp->SetDisplayModeSwitchEnabled(msg->GetDisplayModeSwitchEnabled());
    disp->SetTripleBufferingEnabled(msg->GetTripleBufferingEnabled());
    disp->SetAlwaysOnTop(msg->GetAlwaysOnTop());
    disp->SetVerticalSyncEnabled(msg->GetVerticalSyncEnabled());
    disp->SetIconName(msg->GetIconName());
    disp->SetWindowTitle(msg->GetWindowTitle());
    disp->SetWindowData(msg->GetWindowData());
	disp->SetEmbedded(msg->GetEmbedded());
    disp->SetDecorated(msg->GetDecorated());
    disp->SetResizable(msg->GetResizable());
    this->SetupGraphicsRuntime(msg);

    msg->SetActualDisplayMode(disp->GetDisplayMode());
    msg->SetActualAdapter(disp->GetAdapter());
    msg->SetActualFullscreen(disp->IsFullscreen());
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsHandler::OnUpdateDisplay( const Ptr<Graphics::UpdateDisplay>& msg )
{
	n_assert(this->isGraphicsRuntimeValid);

	// configure the display device and setup the graphics runtime
	DisplayDevice* disp = this->displayDevice;

	disp->SetDisplayMode(msg->GetDisplayMode());
	disp->SetAntiAliasQuality(msg->GetAntiAliasQuality());
	disp->SetFullscreen(msg->GetFullscreen());
    if(msg->GetWindowData().Size() > 0)
    {
	    disp->SetWindowData(msg->GetWindowData());
    }
	disp->SetTripleBufferingEnabled(msg->GetTripleBufferingEnabled());

	// reopen display
    disp->DisableCallbacks();
	disp->Reopen();
    disp->EnableCallbacks();
}

} // namespace Graphics

