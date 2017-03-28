//------------------------------------------------------------------------------
//  game/graphicsfeature.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphicsfeature/graphicsfeatureproperties.h"
#include "basegamefeature/loader/loaderserver.h"
#include "visibility/visibilitysystems/visibilityquadtree.h"
#include "game/gameserver.h"
#include "debugrender/debugrenderprotocol.h"
#include "threading/thread.h"
#include "input/keyboard.h"
#include "basegametiming/gametimesource.h"   
#include "resources/simpleresourcemapper.h"
#include "graphics/graphicsinterface.h"
#include "framesync/framesynctimer.h"
#include "coregraphics/streamtextureloader.h"
#include "coregraphics/streammeshloader.h"
#include "coreanimation/streamanimationloader.h"
#include "coreanimation/animresource.h"
#include "coreanimation/managedanimresource.h"
#include "basegamefeatureunit.h"
#include "db/reader.h"
#include "db/dbserver.h"
#include "coregraphics/displaydevice.h"
#include "animpath/pathanimation.h"
#include "animpath/streampathanimationloader.h"
#include "animpath/managedpathanimation.h"
#include "graphicsfeature/properties/animpathproperty.h"
#include "graphicsfeature/properties/lightprobeproperty.h"
#include "managers/levelattrsmanager.h"
#include "visibility/visibilityprotocol.h"
#include "loader/environmentloader.h"
#include "managers/enventitymanager.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeatureUnit, 'FGFX' , Game::FeatureUnit);
__ImplementSingleton(GraphicsFeatureUnit);

using namespace CoreGraphics;
using namespace Graphics;
using namespace Input;
using namespace Resources;
using namespace Threading;
using namespace Animation;
using namespace Util;
using namespace Visibility;

//------------------------------------------------------------------------------
/**
*/
GraphicsFeatureUnit::GraphicsFeatureUnit() : 	
    resizable(true),
    decorated(true),
	defaultGraphicsWorld(true),
	viewClass(View::RTTI),
	frameShader(ResourceId(NEBULA3_DEFAULT_FRAMESHADER_NAME))
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GraphicsFeatureUnit::~GraphicsFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnActivate()
{
	n_assert(!this->IsActive());

	// create post effect manager, setup world, then attach manager
	if (this->defaultGraphicsWorld) this->SetupDefaultGraphicsWorld();

	// setup anim path mapper
	this->animPathMapper = SimpleResourceMapper::Create();
	this->animPathMapper->SetResourceClass(PathAnimation::RTTI);
	this->animPathMapper->SetResourceLoaderClass(StreamPathAnimationLoader::RTTI);
	this->animPathMapper->SetManagedResourceClass(ManagedPathAnimation::RTTI);
	ResourceManager::Instance()->AttachMapper(this->animPathMapper.upcast<Resources::ResourceMapper>());

	this->envEntityManager = EnvEntityManager::Create();
	this->AttachManager(this->envEntityManager.upcast<Game::Manager>());

	// attach loader to BaseGameFeature::LoaderServer
	Ptr<GraphicsFeature::EnvironmentLoader> environmentloader = GraphicsFeature::EnvironmentLoader::Create();
	BaseGameFeature::LoaderServer::Instance()->AttachEntityLoader(environmentloader.upcast<BaseGameFeature::EntityLoaderBase>());

	FeatureUnit::OnActivate();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnDeactivate()
{
	n_assert(this->IsActive());
	
	this->RemoveManager(this->envEntityManager.upcast<Game::Manager>());
	this->envEntityManager = 0;

	// destroy post effect and world
	this->DiscardDisplay();
    FeatureUnit::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::SetupDefaultGraphicsWorld()
{
    n_assert(!this->defaultStage.isvalid());
    n_assert(!this->defaultView.isvalid());
    n_assert(!this->defaultCamera.isvalid());

    ResourceId defaultStageName("DefaultStage");
    ResourceId defaultViewName("DefaultView");
       
    // attach visibility systems to checker
    Ptr<Visibility::VisibilityQuadtree> visSystem = Visibility::VisibilityQuadtree::Create();
	visSystem->SetQuadTreeSettings(4, worldBoundingBox);
    Util::Array<Ptr<VisibilitySystemBase> > visSystems;
    visSystems.Append(visSystem.cast<VisibilitySystemBase>());
    this->defaultStage = this->graphicsServer->CreateStage(defaultStageName, visSystems);
        
    this->defaultView = this->graphicsServer->CreateView(this->viewClass,
                                                         defaultViewName,
														 0,
                                                         true);

    Ptr<Frame2::FrameScript> frameShader = Frame2::FrameServer::Instance()->LoadFrameScript("main", "frame:vkdebug.json");
    this->defaultView->SetStage(this->defaultStage);
    this->defaultView->SetFrameScript(frameShader);

    // setup a default camera
    this->defaultCamera = CameraEntity::Create();
    this->defaultStage->AttachEntity(this->defaultCamera.cast<GraphicsEntity>());
    this->defaultView->SetCameraEntity(this->defaultCamera);
	
	// create global light
	this->globalLight = GlobalLightEntity::Create();
    this->globalLight->SetVolumetric(true);
	this->defaultStage->AttachEntity(this->globalLight.cast<GraphicsEntity>());	
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::DiscardDefaultGraphicsWorld()
{
	// cleanup environment entity
	this->envEntityManager->ClearEnvEntity();

	n_assert(this->IsActive());
    n_assert(this->defaultStage.isvalid());
    n_assert(this->defaultView.isvalid());
    n_assert(this->defaultCamera.isvalid());    
	
    this->defaultStage->RemoveEntity(this->defaultCamera.cast<GraphicsEntity>());
    this->defaultCamera = 0;
	this->defaultStage->RemoveEntity(this->globalLight.cast<GraphicsEntity>());
	this->globalLight = 0;				
    this->graphicsServer->DiscardAllStages();
    this->graphicsServer->DiscardAllViews();
    this->defaultView = 0;
    this->defaultStage = 0;    
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnFrame()
{    
	this->debugShapeRenderer->OnFrame();
    this->debugTextRenderer->OnFrame();

	// draw frame
    GraphicsInterface::Instance()->OnFrame();
    
    Game::FeatureUnit::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnEndFrame()
{
    // in windowed mode, give other apps time slice
    if (!this->display->Settings().IsFullscreen())
    {
        Core::SysFunc::Sleep(0.0);
    }

    Game::FeatureUnit::OnEndFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::StartRenderDebug()
{   
    Ptr<Debug::RenderDebugView> renderDebugMsg = Debug::RenderDebugView::Create();
    renderDebugMsg->SetThreadId(Thread::GetMyThreadId());
    renderDebugMsg->SetEnableDebugRendering(true);
    Graphics::GraphicsInterface::Instance()->Send(renderDebugMsg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::StopRenderDebug()
{   
    Ptr<Debug::RenderDebugView> renderDebugMsg = Debug::RenderDebugView::Create();
    renderDebugMsg->SetThreadId(Thread::GetMyThreadId());
    renderDebugMsg->SetEnableDebugRendering(false);    
    Graphics::GraphicsInterface::Instance()->Send(renderDebugMsg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnConfigureDisplay()
{
    // display adapter
    Adapter::Code adapter = Adapter::Primary;
    if (this->args.HasArg("-adapter"))
    {
        adapter = Adapter::FromString(this->args.GetString("-adapter"));
        if (this->display->AdapterExists(adapter))
        {
            this->display->Settings().SetAdapter(adapter);
        }
    }

    // set parent window data in display
    this->display->SetWindowData(this->windowData);

    // display mode
    CoreGraphics::DisplayMode displayMode;
    if (this->args.HasArg("-x"))
    {
        displayMode.SetXPos(this->args.GetInt("-x"));
    }
    if (this->args.HasArg("-y"))
    {
        displayMode.SetYPos(this->args.GetInt("-y"));
    }
    if (this->args.HasArg("-w"))
    {
        displayMode.SetWidth(this->args.GetInt("-w"));
    }
    if (this->args.HasArg("-h"))
    {
        displayMode.SetHeight(this->args.GetInt("-h"));
    }
    if (this->args.HasArg("-ratio"))
    {
        displayMode.SetAspectRatio(this->args.GetFloat("-ratio"));
    }
    this->display->Settings().DisplayMode() = displayMode;
    this->display->Settings().SetFullscreen(this->args.GetBoolFlag("-fullscreen"));
    this->display->Settings().SetAlwaysOnTop(this->args.GetBoolFlag("-alwaysontop"));
    this->display->Settings().SetVerticalSyncEnabled(this->args.GetBoolFlag("-vsync"));
	this->display->Settings().SetEmbedded(this->args.GetBoolFlag("-embedded"));
    this->display->Settings().SetResizable(this->resizable);
    this->display->Settings().SetDecorated(this->decorated);
    
    if (this->args.HasArg("-aa"))
    {
        this->display->Settings().SetAntiAliasQuality(AntiAliasQuality::FromString(this->args.GetString("-aa")));
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsFeatureUnit::OnSetupResourceMappers()
{
    Array<Ptr<ResourceMapper> > resourceMappers;

    // setup resource mapper for textures
    Ptr<SimpleResourceMapper> texMapper = SimpleResourceMapper::Create();
    texMapper->SetPlaceholderResourceId(ResourceId(NEBULA3_PLACEHOLDER_TEXTURENAME));
    texMapper->SetResourceClass(Texture::RTTI);
    texMapper->SetResourceLoaderClass(CoreGraphics::StreamTextureLoader::RTTI);
    texMapper->SetManagedResourceClass(ManagedTexture::RTTI);
    resourceMappers.Append(texMapper.upcast<ResourceMapper>());

    // setup resource mapper for meshes
    Ptr<SimpleResourceMapper> meshMapper = SimpleResourceMapper::Create();
    meshMapper->SetPlaceholderResourceId(ResourceId(NEBULA3_PLACEHOLDER_MESHNAME));
    meshMapper->SetResourceClass(Mesh::RTTI);
    meshMapper->SetResourceLoaderClass(CoreGraphics::StreamMeshLoader::RTTI);
    meshMapper->SetManagedResourceClass(ManagedMesh::RTTI);
    resourceMappers.Append(meshMapper.upcast<ResourceMapper>());

    // setup resource mapper for animations
    // FIXME: should be configurable!
    Ptr<SimpleResourceMapper> animMapper = SimpleResourceMapper::Create();
    animMapper->SetResourceClass(CoreAnimation::AnimResource::RTTI);
    animMapper->SetResourceLoaderClass(CoreAnimation::StreamAnimationLoader::RTTI);
    animMapper->SetManagedResourceClass(CoreAnimation::ManagedAnimResource::RTTI);
    resourceMappers.Append(animMapper.upcast<ResourceMapper>());

    this->display->SetResourceMappers(resourceMappers);
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsFeatureUnit::ApplyDisplaySettings()
{	
	this->display->Reopen();		
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsFeatureUnit::SetupDisplay()
{
	n_assert(!this->IsActive());
	n_assert(!this->defaultStage.isvalid());
	n_assert(!this->defaultView.isvalid());

	// setup the graphics subsystem
	this->graphicsInterface = GraphicsInterface::Create();
	this->graphicsInterface->Open();

	// setup resource manager
	this->resManager = Resources::ResourceManager::Instance();
	this->display = Display::Create();

	// setup default resource mappers
	this->OnSetupResourceMappers();
	this->OnConfigureDisplay();
	this->display->Open();	

	// setup the frame-sync timer
	this->frameSyncTimer = FrameSync::FrameSyncTimer::Instance();
	this->graphicsServer = GraphicsServer::Instance();

	this->debugShapeRenderer = Debug::DebugShapeRenderer::Create();
	this->debugTextRenderer = Debug::DebugTextRenderer::Create();

	// append standard managers
	this->attachmentManager = GraphicsFeature::AttachmentManager::Create();
	this->AttachManager(this->attachmentManager.cast<Game::Manager>());

	// create default anim event handler
	this->animEventHandler = GameAnimEventHandler::Create();
	Animation::AnimEventServer::Instance()->RegisterAnimEventHandler(this->animEventHandler.cast<AnimEventHandlerBase>());
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsFeatureUnit::DiscardDisplay()
{
	n_assert(this->IsActive());

	Animation::AnimEventServer::Instance()->UnregisterAnimEventHandler(this->animEventHandler.cast<AnimEventHandlerBase>());
	this->animEventHandler = 0;

	if (this->defaultStage.isvalid())
	{
		this->DiscardDefaultGraphicsWorld();    
	}

	n_assert(!this->defaultStage.isvalid());
	n_assert(!this->defaultView.isvalid());
	n_assert(!this->defaultCamera.isvalid());

	this->RemoveManager(this->attachmentManager.upcast<Game::Manager>());
	this->attachmentManager = 0;

	// discard framesync timer
	this->frameSyncTimer = 0;

	// discard resource manager
	this->resManager = 0;
	
	this->display->Close();
	this->display = 0;
	this->graphicsInterface->Close();
	this->graphicsInterface = 0;
	this->graphicsServer = 0;
	this->debugShapeRenderer = 0;
	this->debugTextRenderer = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsFeatureUnit::GetFullscreen() const
{
	return CoreGraphics::DisplayDevice::Instance()->IsFullscreen();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::SetFullscreen(bool enable)
{
	bool fullscreen = this->GetFullscreen();
	if (fullscreen == enable)
	{
		return;
	}
			
	this->display->Settings().SetFullscreen(enable);		
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnLoad()
{
    if (!BaseGameFeature::LevelAttrsManager::HasInstance())
    {
        return;
    }
	// get world extents
	Math::float4 bbCenter = BaseGameFeature::LevelAttrsManager::Instance()->GetFloat4(Attr::WorldCenter);
	Math::float4 bbExtents = BaseGameFeature::LevelAttrsManager::Instance()->GetFloat4(Attr::WorldExtents);
		
	Math::bbox box = Math::bbox(bbCenter, bbExtents);

	Ptr<Visibility::ChangeVisibilityBounds> msg = Visibility::ChangeVisibilityBounds::Create();
	msg->SetWorldBoundingBox(box);
	msg->SetStageName("DefaultStage");
	GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());

	Math::matrix44 trans = BaseGameFeature::LevelAttrsManager::Instance()->GetMatrix44(Attr::GlobalLightTransform);		
	this->GetGlobalLightEntity()->SetTransform(trans);
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnBeforeLoad()
{
	// if no world exist create default one
	if (!this->GetDefaultStage().isvalid())
	{
		this->SetupDefaultGraphicsWorld();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsFeatureUnit::OnBeforeCleanup()
{
	// cleanup graphics world
	this->DiscardDefaultGraphicsWorld();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Graphics::View>
GraphicsFeatureUnit::CreateWindowAndView(const Util::StringAtom& viewName, const Util::StringAtom& frameShader, const Util::Blob& optWindowData)
{
	Ptr<Graphics::SetupWindow> msg = Graphics::SetupWindow::Create();
	msg->SetAdapter(this->display->Settings().GetAdapter());
	msg->SetDisplayMode(this->display->Settings().DisplayMode());
	msg->SetAntiAliasQuality(this->display->Settings().GetAntiAliasQuality());
	msg->SetFullscreen(this->display->Settings().IsFullscreen());
	msg->SetDisplayModeSwitchEnabled(this->display->Settings().IsDisplayModeSwitchEnabled());
	msg->SetTripleBufferingEnabled(this->display->Settings().IsTripleBufferingEnabled());
	msg->SetAlwaysOnTop(this->display->Settings().IsAlwaysOnTop());
	msg->SetIconName(this->display->Settings().GetIconName());
	msg->SetWindowTitle(this->display->Settings().GetWindowTitle());
	msg->SetWindowData(optWindowData);
	msg->SetEmbedded(this->display->Settings().IsEmbedded());
	msg->SetResizable(this->display->Settings().IsResizable());
	msg->SetDecorated(this->display->Settings().IsDecorated());

	// create window
	__StaticSend(GraphicsInterface, msg);

	// create view using window
	Ptr<Graphics::View> view = GraphicsServer::Instance()->CreateView(this->viewClass, viewName, msg->GetWindowId(), false, true);
	Ptr<Frame2::FrameScript> frm = Frame2::FrameServer::Instance()->GetFrameScript(frameShader);
	view->SetFrameScript(frm);

	return view;
}

} // namespace GraphicsFeature
