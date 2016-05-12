//------------------------------------------------------------------------------
//  previewstate.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
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
#include "imgui/imgui.h"
#include "debug/debugserver.h"
#include "managers/attributewidgetmanager.h"

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
	activeTransformAction(0),
	showPerformance(false),
	lastFrameTime(1),
	lastUpdateFpsTime(1),
	performanceFrame(0)
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
	this->HandleInput();
	Dynui::ImguiAddon::BeginFrame();
	this->console->Render();
	this->selectionUtil->Render();
	this->placementUtil->Render();

	if (this->showPerformance)
	{
		ImGui::Begin("Performance", NULL, ImVec2(0, 0), -1.0f, ImGuiWindowFlags_AlwaysAutoResize);
			Ptr<Debug::DebugCounter> drawcalls = Debug::DebugServer::Instance()->GetDebugCounterByName("RenderDeviceNumDrawCalls");
			Ptr<Debug::DebugCounter> primitives = Debug::DebugServer::Instance()->GetDebugCounterByName("RenderDeviceNumPrimitives");
			Ptr<Debug::DebugCounter> computes = Debug::DebugServer::Instance()->GetDebugCounterByName("RenderDeviceNumComputes");

			Util::Array<int> drawcallHistory = drawcalls->GetHistory();
			float drawcallHistoryBuffer[90];
			IndexT i;

			// convert draw call history to floats and take the last 90
			IndexT j = 0;
			for (i = Math::n_max(0, drawcallHistory.Size() - 90); i < drawcallHistory.Size(); i++)
			{
				drawcallHistoryBuffer[j++] = drawcallHistory[i];
			}

			// convert primitive history to floats and take the last 90
			Util::Array<int> primitivesHistory = primitives->GetHistory();
			float primitivesHistoryBuffer[90];
			j = 0;
			for (i = Math::n_max(0, primitivesHistory.Size() - 90); i < primitivesHistory.Size(); i++)
			{
				primitivesHistoryBuffer[j++] = primitivesHistory[i];
			}

			// convert compute history to floats and take the last 90
			Util::Array<int> computesHistory = computes->GetHistory();
			float computesHistoryBuffer[90];
			j = 0;
			for (i = Math::n_max(0, computesHistory.Size() - 90); i < computesHistory.Size(); i++)
			{
				computesHistoryBuffer[j++] = computesHistory[i];
			}

			// produce UI
			Timing::Time frame = FrameSync::FrameSyncTimer::Instance()->GetTime();
			if (frame - this->lastUpdateFpsTime > 1.5)
			{
				this->lastFrameTime = FrameSync::FrameSyncTimer::Instance()->GetFrameTime();
				this->lastUpdateFpsTime = frame;
			}
			ImGui::Text("FPS: %.2f", 1 / this->lastFrameTime);
			ImGui::Text("Number of draw calls: %d", drawcallHistory.IsEmpty() ? 0 : drawcallHistory.Back());
			if (!drawcallHistory.IsEmpty())	ImGui::PlotLines("Draw calls", drawcallHistoryBuffer, Math::n_min(drawcallHistory.Size(), 90), 0, NULL, 0, 3000, ImVec2(200, 100));

			ImGui::Text("Number of drawn primitives: %d", primitivesHistory.IsEmpty() ? 0 : primitivesHistory.Back());
			if (!primitivesHistory.IsEmpty()) ImGui::PlotLines("Primitives processed", primitivesHistoryBuffer, Math::n_min(primitivesHistory.Size(), 90), 0, NULL, 0, 10e6, ImVec2(200, 100));

			ImGui::Text("Number of compute shader executions: %d", computesHistory.IsEmpty() ? 0 : computesHistory.Back());
			if (!computesHistory.IsEmpty()) ImGui::PlotLines("GPU kernels", computesHistoryBuffer, Math::n_min(computesHistory.Size(), 90), 0, NULL, 0, 10e6, ImVec2(200, 100));
		ImGui::End();
	}
	
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
	if (this->selectionUtil->HasSelection() && !this->selectionUtil->IsInDrag())
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
	this->defaultCam->SetFloat4(Attr::MayaCameraCenterOfInterest, Math::float4(0, 0, 0, 1));
	Math::matrix44 camTrans;
	camTrans.set_position(Math::float4(10, 10, 10, 1));
	this->defaultCam->SetMatrix44(Attr::Transform, camTrans);
	this->defaultCam->SetFloat4(Attr::MayaCameraCenterOfInterest, Math::float4(0, 0, 0, 1));	
	BaseGameFeature::EntityManager::Instance()->AttachEntity(defaultCam);
	this->fpsCam = BaseGameFeature::FactoryManager::Instance()->CreateEntityByTemplate("FreeCamera", "FreeCamera");
	this->fpsCam->SetBool(Attr::CameraFocus, false);
	this->fpsCam->SetBool(Attr::InputFocus, false);
	BaseGameFeature::EntityManager::Instance()->AttachEntity(fpsCam);
	

	// create selection and placement utility
	this->selectionUtil = SelectionUtil::Create();
	this->placementUtil = PlacementUtil::Create();
	this->placementUtil->Setup();
	this->placementUtil->SetCameraEntity(this->defaultCam);

	Physics::PhysicsServer::Instance()->GetScene()->SetGravity(Math::vector(0,0,0));

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

	// setup picking server
	this->pickingServer = Picking::PickingServer::Create();
	this->pickingServer->Open();

	// setup performance buffers
	this->drawcallBuffer.SetCapacity(90);
	this->primitivesBuffer.SetCapacity(90);
	this->computesBuffer.SetCapacity(90);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditorState::OnStateLeave( const Util::String& nextState )
{
	this->pickingServer->Close();
	this->pickingServer = 0;

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
    AttributeWidgetManager::Instance()->ClearAttributeControllers();
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditorState::TogglePerformanceOverlay()
{
	this->showPerformance = !this->showPerformance;
}

} // namespace LevelEditor2