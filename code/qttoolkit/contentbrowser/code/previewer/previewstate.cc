//------------------------------------------------------------------------------
//  previewstate.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "previewstate.h"
#include "core/ptr.h"
#include "input/inputserver.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "basegamefeature/managers/factorymanager.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/managers/entitymanager.h"
#include "basegamefeature/basegameprotocol.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "physics/physicsserver.h"
#include "qtaddons/remoteinterface/qtremoteserver.h"
#include "qtaddons/remoteinterface/qtremoteclient.h"
#include "debugrender/debugshaperenderer.h"
#include "physics/model/physicsmodel.h"
#include "physics/resource/managedphysicsmodel.h"
#include "debugrender/debugrender.h"
#include "posteffect/posteffectentity.h"
#include "contentbrowserapp.h"
#include "resources/resourcemanager.h"
#include "messaging/messagecallbackhandler.h"
#include "graphicsfeature/properties/mayacameraproperty.h"
#include "visibility/visibilitysystems/visibilityquadtree.h"
#include "visibility/visibilitysystems/visibilitysystembase.h"
#include "renderutil/nodelookuputil.h"
#include "coregraphics/streamtexturesaver.h"

using namespace Util;
using namespace Graphics;
using namespace GraphicsFeature;
using namespace Physics;
using namespace Math;
using namespace Input;
using namespace PostEffect;
namespace ContentBrowser
{
__ImplementClass(ContentBrowser::PreviewState, 'PRST',  BaseGameFeature::GameStateHandler);

//------------------------------------------------------------------------------
/**
*/
PreviewState::PreviewState() :
	defaultCam(0),
	enablePhysics(false),
	showControls(false),
	workLight(true),
	managedSurface(NULL),
	surfaceInstance(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PreviewState::~PreviewState()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
PreviewState::OnFrame()
{
	this->HandleInput();

	// get post effect entity
	const Ptr<PostEffectEntity>& peEntity = ContentBrowserApp::Instance()->GetPostEffectEntity();

	// handle light transform depending on light mode
	if (this->workLight)
	{
#ifdef USE_POSTEFFECT_GLOBALLIGHTTRANSFORM
		peEntity->Params().light->SetLightTransform(this->defaultCam->GetMatrix44(Attr::Transform));
		peEntity->SetDirty(true);
#else
		GraphicsFeatureUnit::Instance()->GetGlobalLightEntity()->SetTransform(this->defaultCam->GetMatrix44(Attr::Transform));
#endif
	}
	else
	{
#ifdef USE_POSTEFFECT_GLOBALLIGHTTRANSFORM
		peEntity->Params().light->SetLightTransform(this->lightTransform);
		peEntity->SetDirty(true);
#else
		GraphicsFeatureUnit::Instance()->GetGlobalLightEntity()->SetTransform(this->lightTransform);
#endif
	}

	// update remote interface and client
	QtRemoteInterfaceAddon::QtRemoteServer::Instance()->OnFrame();
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("editor")->OnFrame();

	// renders physics
	if (this->enablePhysics)
	{
		PhysicsServer::Instance()->GetScene()->RenderDebug();
	}

	// renders controls
	if (this->showControls)
	{
		_debug_text("Hold ALT, then press and hold LMB to move camera", Math::float2(0.02f, 0.02f), Math::float4(1));
        _debug_text("Press SPACE to reset camera", Math::float2(0.02f, 0.04f), Math::float4(1));
		_debug_text("Press F to focus camera on asset", Math::float2(0.02f, 0.06f), Math::float4(1));

		if (!this->workLight)
		{
			_debug_text("Press and hold LMB to move direction of light", Math::float2(0.02f, 0.08f), Math::float4(1));
		}
		
	}

	return GameStateHandler::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void 
PreviewState::HandleInput()
{
	// get mouse input device
	const Ptr<Mouse>& mouse = InputServer::Instance()->GetDefaultMouse();
	const Ptr<Keyboard>& keyboard = InputServer::Instance()->GetDefaultKeyboard();

	if (!this->workLight && mouse->ButtonPressed(MouseButton::LeftButton) && !keyboard->KeyPressed(Key::LeftMenu))
	{
		// get movement
		float2 movement = mouse->GetMovement();

		// create matrix for x-rotation
		matrix44 xrot = matrix44::rotationy(movement.x()/100);

		// create matrix for y-rotation
		matrix44 yrot = matrix44::rotationx(movement.y()/100);

		// combine
		matrix44 trans = matrix44::multiply(xrot, yrot);

		// now apply to light
		this->lightTransform = matrix44::multiply(this->lightTransform, trans);
	}

	if (keyboard->KeyPressed(Key::F))
	{
		bbox boundingBox = this->modelEntity->GetGlobalBoundingBox();
		Ptr<GraphicsFeature::MayaCameraProperty> cameraProperty = this->defaultCam->FindProperty(GraphicsFeature::MayaCameraProperty::RTTI).downcast<GraphicsFeature::MayaCameraProperty>();
		cameraProperty->SetCameraFocus(boundingBox.center(), boundingBox.diagonal_size());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::OnStateEnter(const Util::String& prevState)
{
	GameStateHandler::OnStateEnter(prevState);

	// get the default stage
	this->defaultStage = GraphicsServer::Instance()->GetDefaultView()->GetStage();

	// lookup frameshader used by a frame draw
	Ptr<Frame::FrameShader> frameShader = Frame::FrameServer::Instance()->LookupFrameShader(NEBULA3_DEFAULT_FRAMESHADER_NAME);

	// setup surface view target
	this->surfaceViewTarget = CoreGraphics::RenderTarget::Create();
	this->surfaceViewTarget->SetResolveTextureResourceId("ResourcePreviewTarget");
	this->surfaceViewTarget->SetWidth(150);
	this->surfaceViewTarget->SetHeight(150);
	this->surfaceViewTarget->SetAntiAliasQuality(CoreGraphics::AntiAliasQuality::None);
	this->surfaceViewTarget->SetColorBufferFormat(CoreGraphics::PixelFormat::SRGBA8);
	this->surfaceViewTarget->Setup();

	// setup model stage
	Ptr<Visibility::VisibilityQuadtree> visSystem = Visibility::VisibilityQuadtree::Create();
	visSystem->SetQuadTreeSettings(4, Math::bbox());
	Util::Array<Ptr<Visibility::VisibilitySystemBase> > visSystems;
	visSystems.Append(visSystem.cast<Visibility::VisibilitySystemBase>());
	this->surfaceStage = Graphics::GraphicsServer::Instance()->CreateStage("SurfacePreviewStage", visSystems);
	this->surfaceView = Graphics::GraphicsServer::Instance()->CreateView(Graphics::View::RTTI, "SurfacePreviewView", false);
	this->surfaceView->SetStage(this->surfaceStage);
	this->surfaceView->SetOffscreenTarget(this->surfaceViewTarget);
	this->surfaceView->SetFrameShader(frameShader);
	Math::rectangle<int> viewport;
	viewport.left = 0;
	viewport.top = 0;
	viewport.bottom = 150;
	viewport.right = 150;
	this->surfaceView->SetResolveRect(viewport);

	// setup camera and attach to stage
	this->surfaceCamera = CameraEntity::Create();
	Graphics::CameraSettings settings;
	settings.SetupPerspectiveFov(n_deg2rad(90.0f), 1, 0.1f, 1000);
	this->surfaceCamera->SetCameraSettings(settings);
	this->surfaceCamera->SetTransform(matrix44::lookatrh(point(1, 1, 1), point(0, 0, 0), vector::upvec()));
	this->surfaceStage->AttachEntity(this->surfaceCamera.cast<GraphicsEntity>());

	// setup light
	this->surfaceLight = GlobalLightEntity::Create();
	this->surfaceLight->SetVolumetric(false);
	this->surfaceLight->SetCastShadows(false);
	this->surfaceLight->SetColor(float4(10));
	this->surfaceStage->AttachEntity(this->surfaceLight.cast<GraphicsEntity>());

	// create surface placeholder model
	this->surfaceModelEntity = ModelEntity::Create();
	this->surfaceModelEntity->SetTransform(matrix44::translation(0, 0, 0));
	this->surfaceModelEntity->SetResourceId("mdl:system/shadingsphere.n3");
	this->surfaceModelEntity->SetLoadSynced(true);
	this->surfaceStage->AttachEntity(this->surfaceModelEntity.cast<GraphicsEntity>());

	// create placeholder model
	this->modelEntity = ModelEntity::Create();
	this->modelEntity->SetTransform(matrix44::translation(0, 0, 0));
	this->modelEntity->SetResourceId("mdl:system/placeholder.n3");
	this->modelEntity->SetLoadSynced(true);
	this->defaultStage->AttachEntity(this->modelEntity.cast<GraphicsEntity>());

	// create camera
	this->defaultCam = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("Camera", "Camera");
	BaseGameFeature::EntityManager::Instance()->AttachEntity(defaultCam);

	// disable gravity
	Physics::PhysicsServer::Instance()->GetScene()->SetGravity(Math::vector(0,0,0));
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::OnStateLeave(const Util::String& nextState)
{
	// cleanup scene before quitting application
	this->defaultStage->RemoveEntity(this->modelEntity.cast<GraphicsEntity>());
	this->modelEntity = 0;

	this->defaultStage = 0;
	this->defaultCam = 0;
	this->physicsModel = 0;
	this->physicsObjects.Clear();

	this->surfaceViewTarget->Discard();
	this->surfaceViewTarget = 0;
	this->surfaceStage->RemoveAllEntities();
	this->surfaceModelEntity = 0;
	this->surfaceLight = 0;
	this->surfaceCamera = 0;
	this->surfaceLight = 0;
	this->surfaceStage = 0;
	this->surfaceView = 0;

	if (this->managedSurface.isvalid())
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedSurface.upcast<Resources::ManagedResource>());
		this->managedSurface = 0;
	}

	GameStateHandler::OnStateLeave(nextState);
}

//------------------------------------------------------------------------------
/**
*/
bool
PreviewState::SetModel(const Resources::ResourceId& resource)
{
	// create placeholder model
	this->defaultStage->RemoveEntity(this->modelEntity.cast<GraphicsEntity>());
	this->modelEntity->SetTransform(matrix44::translation(0.0, 0.0, 0.0));
	this->modelEntity->SetResourceId(resource);
	this->defaultStage->AttachEntity(this->modelEntity.cast<GraphicsEntity>());

    // setup anim event tracking
    this->modelEntity->ConfigureAnimEventTracking(true, false);

	// fetch skins
	this->modelEntity->ValidateCharacter();
	Ptr<FetchSkinList> fetchSkinsMessage = FetchSkinList::Create();
    __Send(this->modelEntity, fetchSkinsMessage);
	this->OnFetchedSkinList(fetchSkinsMessage.upcast<Messaging::Message>());

	return this->modelEntity->IsValid();
}

//------------------------------------------------------------------------------
/**
*/
bool
PreviewState::SetPhysics(const Resources::ResourceId& resource)
{
	for (int i = 0; i < physicsObjects.Size(); i++)
	{
		PhysicsServer::Instance()->GetScene()->Detach(this->physicsObjects[i]);
	}
	this->physicsObjects.Clear();
	if (this->physicsModel.isvalid())
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->physicsModel.cast<Resources::ManagedResource>());
	}
	this->physicsModel = 0;

	// create physics resource
	this->physicsModel = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsModel::RTTI, resource).cast<ManagedPhysicsModel>();

	// if succeeded, create instance of model
	if (this->physicsModel.isvalid())
	{
		this->physicsObjects = physicsModel->GetModel()->CreateStaticInstance(matrix44::identity());
		for (int i = 0; i < physicsObjects.Size(); i++)
		{
			PhysicsServer::Instance()->GetScene()->Attach(physicsObjects[i]);
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::SetSurface(const Resources::ResourceId& resource)
{
	if (this->managedSurface.isvalid())
	{
		Resources::ResourceManager::Instance()->DiscardManagedResource(this->managedSurface.upcast<Resources::ManagedResource>());
		this->managedSurface = 0;
	}

	// thankfully, this model uses a hierarchy we know already
	Ptr<Models::StateNodeInstance> node = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(this->surfaceModelEntity, "root/sphere");
	this->managedSurface = Resources::ResourceManager::Instance()->CreateManagedResource(Materials::Surface::RTTI, String::Sprintf("sur:%s.sur", resource.AsString().AsCharPtr()), NULL, true).downcast<Materials::ManagedSurface>();
	this->surfaceInstance = this->managedSurface->GetSurface()->CreateInstance();
	node->SetSurfaceInstance(this->surfaceInstance);
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::SetSurfacePreview(bool b)
{
	Ptr<Graphics::View> view = GraphicsServer::Instance()->GetDefaultView();
	if (b)
	{
		view->SetStage(this->surfaceStage);
	}
	else
	{
		view->SetStage(this->defaultStage);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::SaveThumbnail(const Util::String& path, bool swapStage)
{
	// render a single frame
	if (swapStage)
	{
		CoreGraphics::DisplayMode mode = CoreGraphics::DisplayDevice::Instance()->GetDisplayMode();
		Graphics::CameraSettings settings;
		settings.SetupPerspectiveFov(n_deg2rad(90.0f), mode.GetWidth() / float(mode.GetHeight()), 0.1f, 1000);

		this->surfaceCamera->SetTransform(this->defaultCam->GetMatrix44(Attr::Transform));
		this->surfaceView->SetStage(this->defaultStage);
		this->surfaceCamera->SetCameraSettings(settings);
	}
	this->surfaceView->SetCameraEntity(this->surfaceCamera);
	this->surfaceView->OnFrame(NULL, 0, 0, false);
	this->surfaceView->SetCameraEntity(NULL);
	if (swapStage)
	{
		Graphics::CameraSettings settings;
		settings.SetupPerspectiveFov(n_deg2rad(90.0f), 1, 0.1f, 1000);
		
		this->surfaceView->SetStage(this->surfaceStage);
		this->surfaceCamera->SetTransform(matrix44::lookatrh(point(1, 1, 1), point(0, 0, 0), vector::upvec()));
		this->surfaceCamera->SetCameraSettings(settings);
	}
	Ptr<CoreGraphics::Texture> tex = this->surfaceViewTarget->GetResolveTexture();
	Ptr<CoreGraphics::StreamTextureSaver> saver = CoreGraphics::StreamTextureSaver::Create();
	Ptr<IO::Stream> stream = IO::IoServer::Instance()->CreateStream(path);
	saver->SetFormat(CoreGraphics::ImageFileFormat::PNG);
	saver->SetMipLevel(0);
	saver->SetStream(stream);
	tex->SetSaver(saver.upcast<Resources::ResourceSaver>());
	n_assert(tex->Save());
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::PreImportModel()
{
	this->defaultStage->RemoveEntity(this->modelEntity.cast<GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void
PreviewState::PostImportModel()
{
	this->defaultStage->AttachEntity(this->modelEntity.cast<GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void 
PreviewState::OnFetchedSkinList( const Ptr<Messaging::Message>& msg )
{
	Ptr<Graphics::FetchSkinList> rMsg = msg.downcast<Graphics::FetchSkinList>();

	// if we have a skin, apply the first
	const Array<StringAtom>& skins = rMsg->GetSkins();
	IndexT i;
	for (i = 0; i < skins.Size(); i++)
	{
		Ptr<Graphics::ShowSkin> showSkin = Graphics::ShowSkin::Create();
		showSkin->SetSkin(skins[i]);
		this->modelEntity->HandleMessage(showSkin.cast<Messaging::Message>());
	}
}

} // namespace ContentBrowser