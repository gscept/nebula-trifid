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
	workLight(true)
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
	if(this->enablePhysics)
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
PreviewState::OnStateEnter( const Util::String& prevState )
{
	GameStateHandler::OnStateEnter(prevState);

	// get default state
	Ptr<Stage> defaultStage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	
	// create placeholder model
	this->modelEntity = ModelEntity::Create();
	this->modelEntity->SetTransform(matrix44::translation(0, 0, 0));
	this->modelEntity->SetResourceId("mdl:system/placeholder.n3");
	this->modelEntity->SetLoadSynced(true);
	defaultStage->AttachEntity(this->modelEntity.cast<GraphicsEntity>());

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
PreviewState::OnStateLeave( const Util::String& nextState )
{
	// get default stage 
	Ptr<Stage> defaultStage = GraphicsFeatureUnit::Instance()->GetDefaultStage();

	// cleanup scene before quitting application
	defaultStage->RemoveEntity(this->modelEntity.cast<GraphicsEntity>());
	this->modelEntity = 0;

	this->defaultCam = 0;
	this->physicsModel = 0;
	this->physicsObjects.Clear();

	GameStateHandler::OnStateLeave(nextState);
}

//------------------------------------------------------------------------------
/**
*/
bool
PreviewState::SetModel(const Resources::ResourceId& resource)
{
	// get default state and remove current model entity from stage
	Ptr<Stage> defaultStage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	defaultStage->RemoveEntity(this->modelEntity.cast<GraphicsEntity>());

	// create placeholder model
	this->modelEntity->SetTransform(matrix44::translation(0.0, 0.0, 0.0));
	this->modelEntity->SetResourceId(resource);
	defaultStage->AttachEntity(this->modelEntity.cast<GraphicsEntity>());

    // setup anim event tracking
    this->modelEntity->ConfigureAnimEventTracking(true, false);

	// fetch skins
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