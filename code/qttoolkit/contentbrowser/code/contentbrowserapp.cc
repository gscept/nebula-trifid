
//------------------------------------------------------------------------------
//  contentbrowserapp.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "contentbrowserapp.h"
#include "util/commandlineargs.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "io/assignregistry.h"
#include "io/assign.h"
#include "resources/resource.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "style/nebulastyletool.h"
#include "remoteinterface/qtremoteprotocol.h"
#include "editorfeatures/editorbasegamefeature.h"
#include "platform.h"
#include "qtaddons/miniexporter/code/miniexporter.h"
#include <QPlastiqueStyle>
#include "resources/resourcemanager.h"
#include "shady/code/simulation/simulationcommands.h"
#include "algorithm/algorithmprotocol.h"

using namespace QtToolkitUtil;
using namespace QtRemoteInterfaceAddon;
using namespace BaseGameFeature;
using namespace Util;
using namespace Actions;
using namespace Graphics;
using namespace PostEffect;

namespace ContentBrowser
{ 

__ImplementSingleton(ContentBrowserApp);
//------------------------------------------------------------------------------
/**
*/
ContentBrowserApp::ContentBrowserApp() : 
	previewState(0)
{
	__ConstructSingleton;
#if __NEBULA3_HTTP__
	this->defaultTcpPort = 2101;
#endif
}

//------------------------------------------------------------------------------
/**
*/
ContentBrowserApp::~ContentBrowserApp()
{
	__DestructSingleton;
	if (this->IsOpen())
	{
		this->Close();
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
ContentBrowserApp::Open()
{
	n_assert(!this->IsOpen());
	if(GameApplication::Open())
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::Close()
{
	n_assert(this->IsOpen());
	GameApplication::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::SetupStateHandlers()
{
	this->previewState = PreviewState::Create();
	this->previewState->SetSetupMode(GameStateHandler::NewGame);
	this->previewState->SetName("PreviewState");
	this->previewState->SetLevelName("EmptyWorld");

	this->AddStateHandler(this->previewState.upcast<App::StateHandler>());

	this->SetState("PreviewState");

	// setup window
	this->browserWindow->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::SetupGameFeatures()
{   
	GameApplication::SetupGameFeatures();

    // setup project info
#ifdef __WIN32__
    this->projInfo.SetCurrentPlatform(ToolkitUtil::Platform::Win32);
#elif __LINUX__
    this->projInfo.SetCurrentPlatform(ToolkitUtil::Platform::Linux);
#endif

    this->projInfo.Setup();
    IO::AssignRegistry::Instance()->SetAssign(IO::Assign("src", this->projInfo.GetAttr("SrcDir")));
    IO::AssignRegistry::Instance()->SetAssign(IO::Assign("dst", this->projInfo.GetAttr("DstDir")));
    IO::AssignRegistry::Instance()->SetAssign(IO::Assign("int", this->projInfo.GetAttr("IntDir")));

	// setup feature
	this->inputFeature = InputFeature::InputFeatureUnit::Create();
	this->gameServer->AttachGameFeature(this->inputFeature.upcast<Game::FeatureUnit>());

    // create and attach qt feature
    this->qtFeature = QtFeature::QtFeatureUnit::Create();
    this->gameServer->AttachGameFeature(this->qtFeature.upcast<Game::FeatureUnit>());

    // create palette
    QtToolkitUtil::NebulaStyleTool palette;
    QApplication::setPalette(palette);
    QApplication::setStyle(new QPlastiqueStyle);
	QApplication* app = (QApplication*)QApplication::instance();
	app->setFont(palette.font);
	app->setStyleSheet(palette.globalStyleSheet);

    // run mini exporter
    Ptr<MiniExporterAddon::MiniExporter> miniExporter = MiniExporterAddon::MiniExporter::Create();
    miniExporter->Run();

    // setup remote server
    this->remoteServer = QtRemoteServer::Create();
    this->remoteServer->SetPort(3102);
    this->remoteServer->Open();

    // setup remote client
    this->remoteClient = QtRemoteClient::Create();
    this->remoteClient->Register("editor");
    this->remoteClient->SetPort(3103);

    // create qt gui window
    this->browserWindow = new ContentBrowserWindow;
    this->browserWindow->GetNebulaWidget()->SetCallbackApplication(this);

    // create and attach default graphic features
    this->graphicsFeature = GraphicsFeature::GraphicsFeatureUnit::Create();
    this->graphicsFeature->SetRenderDebug(true);

    // open window
    this->browserWindow->show();
	QApplication::processEvents();

	// show splash
	this->splash = SplashAddon::Splash::Create();
	this->splash->SetTitle("Content Browser");
	this->splash->Open();

    // create base game feature
    this->baseFeature = Toolkit::EditorBaseGameFeatureUnit::Create();
    
    // add arguments for embedding the Nebula context in the desired Nebula frame
    String extraArgs;
    extraArgs.Format("-embedded");
    CommandLineArgs args(extraArgs);
    this->args.AppendCommandString(extraArgs);
    this->graphicsFeature->SetCmdLineArgs(this->GetCmdLineArgs());
    this->graphicsFeature->SetWindowData(this->browserWindow->GetNebulaWindowData());
	this->graphicsFeature->SetupDisplay();

    this->gameServer->AttachGameFeature(this->baseFeature.upcast<Game::FeatureUnit>());


	// add effects feature for playing anim events
	this->effectsFeature = EffectsFeature::EffectsFeatureUnit::Create();
	this->gameServer->AttachGameFeature(this->effectsFeature.cast<Game::FeatureUnit>());


    this->gameServer->AttachGameFeature(this->graphicsFeature.upcast<Game::FeatureUnit>());    

    // setup physics feature
    this->physicsFeature = PhysicsFeature::PhysicsFeatureUnit::Create();
    this->physicsFeature->SetRenderDebug(true);
	this->physicsFeature->SetInitVisualDebuggerFlag(false);
    this->gameServer->AttachGameFeature(this->physicsFeature.upcast<Game::FeatureUnit>());

	// setup scripting
	this->scriptFeature = ScriptingFeature::ScriptingFeatureUnit::Create();
	this->gameServer->AttachGameFeature(this->scriptFeature.upcast<Game::FeatureUnit>());
	Commands::SimulationCommands::Register();

    // create material database
    this->materialDatabase = MaterialDatabase::Create();
    this->materialDatabase->Open();

	this->postEffectFeature = PostEffectFeatureUnit::Create();
	this->gameServer->AttachGameFeature(this->postEffectFeature.upcast<Game::FeatureUnit>());

    // create default post effect entity
    this->postEffectEntity = PostEffectManager::Instance()->GetDefaultEntity();
   
	// create ui feature
	this->uiFeature = UI::UiFeatureUnit::Create();
	this->uiFeature->SetRenderDebug(true);

	// do not load ui fonts and layouts
	this->uiFeature->SetAutoload(false);
	this->gameServer->AttachGameFeature(this->uiFeature.upcast<Game::FeatureUnit>());

	// for the ease of testing, load all fonts
	this->uiFeature->LoadAllFonts("gui:");

	// setup grid
	this->grid = Grid::GridAddon::Create();
	this->grid->Setup();

	// setup intermediate gui
	this->imgui = Dynui::ImguiAddon::Create();
	this->imgui->Setup();
	
    // register input proxy for inputs from nebula to the qt app
    this->qtFeature->RegisterQtInputProxy(this->browserWindow);

    // set all resource mappers to be synchronized
    Resources::ResourceManager::Instance()->SetMappersAsync(false);

	// close splash
	this->splash->Close();
	this->splash = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::CleanupGameFeatures()
{
	this->postEffectEntity = 0;

	// close grid
	this->grid->Discard();
	this->grid = 0;

	// close imgui
	this->imgui->Discard();
	this->imgui = 0;

	this->gameServer->RemoveGameFeature(this->effectsFeature.upcast<Game::FeatureUnit>());
	this->effectsFeature = 0;
	this->gameServer->RemoveGameFeature(this->postEffectFeature.upcast<Game::FeatureUnit>());
	this->postEffectFeature = 0;
	this->gameServer->RemoveGameFeature(this->uiFeature.upcast<Game::FeatureUnit>());
	this->uiFeature = 0;
	this->gameServer->RemoveGameFeature(this->scriptFeature.upcast<Game::FeatureUnit>());
	this->scriptFeature = 0;
	this->gameServer->RemoveGameFeature(this->physicsFeature.upcast<Game::FeatureUnit>());
	this->physicsFeature = 0;
	this->gameServer->RemoveGameFeature(this->baseFeature.upcast<Game::FeatureUnit>());
	this->baseFeature = 0;
	this->gameServer->RemoveGameFeature(this->graphicsFeature.upcast<Game::FeatureUnit>());
	this->graphicsFeature = 0;
	this->gameServer->RemoveGameFeature(this->qtFeature.upcast<Game::FeatureUnit>());
	this->qtFeature = 0;	
	this->gameServer->RemoveGameFeature(this->inputFeature.upcast<Game::FeatureUnit>());
	this->inputFeature = 0;

	// clear state
	this->previewState = 0;

	// shut down remote interface
	this->remoteServer->Close();
	this->remoteServer = 0;

	// unregister client
	this->remoteClient->Unregister("editor");

	// close remote client if needed
	if (this->remoteClient->IsOpen())
	{
		this->remoteClient->Close();
	}
	this->remoteClient = 0;

	// shut down materials
	this->materialDatabase->Close();
	this->materialDatabase = 0;

	// clear undo-redo stack
	this->undoStack.Clear();
	this->redoStack.Clear();

	GameApplication::CleanupGameFeatures();
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::PushAction( const Ptr<BaseAction>& action )
{
	this->undoStack.Push(action);
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::ClearActions()
{
	this->undoStack.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::Undo()
{
	if (this->undoStack.Size() > 0)
	{
		Ptr<BaseAction> action = this->undoStack.Pop();
		action->Undo();
		this->redoStack.Push(action);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ContentBrowserApp::Redo()
{
	if (this->redoStack.Size() > 0)
	{
		Ptr<BaseAction> action = this->redoStack.Pop();
		action->Redo();
		this->undoStack.Push(action);
	}
}
} // namespace ContentBrowser