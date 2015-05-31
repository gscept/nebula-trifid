//------------------------------------------------------------------------------
//  leveleditor2app.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
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
#include "leveleditor2protocol.h"
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
#include "style/graypalette.h"
#include <QPlastiqueStyle>



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

    // create and attach qt feature
    this->qtFeature = QtFeature::QtFeatureUnit::Create();
    this->gameServer->AttachGameFeature(this->qtFeature.upcast<Game::FeatureUnit>());    

    // create palette
    QtToolkitUtil::GrayPalette palette;
    QApplication::setPalette(palette);
    QApplication::setStyle(new QPlastiqueStyle);

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

    // add arguments for embedding the Nebula context in the desired Nebula frame
    String extraArgs;
    extraArgs.Format("-embedded");
    CommandLineArgs args(extraArgs);
    this->args.AppendCommandString(extraArgs);
    this->graphicsFeature->SetCmdLineArgs(this->GetCmdLineArgs());
    this->graphicsFeature->SetWindowData(this->editorWindow->GetNebulaWindowData());
	this->graphicsFeature->SetupDisplay();
    this->gameServer->AttachGameFeature(this->graphicsFeature.upcast<Game::FeatureUnit>());

    // setup base game feature
    this->baseFeature = Toolkit::EditorBaseGameFeatureUnit::Create();	
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

    // register input proxy for inputs from nebula to the qt app
    this->qtFeature->RegisterQtInputProxy(this->editorWindow);	

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
	this->editorState = 0;

	GameApplication::CleanupGameFeatures();
}


//------------------------------------------------------------------------------
/**
*/
void 
LevelEditor2App::DuplicateCurrentSelection()
{
	Util::Array<Ptr<Game::Entity>> entities = SelectionUtil::Instance()->GetSelectedEntities();		
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
		for(IndexT i = 0 ; i < entities.Size() ; i++)
		{
			if(!entities[i]->GetGuid(Attr::ParentGuid).IsValid())
			{
				entities[i]->SetGuid(Attr::ParentGuid,realId);
			}				
		}
		LevelEditor2App::Instance()->GetWindow()->GetEntityTreeWidget()->RebuildTree();
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
		bbox boundingBox = SelectionUtil::Instance()->GetGroupBox(); // bad, using the group box since it will be updated even if grouping isn't active
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
	Util::Array< Ptr<Game::Entity>> entities = SelectionUtil::Instance()->GetSelectedEntities();

	// If none is selected, do nothing
	if( entities.IsEmpty() )
	{
		return;
	}

	Math::point centerOfInterest;
	if( entities.Size() > 1 || SelectionUtil::Instance()->InGroupMode() )
	{
		centerOfInterest = SelectionUtil::Instance()->GetGroupBox().center();
	}
	else
	{
		centerOfInterest = entities[0]->GetMatrix44(Attr::Transform).get_position();
	}

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

} // namespace LevelEditor2
//------------------------------------------------------------------------------