//------------------------------------------------------------------------------
//  leveleditor2app.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "leveleditor2app.h"
#include "actions/actionmanager.h"
#include "leveleditor2entitymanager.h"
#include "util/commandlineargs.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "io/assignregistry.h"
#include "io/assign.h"
#include "resources/resource.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "io/xmlwriter.h"
#include "properties/editorproperty.h"
#include "leveleditor2/leveleditor2protocol.h"
#include "properties/lightproperty.h"
#include "basegamefeature/basegameprotocol.h"
#include "math/scalar.h"
#include "game/levelexporter.h"
#include "managers/attributewidgetmanager.h"
#include "posteffect/posteffectentity.h"
#include "scriptingfeature/scriptingcommands.h"
#include "physicsfeature/physicsprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "actions/duplicateaction.h"
#include "entityutils/placementutil.h"
#include "properties/mayacameraproperty.h"
#include "style/nebulastyletool.h"
#include <QPlastiqueStyle>
#include "toolkitconsolehandler.h"



using namespace QtRemoteInterfaceAddon;
using namespace Graphics;
using namespace Util;
using namespace BaseGameFeature;
using namespace Attr;
using namespace ToolkitUtil;
using namespace Math;

namespace LevelEditor2
{

__ImplementSingleton(LevelEditor2App);
//------------------------------------------------------------------------------
/**
*/
LevelEditor2App::LevelEditor2App() : 
	editorState(0)	
{
	__ConstructSingleton;
#if __NEBULA3_HTTP__
	this->defaultTcpPort = 2102;
#endif
}

//------------------------------------------------------------------------------
/**
*/
LevelEditor2App::~LevelEditor2App()
{
	this->editorState = 0;

	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool 
LevelEditor2App::Open()
{
    n_assert(!this->IsOpen());
    if (GameApplication::Open())
    {   
		Ptr<ToolkitUtil::ToolkitConsoleHandler> console = ToolkitUtil::ToolkitConsoleHandler::Create();
		IO::Console::Instance()->AttachHandler(console.cast<IO::ConsoleHandler>());
        // add the different game entity types for the leveleditor
        Array<String> props;
        props.Append("LevelEditor2::EditorLightProperty");
        props.Append("LevelEditor2::EditorProperty");
        BaseGameFeature::FactoryManager::Instance()->AddBlueprint("EditorLight","Entity",props,true);

        Array<String> props2;
        props2.Append("GraphicsFeature::GraphicsProperty");
        props2.Append("LevelEditor2::EditorProperty");
        BaseGameFeature::FactoryManager::Instance()->AddBlueprint("EditorEntity","Entity",props2,true);

        Array<String> props3;
        props3.Append("LevelEditor2::EditorProperty");
        BaseGameFeature::FactoryManager::Instance()->AddBlueprint("EditorTransform","Entity",props3,true);

		Array<String> props4;		
		props4.Append("LevelEditor2::EditorNavMeshProperty");
		props4.Append("LevelEditor2::EditorProperty");
		BaseGameFeature::FactoryManager::Instance()->AddBlueprint("EditorNavMesh","Entity",props4,true);

		Array<String> props5;
		props5.Append("LevelEditor2::EditorLightProbeProperty");
		props5.Append("LevelEditor2::EditorProperty");
		BaseGameFeature::FactoryManager::Instance()->AddBlueprint("EditorLightProbe", "Entity", props5, true);

        Array<String> props6;
        props6.Append("GraphicsFeature::GraphicsProperty");
        props6.Append("LevelEditor2::EditorProperty");
        props6.Append("LevelEditor2::EditorNavAreaProperty");        
        BaseGameFeature::FactoryManager::Instance()->AddBlueprint("EditorNavAreaMarker", "Entity", props6, true);

		Array<String> props7;		
		props7.Append("LevelEditor2::EditorMultiselectProperty");
		BaseGameFeature::FactoryManager::Instance()->AddBlueprint("EditorMultiselect", "Entity", props7, true);
        

        this->blueprintManager = Toolkit::EditorBlueprintManager::Create();
		this->blueprintManager->SetLogger(this->GetLogger());
        this->blueprintManager->Open();
        this->blueprintManager->ParseProjectInfo("proj:projectinfo.xml");
        this->blueprintManager->ParseProjectInfo("toolkit:projectinfo.xml");

        this->blueprintManager->ParseBlueprint("proj:data/tables/blueprints.xml");    
        this->blueprintManager->ParseBlueprint("toolkit:data/tables/blueprints.xml");
        // templates need to be parsed from tookit first to add virtual templates with their attributes
        this->blueprintManager->ParseTemplates("toolkit:data/tables/db");
        this->blueprintManager->ParseTemplates("proj:data/tables/db");
		this->blueprintManager->SetTemplateTargetFolder("proj:data/tables/db");


        this->blueprintManager->UpdateAttributeProperties();
        this->blueprintManager->CreateMissingTemplates();
		

        // configure global light
        Math::matrix44 lightTransform = Math::matrix44::rotationx(Math::n_deg2rad(-45.0f));
        LevelEditor2EntityManager::Instance()->CreateLightEntity("GlobalLight");
        Ptr<Game::Entity> glight = LevelEditor2EntityManager::Instance()->GetGlobalLight();

        Ptr<BaseGameFeature::UpdateTransform> msg = BaseGameFeature::UpdateTransform::Create();
        msg->SetMatrix(lightTransform);
        glight->SendSync(msg.cast<Messaging::Message>());

        // create global attributes container and load data
        Ptr<IO::FileStream> globalStream = IO::FileStream::Create();
        globalStream->SetURI("proj:data/tables/globals.xml");
        this->globalAttrs = Attr::AttrContainerXMLStorage::Create();
        this->globalAttrs->SetStream(globalStream.cast<IO::Stream>(),"Globals");
        this->globalAttrs->Load();
    
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2App::Exit()
{
	//QApplication::exit(0);
	Application::Exit();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::Close()
{
	n_assert(this->IsOpen());

	delete this->editorWindow;
	GameApplication::Close();
	this->blueprintManager = 0;
    this->globalAttrs = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::SetupStateHandlers()
{

	this->editorState->SetSetupMode(GameStateHandler::NewGame);
	this->editorState->SetName("LevelEditorState");
	this->editorState->SetLevelName("EmptyWorld");
	this->AddStateHandler(this->editorState.upcast<App::StateHandler>());
	
	this->SetState("LevelEditorState");

	// give reference of the editorstate to the entity treeview
	this->editorWindow->GetEntityTreeWidget()->SetEditorState(this->editorState);

	// setup stuff in the editor window which must happen after all systems have been setup
	this->editorWindow->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::CleanupStateHandlers()
{
	GameApplication::CleanupStateHandlers();
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::SetupGameFeatures()
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
	IO::AssignRegistry::Instance()->SetAssign(IO::Assign("work", "root:work"));

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

    // setup remote server
    this->remoteServer = QtRemoteServer::Create();
    this->remoteServer->SetPort(3103);
    this->remoteServer->Open();

    // setup remote client
    this->remoteClient = QtRemoteClient::Create();
    this->remoteClient->SetPort(3102);
    this->remoteClient->Register("cb");

    // setup remote viewer client
    this->viewerClient = QtRemoteClient::Create();
    this->viewerClient->SetPort(3104);
    this->viewerClient->Register("viewer");
	
    // create editorstate singleton already so we can connect slots to it
    this->editorState = LevelEditorState::Create();

	// create light probe manager, it manages its own UI, so the window needs this to be created first
	this->lightProbeManager = LightProbeManager::Create();

    // create qt gui window
    this->editorWindow = new LevelEditor2Window;
    this->editorWindow->GetNebulaWidget()->SetCallbackApplication(this);

    // create and attach default graphic features
    this->graphicsFeature = GraphicsFeature::GraphicsFeatureUnit::Create();
    this->graphicsFeature->SetRenderDebug(true);

    // open window
    this->editorWindow->show();
    QApplication::processEvents();

	// show splash
	this->splash = SplashAddon::Splash::Create();
	this->splash->SetTitle("Level Editor");
	this->splash->Open();

    // create game feature
    this->baseFeature = Toolkit::EditorBaseGameFeatureUnit::Create();

    // add arguments for embedding the Nebula context in the desired Nebula frame
    String extraArgs;
    extraArgs.Format("-embedded");
    CommandLineArgs args(extraArgs);
    this->args.AppendCommandString(extraArgs);
    this->graphicsFeature->SetCmdLineArgs(this->GetCmdLineArgs());
    this->graphicsFeature->SetWindowData(this->editorWindow->GetNebulaWindowData());
	this->graphicsFeature->SetupDisplay();

    // game feature needs to be attached before graphicsfeature (but after setupdisplay)
    this->gameServer->AttachGameFeature(this->graphicsFeature.upcast<Game::FeatureUnit>());
	this->gameServer->AttachGameFeature(this->baseFeature.upcast<Game::FeatureUnit>());

    // create and attach the leveleditor-specific managers
    Ptr<ActionManager> actionManager = ActionManager::Create();
    this->baseFeature->AttachManager(actionManager.upcast<Game::Manager>());

    Ptr<LevelEditor2EntityManager> entityManager = LevelEditor2EntityManager::Create();
    QTreeWidget* treeWidget = this->editorWindow->GetEntityTreeWidget();
    entityManager->SetEntityTreeWidget(static_cast<EntityTreeWidget*>(treeWidget));
    this->baseFeature->AttachManager(entityManager.upcast<Game::Manager>());

    Ptr<AttributeWidgetManager> widgetManager = AttributeWidgetManager::Create();
    QLayout* widgetContainer = this->editorWindow->GetAttributeWidgetContainer();
    widgetManager->SetAttributeWidgetContainer(widgetContainer);
    widgetManager->SetEntityTreeWidget(static_cast<EntityTreeWidget*>(treeWidget));
    this->baseFeature->AttachManager(widgetManager.upcast<Game::Manager>());

    // create and attach core features
    this->physicsFeature = PhysicsFeature::PhysicsFeatureUnit::Create();
    this->physicsFeature->SetRenderDebug(true);
	this->physicsFeature->SetInitVisualDebuggerFlag(false);
    this->gameServer->AttachGameFeature(this->physicsFeature.upcast<Game::FeatureUnit>());

	this->postEffectFeature = PostEffect::PostEffectFeatureUnit::Create();
	this->gameServer->AttachGameFeature(this->postEffectFeature.upcast<Game::FeatureUnit>());

    // create default post effect entity
    this->postEffectEntity = PostEffect::PostEffectManager::Instance()->GetDefaultEntity();

	this->GetWindow()->GetPostEffectController()->SetPostEffectEntity(this->postEffectEntity);
	this->GetWindow()->GetPostEffectController()->LoadPresets();

    // create level
    this->level = Level::Create();

    this->scriptingFeature = ScriptingFeature::ScriptingFeatureUnit::Create();
    this->gameServer->AttachGameFeature(this->scriptingFeature.upcast<Game::FeatureUnit>());
    Commands::ScriptingCommands::Register();
    Commands::PhysicsProtocol::Register();
    Commands::GraphicsFeatureProtocol::Register();
    Commands::BaseGameProtocol::Register();	
	Commands::LeveleditorCommands::Register();
	Commands::LevelEditor2Protocol::Register();

	Scripting::ScriptServer::Instance()->AddPath("toolkit:data/scripts/?.lua");

	this->ScanScripts();

	// open light probe manager
	this->lightProbeManager->Open();

	// setup intermediate gui
	this->imgui = Dynui::ImguiAddon::Create();
	this->imgui->Setup();

	// setup grid
	this->grid = Grid::GridAddon::Create();
	this->grid->Setup();

	// setup silhouette
	this->silhouette = Silhouette::SilhouetteAddon::Create();
	this->silhouette->Setup();

    this->navigationFeature = Navigation::NavigationFeatureUnit::Create();
    this->gameServer->AttachGameFeature(navigationFeature.upcast<Game::FeatureUnit>());

    // register input proxy for inputs from nebula to the qt app
    this->qtFeature->RegisterQtInputProxy(this->editorWindow);	

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
LevelEditor2App::CleanupGameFeatures()
{
	this->postEffectEntity = 0;
	this->remoteServer->Close();
	this->remoteServer = 0;

	// close and let the light probe manager go
	this->lightProbeManager->Close();
	this->lightProbeManager = 0;

	// close intermediate gui
	this->imgui->Discard();
	this->imgui = 0;

	// close silhouette
	this->silhouette->Discard();
	this->silhouette = 0;

	// close grid
	this->grid->Discard();
	this->grid = 0;

	// close remote client if needed
	this->remoteClient->Unregister("cb");
	if(remoteClient->IsOpen())
	{
		remoteClient->Close();
	}
	remoteClient = 0;
	
	this->viewerClient->Unregister("viewer");
	if(viewerClient->IsOpen())
	{
		viewerClient->Close();
	}
	viewerClient = 0;	

	this->level = 0;

	this->navigation = 0;

	this->postEffectEntity = 0;
	this->gameServer->RemoveGameFeature(this->postEffectFeature.upcast<Game::FeatureUnit>());
	this->postEffectFeature = 0;
	this->gameServer->RemoveGameFeature(this->scriptingFeature.upcast<Game::FeatureUnit>());
	this->scriptingFeature = 0;
	this->gameServer->RemoveGameFeature(this->physicsFeature.upcast<Game::FeatureUnit>());
	this->physicsFeature = 0;
	this->gameServer->RemoveGameFeature(this->baseFeature.upcast<Game::FeatureUnit>());
	this->baseFeature = 0;
	this->gameServer->RemoveGameFeature(this->graphicsFeature.upcast<Game::FeatureUnit>());
	this->graphicsFeature = 0;
	this->gameServer->RemoveGameFeature(this->qtFeature.upcast<Game::FeatureUnit>());
	this->qtFeature = 0;
    this->gameServer->RemoveGameFeature(this->navigationFeature.upcast<Game::FeatureUnit>());
	this->editorState = 0;
	this->gameServer->RemoveGameFeature(this->inputFeature.upcast<Game::FeatureUnit>());
	this->inputFeature = 0;

	GameApplication::CleanupGameFeatures();
}


//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::DuplicateCurrentSelection()
{    
	Util::Array<Ptr<Game::Entity>> entities = SelectionUtil::Instance()->GetSelectedEntities(true);		
	Ptr<DuplicateAction> dpl = DuplicateAction::Create();
	dpl->SetEntities(entities);
	Util::String msg;
	if(dpl->CheckIfAllowed(msg))
	{
		ActionManager::Instance()->PerformAction(dpl.cast<Action>());
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::ToggleTranslateFeature()
{
	PlacementUtil::Instance()->ToggleTransformMode(PlacementUtil::TRANSLATE);
}


//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::ToggleRotateFeature()
{
	PlacementUtil::Instance()->ToggleTransformMode(PlacementUtil::ROTATE);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::ToggleScaleFeature()
{
	PlacementUtil::Instance()->ToggleTransformMode(PlacementUtil::SCALE);
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::GroupSelection()
{
	EntityGuid id = LevelEditor2EntityManager::Instance()->CreateTransformEntity("Group");

	if(SelectionUtil::Instance()->HasSelection())
	{
		EntityGuid realId = LevelEditor2EntityManager::Instance()->GetEntityById(id)->GetGuid(Attr::EntityGuid);
		Util::Array<Ptr<Game::Entity>> entities = SelectionUtil::Instance()->GetSelectedEntities();
		bool sameParent = true;
		EntityGuid lastGuid = entities[0]->GetGuid(Attr::ParentGuid);
		for(IndexT i = 0 ; i < entities.Size() ; i++)
		{	
			EntityGuid next = entities[i]->GetGuid(Attr::ParentGuid);
			if (next != lastGuid)
			{
				sameParent = false;
			}
			entities[i]->SetGuid(Attr::ParentGuid,realId);
							
		}
		if (sameParent && lastGuid.IsValid())
		{
			LevelEditor2EntityManager::Instance()->GetEntityById(id)->SetGuid(Attr::ParentGuid, lastGuid);
		}
		LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->RebuildTree();
		Ptr<SelectAction> action = SelectAction::Create();
		action->SetSelectionMode(SelectAction::SetSelection);
		Util::Array<EntityGuid> node;
		node.Append(id);
		action->SetEntities(node);
		ActionManager::Instance()->PerformAction(action.cast<Action>());
		LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->GetEntityTreeItem(id)->setExpanded(true);
	}			
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::FocusOnSelection()
{
	// If none is selected, do nothing
	if(SelectionUtil::Instance()->HasSelection())
	{
		// Pass values to property
		bbox boundingBox = SelectionUtil::CalculateGroupBox(SelectionUtil::Instance()->GetSelectedEntityIds());
		Ptr<Game::Entity> camera_entity = PlacementUtil::Instance()->GetCameraEntity();
		Ptr<GraphicsFeature::MayaCameraProperty> cameraProperty = camera_entity->FindProperty(GraphicsFeature::MayaCameraProperty::RTTI).downcast<GraphicsFeature::MayaCameraProperty>();
		cameraProperty->SetCameraFocus( boundingBox.center(), boundingBox.diagonal_size() );
	}	
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::CenterOnSelection()
{
	Util::Array<EntityGuid> entities = SelectionUtil::Instance()->GetSelectedEntityIds();

	// If none is selected, do nothing
	if( entities.IsEmpty() )
	{
		return;
	}

	Math::bbox boundingBox = SelectionUtil::CalculateGroupBox(entities);
	Math::point centerOfInterest = boundingBox.center();
	
	// Pass it to camera property
	Ptr<Game::Entity> camera_entity =  PlacementUtil::Instance()->GetCameraEntity();
	Ptr<GraphicsFeature::MayaCameraProperty> cameraProperty = camera_entity->FindProperty(GraphicsFeature::MayaCameraProperty::RTTI).downcast<GraphicsFeature::MayaCameraProperty>();
	cameraProperty->SetCameraFocus( centerOfInterest );
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::AddNavArea()
{
    Util::String errorMessage;
    EntityGuid entityId;
    ActionManager::Instance()->CreateEntity(NavMeshArea, "EditorNavAreaMarker", "EditorNavAreaMarker", entityId, errorMessage);    
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::CreateNavMesh()
{
	if(!SelectionUtil::Instance()->HasSelection())
	{
		return;
	}

	Util::String errorMessage;
	EntityGuid entityId;
	if (!ActionManager::Instance()->CreateEntity(NavMesh, "NavMesh", "NavMesh", entityId, errorMessage))
	{		
		return;
	}

    Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();

	Util::Array<EntityGuid> ids;
	ids.Append(entityId);
	LevelEditor2App::Instance()->GetCurrentStateHandler().cast<LevelEditor2::LevelEditorState>()->UpdateSelection(ids);

    Ptr<LevelEditor2::CreateNavMesh> msg = LevelEditor2::CreateNavMesh::Create();
    msg->SetEntities(ents);
    LevelEditor2EntityManager::Instance()->GetEntityById(entityId)->SendSync(msg.cast<Messaging::Message>());
    
	return;	
}

//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::UpdateNavMesh()
{
    if(!SelectionUtil::Instance()->HasSelection())
    {
        return;
    }
    Util::Array<Ptr<Game::Entity>> ents = SelectionUtil::Instance()->GetSelectedEntities();
    if(ents.Size() > 1)
    {
        return;
    }
    if(ents[0]->AcceptsMessage(LevelEditor2::UpdateNavMesh::Id))
    {
        Ptr<LevelEditor2::UpdateNavMesh> msg = LevelEditor2::UpdateNavMesh::Create();
        ents[0]->SendSync(msg.cast<Messaging::Message>());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2App::ScanScripts()
{
	if (IO::IoServer::Instance()->DirectoryExists("toolkit:data/leveleditor/scripts"))
	{
		Util::Array<Util::String> files = IO::IoServer::Instance()->ListFiles("toolkit:data/leveleditor/scripts", "*.lua", true);
		for (int i = 0; i < files.Size(); i++)
		{
			Util::String script = IO::IoServer::Instance()->ReadFile(files[i]);
			if (Scripting::ScriptServer::Instance()->ScriptHasFunction(script, "__property_init"))
			{
				Scripting::ScriptServer::Instance()->Eval(script);
				Scripting::ScriptServer::Instance()->Eval("__property_init()");
			}
			if (Scripting::ScriptServer::Instance()->ScriptHasFunction(script, "__init"))
			{
				Scripting::ScriptServer::Instance()->Eval(script);
				Scripting::ScriptServer::Instance()->Eval("__init()");
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2App::RegisterPropertyCallback(const Util::String & propertyClass, const Util::String & displayName, const Util::String & scriptFunc)
{
	PropertyCallbackEntry entry = { displayName, scriptFunc };
	if (!this->propertyCallbacks.Contains(propertyClass))
	{
		Util::Array<PropertyCallbackEntry> props;		
		props.Append(entry);
		this->propertyCallbacks.Add(propertyClass, props);
	}
	else
	{
		this->propertyCallbacks[propertyClass].Append(entry);
	}
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<LevelEditor2App::PropertyCallbackEntry> &
LevelEditor2App::GetPropertyCallbacks(const Util::String& propertyClass)
{	
	return this->propertyCallbacks[propertyClass];
}

//------------------------------------------------------------------------------
/**
*/
bool
LevelEditor2App::HasPropertyCallbacks(const Util::String& propertyClass)
{
	return this->propertyCallbacks.Contains(propertyClass);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2App::PropertCallback()
{
	QPushButton * box = (QPushButton*)QObject::sender();
	Util::String script = box->property("script").toString().toLatin1().constData();
	Util::String exec;
	exec.Format(script.AsCharPtr(), box->property("entity").toUInt());
	Scripting::ScriptServer::Instance()->Eval(exec);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2App::RegisterScript(const Util::String & displayName, const Util::String & scriptFunc)
{
	QAction * action = this->editorWindow->GetUi().menu_Scripts->addAction(displayName.AsCharPtr());
	this->scriptCallbacks.Add(action, scriptFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
LevelEditor2App::ScriptAction(QAction* action)
{
	n_assert(this->scriptCallbacks.Contains(action));
	Scripting::ScriptServer::Instance()->Eval(this->scriptCallbacks[action]);
	if (Scripting::ScriptServer::Instance()->HasError())
	{
		n_status("Scrip action error: %s\n", Scripting::ScriptServer::Instance()->GetError().AsCharPtr());
	}
}

} // namespace LevelEditor2
//------------------------------------------------------------------------------