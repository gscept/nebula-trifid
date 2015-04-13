//------------------------------------------------------------------------------
//  previewstate.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "leveleditorstate.h"
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
#include "entityutils/selectionutil.h"
#include "entityutils/placementutil.h"
#include "leveleditor2entitymanager.h"
#include "lighting/lighttype.h"
#include "properties/lightproperty.h"
#include "posteffect/posteffectmanager.h"
#include "properties/mayacameraproperty.h"
#include "actions/duplicateaction.h"
#include "remoteinterface/qtremoteclient.h"
#include "remoteinterface/qtremoteserver.h"
#include "leveleditor2app.h"
#include "properties/editorproperty.h"
#include "godrays/godrayrendermodule.h"
#include "dynui/imguiaddon.h"

using namespace Util;
using namespace Graphics;
using namespace GraphicsFeature;
using namespace Math;
using namespace Input;

namespace LevelEditor2
{
__ImplementClass(LevelEditor2::LevelEditorState, 'LPVS',  BaseGameFeature::GameStateHandler);

//------------------------------------------------------------------------------
/**
*/
LevelEditorState::LevelEditorState() :
	defaultCam(0),
	activeTransformAction(0)
{	
	this->activateSelectionZoom = Input::Key::F;
}

//------------------------------------------------------------------------------
/**
*/
LevelEditorState::~LevelEditorState()
{
	this->selectionUtil = 0;
	this->placementUtil = 0;
	this->defaultCam = 0;	
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
LevelEditorState::OnFrame()
{
	Dynui::ImguiAddon::BeginFrame();
	this->HandleInput();
	this->console->Render();
	this->selectionUtil->Render();
	this->placementUtil->Render();
	
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("cb")->OnFrame();
	QtRemoteInterfaceAddon::QtRemoteClient::GetClient("viewer")->OnFrame();	
	QtRemoteInterfaceAddon::QtRemoteServer::Instance()->OnFrame();

	Navigation::NavigationServer::Instance()->RenderDebug();
	Dynui::ImguiAddon::EndFrame();
	return GameStateHandler::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::HandleInput()
{
	const Ptr<Input::Keyboard>& keyboard = InputServer::Instance()->GetDefaultKeyboard();

	// check if we are in camera rotation mode, ignore input then
	if (keyboard->KeyPressed(Input::Key::Menu))
	{
		return;
	}

	bool handled = false;
	if (this->selectionUtil->HasSelection())
	{
		handled = this->placementUtil->HandleInput();
	}

	// only handle selections if we're not handling placement
	if (!handled)
	{
		handled = this->selectionUtil->HandleInput();
		if (handled)
		{
			this->HandleSelection();
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::OnStateEnter( const Util::String& prevState )
{
	GameStateHandler::OnStateEnter(prevState);

	// get default state
	Ptr<Stage> defaultStage = GraphicsFeatureUnit::Instance()->GetDefaultStage();

	// create camera
	this->defaultCam = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("Camera", "Camera");
	this->defaultCam->SetBool(Attr::MayaCameraRenderCenterOfInterest, true);
	BaseGameFeature::EntityManager::Instance()->AttachEntity(defaultCam);

	// create selection and placement utility
	this->selectionUtil = SelectionUtil::Create();
	this->placementUtil = PlacementUtil::Create();
	this->placementUtil->SetCameraEntity(this->defaultCam);

	Physics::PhysicsServer::Instance()->GetScene()->SetGravity(Math::vector(0,0,0));

	// enable graphics-based picking
	Ptr<EnablePicking> picking = EnablePicking::Create();
	picking->SetEnabled(true);
	GraphicsInterface::Instance()->Send(picking.upcast<Messaging::Message>());

	// check for settings
	BaseGameFeature::UserProfile* userProfile = BaseGameFeature::LoaderServer::Instance()->GetUserProfile();  
	// make sure the important ones exist
	if(!userProfile->HasAttr("CreateCameraRelative"))
	{
		userProfile->SetBool("CreateCameraRelative",true);
	}	
	if (!userProfile->HasAttr("SelectionColour"))
	{
		userProfile->SetFloat4("SelectionColour", Math::float4(1.0f, 0.3f, 0.0f, 0.3f));
	}
	LevelEditor2App::Instance()->GetWindow()->SetSelectionColour(userProfile->GetFloat4("SelectionColour"));
	// create console
	this->console = Dynui::ImguiConsole::Create();
	this->console->Setup();
	this->consoleHandler = Dynui::ImguiConsoleHandler::Create();
	this->consoleHandler->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::OnStateLeave( const Util::String& nextState )
{
	this->consoleHandler->Discard();
	this->consoleHandler = 0;
	this->console->Discard();
	this->console = 0;

	// get default stage 
	Ptr<Stage> defaultStage = GraphicsFeatureUnit::Instance()->GetDefaultStage();

	// cleanup scene before quitting application
	this->selectionUtil->ClearSelection();
	this->selectionUtil = 0;
	this->placementUtil = 0;

	// save settings
	BaseGameFeature::LoaderServer::Instance()->GetUserProfile()->Save();
	GameStateHandler::OnStateLeave(nextState);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::HandleSelection()
{
	Array<matrix44> entityMatrices;
	Util::Array<Ptr<Game::Entity> > selectedEntities = this->selectionUtil->GetSelectedEntities();

	IndexT i;
	for (i = 0; i < selectedEntities.Size(); i++)
	{
		entityMatrices.Append(selectedEntities[i]->GetMatrix44(Attr::Transform));
	}

	this->placementUtil->SetSelection(entityMatrices);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::HandlePlacementChanges()
{
	Util::Array<matrix44> transformedMatrices = this->placementUtil->GetSelection();
	Util::Array<Ptr<Game::Entity> > selectedEntities = this->selectionUtil->GetSelectedEntities();
	IndexT i;
	for (i=0; i < selectedEntities.Size(); i++)
	{
		if(i < transformedMatrices.Size())
		{
			Ptr<BaseGameFeature::SetTransform> setTransformMsg = BaseGameFeature::SetTransform::Create();
			setTransformMsg->SetMatrix(transformedMatrices[i]);
			selectedEntities[i]->SendSync(setTransformMsg.cast<Messaging::Message>());
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::UpdateSelection(const Util::Array<EntityGuid>& entityIds)
{
	Ptr<SelectAction> action = SelectAction::Create();
	action->SetEntities(entityIds);
	action->SetSelectionMode(SelectAction::SetSelection);
	ActionManager::Instance()->PerformAction(action.cast<Action>());
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::ClearSelection()
{
	SelectionUtil::Instance()->ClearSelection();
	PlacementUtil::Instance()->ClearSelection();
	LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->clearSelection();
	
}


} // namespace LevelEditor2