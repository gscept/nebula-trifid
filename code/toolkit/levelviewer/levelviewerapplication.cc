//------------------------------------------------------------------------------
//  levelviewerapplication.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "levelviewerapplication.h"
#include "graphics/graphicsprotocol.h"
#include "messaging/message.h"
#include "input/keyboard.h"
#include "input/gamepad.h"
#include "managers/factorymanager.h"
#include "managers/entitymanager.h"
#include "gamestates/viewergamestate.h"
#include "scriptingfeature/scriptingfeature.h"
#include "physicsfeature/physicsprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "scriptingfeature/scriptingcommands.h"
#include "gamestates/reloadstate.h"
#include "effectscommands.h"
#include "audioprotocol.h"
#include "uicommands.h"


namespace Tools
{

using namespace QtRemoteInterfaceAddon;
using namespace Graphics;
using namespace Math;
using namespace Input;
using namespace BaseGameFeature;
using namespace Game;

//------------------------------------------------------------------------------
/**
*/
LevelViewerGameStateApplication::LevelViewerGameStateApplication(void)
{
#if __NEBULA3_HTTP__
	this->defaultTcpPort = 2103;
#endif
}

//------------------------------------------------------------------------------
/**
*/
LevelViewerGameStateApplication::~LevelViewerGameStateApplication(void)
{
	if (this->IsOpen())
	{
		this->Close();
	}
}

//------------------------------------------------------------------------------
/**
*/
bool LevelViewerGameStateApplication::Open()
{
	n_assert(!this->IsOpen());
	if (GameApplication::Open())
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void LevelViewerGameStateApplication::Close()
{
	GameApplication::Close();
}


//------------------------------------------------------------------------------
/**
*/
void 
LevelViewerGameStateApplication::SetupStateHandlers()
{
	Ptr<LevelViewerGameState> gameState = LevelViewerGameState::Create();
	if (this->args.HasArg("-level"))
	{
		// overwrite level set, use this
		gameState->SetLevelName(this->args.GetString("-level"));
		gameState->SetSetupMode(LevelViewerGameState::LoadLevel);
	}
	else
	{
		gameState->SetLevelName("Empty");
		gameState->SetSetupMode(LevelViewerGameState::NewGame);
	}	
	gameState->SetName("LevelViewerGameState");

	Ptr<ReloadState> loadState = ReloadState::Create();
	loadState->SetSetupMode(GameStateHandler::EmptyWorld);
	loadState->SetName("Reload");
	this->AddStateHandler(loadState.get());
	

	this->AddStateHandler(gameState.get());

	this->SetState("LevelViewerGameState");


}

//------------------------------------------------------------------------------
/**
*/
void 
LevelViewerGameStateApplication::SetupGameFeatures()
{
	GameApplication::SetupGameFeatures();

	// setup remote server
	this->remoteServer = QtRemoteServer::Create();
	this->remoteServer->SetPort(2104);
	this->remoteServer->Open();

	// setup remote client
	this->remoteClient = QtRemoteClient::Create();
	this->remoteClient->SetPort(2103);

	// create and attach default graphic features
	this->graphicsFeature = GraphicsFeature::GraphicsFeatureUnit::Create();
	this->graphicsFeature->SetCmdLineArgs(this->GetCmdLineArgs());
	this->graphicsFeature->SetRenderDebug(true);
    this->graphicsFeature->SetupDisplay();

	// game features
	this->baseGameFeature = BaseGameFeatureUnit::Create();
	Ptr<LevelViewer::LevelViewerFactoryManager> factory = LevelViewer::LevelViewerFactoryManager::Create();    
	this->baseGameFeature->SetFactoryManager(factory.cast<FactoryManager>());

	// create and attach core features
	this->physicsFeature = PhysicsFeature::PhysicsFeatureUnit::Create(); 
	this->physicsFeature->SetRenderDebug(true);

    // create scripting
	this->scriptingFeature = ScriptingFeature::ScriptingFeatureUnit::Create();

	// create ui
	this->uiFeature = UI::UiFeatureUnit::Create();

    // create effect
	this->effectFeature = EffectsFeature::EffectsFeatureUnit::Create();

	// create post effect
	this->postEffectFeature = PostEffect::PostEffectFeatureUnit::Create();

    // attach features
    this->gameServer->AttachGameFeature(this->graphicsFeature.cast<Game::FeatureUnit>());	
    this->gameServer->AttachGameFeature(this->baseGameFeature.upcast<Game::FeatureUnit>());
    this->gameServer->AttachGameFeature(this->physicsFeature.upcast<Game::FeatureUnit>());
    this->gameServer->AttachGameFeature(this->scriptingFeature.upcast<Game::FeatureUnit>());
    this->gameServer->AttachGameFeature(this->effectFeature.cast<Game::FeatureUnit>());
	this->gameServer->AttachGameFeature(this->uiFeature.cast<Game::FeatureUnit>());
	this->gameServer->AttachGameFeature(this->postEffectFeature.cast<Game::FeatureUnit>());

	// setup intermediate gui
	this->imgui = Dynui::ImguiAddon::Create();
	this->imgui->Setup();	

	Commands::ScriptingCommands::Register();
	Commands::PhysicsProtocol::Register();
	Commands::GraphicsFeatureProtocol::Register();
	Commands::BaseGameProtocol::Register();
	Commands::EffectsCommands::Register();
	Commands::AudioCommands::Register();
	Commands::UICommands::Register();	

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
LevelViewerGameStateApplication::CleanupGameFeatures()
{
	this->remoteServer->Close();
	this->remoteServer = 0;

	// close remote client if needed
	if (this->remoteClient->IsOpen())
	{
		this->remoteClient->Close();
	}

	this->consoleHandler->Discard();
	this->consoleHandler = 0;
	this->console->Discard();
	this->console = 0;

	// close intermediate gui
	this->imgui->Discard();
	this->imgui = 0;

	this->remoteClient = 0;
	this->gameServer->RemoveGameFeature(this->postEffectFeature.upcast<Game::FeatureUnit>());
	this->postEffectFeature = 0;
	this->gameServer->RemoveGameFeature(this->uiFeature.upcast<Game::FeatureUnit>());
	this->uiFeature = 0;
	this->gameServer->RemoveGameFeature(this->scriptingFeature.upcast<Game::FeatureUnit>());
	this->scriptingFeature = 0;
	this->gameServer->RemoveGameFeature(this->effectFeature.upcast<Game::FeatureUnit>());
	this->effectFeature = 0;
	this->gameServer->RemoveGameFeature(this->physicsFeature.upcast<Game::FeatureUnit>());
	this->physicsFeature = 0;
	this->gameServer->RemoveGameFeature(this->graphicsFeature.upcast<Game::FeatureUnit>());
	this->graphicsFeature = 0;
	this->gameServer->RemoveGameFeature(this->baseGameFeature.upcast<Game::FeatureUnit>());
	this->baseGameFeature = 0;

	GameApplication::CleanupGameFeatures();
}
}
