#pragma once
//------------------------------------------------------------------------------
/**
    @class LevelEditor2::LevelEditor2App
    
    Level editor entry point
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#include "application/appgame/gameapplication.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "basegamefeature/statehandlers/gamestatehandler.h"
#include "basegamefeature/basegamefeatureunit.h"
#include "qtfeature/qtfeatureunit.h"
#include "widgets/leveleditor2window.h"
#include "leveleditorstate.h"
#include "remoteinterface/qtremoteclient.h"
#include "remoteinterface/qtremoteserver.h"
#include "physicsfeature/physicsfeatureunit.h"
#include "stateobjectfeature/stateobjectfeatureunit.h"
#include "editorfeatures/editorbasegamefeature.h"
#include "editorfeatures/editorblueprintmanager.h"
#include "level.h"
#include "scriptingfeature/scriptingfeature.h"
#include "navigation/navigationserver.h"
#include "projectinfo.h"
#include "attr/io/attrcontainerxmlstorage.h"
#include "dynui/imguiaddon.h"
#include "grid/gridaddon.h"
#include "splash/code/splash.h"
#include "lighting/lightprobemanager.h"
#include "silhouette/silhouetteaddon.h"
#include "posteffect/posteffectfeatureunit.h"
#include "logger.h"
#include "navigationfeatureunit.h"
#include "inputfeature/inputfeatureunit.h"


//------------------------------------------------------------------------------
namespace LevelEditor2
{

class LevelEditor2App : public QObject, public App::GameApplication
{
    __DeclareSingleton(LevelEditor2App);
    Q_OBJECT
public:

	struct PropertyCallbackEntry
	{
		Util::String displayName;
		Util::String scriptFunction;
	};

    /// constructor
    LevelEditor2App();
    /// destructor
    virtual ~LevelEditor2App();

    /// opens the app
    virtual bool Open();
	/// close the app
	virtual void Exit();
    /// closes the app
    void Close();

    /// get the qt window
    LevelEditor2Window* GetWindow() const;
	/// get game state object
	Ptr<LevelEditorState> GetEditorState();

    /// returns pointer to post effect entity
    const Ptr<PostEffect::PostEffectEntity>& GetPostEffectEntity() const;	

	/// get logger object
	ToolkitUtil::Logger * GetLogger();

	///
	void RegisterPropertyCallback(const Util::String & propertyClass, const Util::String & displayName, const Util::String & scriptFunc);
	///
	void RegisterScript(const Util::String & displayName, const Util::String & scriptFunc);
	///
	const Util::Array<PropertyCallbackEntry> & GetPropertyCallbacks(const Util::String& propertyClass);
	///
	bool HasPropertyCallbacks(const Util::String& propertyClass);

public slots:
    /// toggle translate mode
    void ToggleTranslateFeature();
    /// toggle rotate mode
    void ToggleRotateFeature();
    /// toggle scale mode
    void ToggleScaleFeature();

    /// duplicate selection
    void DuplicateCurrentSelection();
    /// group current selection
    void GroupSelection();

    /// focus camera on selection
    void FocusOnSelection();
    /// center camera on selection
    void CenterOnSelection();

    /// FIXME
    void CreateNavMesh();
	///
    void UpdateNavMesh();
    /// add navmesh area marker
    void AddNavArea();
	///
	void PropertCallback();

    /// access to global attributes container
    const Ptr<Attr::AttrContainerXMLStorage> & GetGlobalAttrs() const;	

	///
	void ScriptAction(QAction*);

private:
    /// setup application state handlers
    void SetupStateHandlers();
    /// cleanup application state handlers
    void CleanupStateHandlers();
    /// setup game features
    void SetupGameFeatures();
    /// cleanup game features
    void CleanupGameFeatures();    
	///
	void ScanScripts();
	
    Ptr<QtRemoteInterfaceAddon::QtRemoteServer> remoteServer;
    Ptr<QtRemoteInterfaceAddon::QtRemoteClient> remoteClient;
    Ptr<QtRemoteInterfaceAddon::QtRemoteClient> viewerClient;

	Ptr<LightProbeManager> lightProbeManager;
	Ptr<SplashAddon::Splash> splash;
    Ptr<LevelEditorState> editorState;
	Ptr<Dynui::ImguiAddon> imgui;
	Ptr<Grid::GridAddon> grid;
	Ptr<Silhouette::SilhouetteAddon> silhouette;
    Ptr<QtFeature::QtFeatureUnit> qtFeature;
    Ptr<Toolkit::EditorBaseGameFeatureUnit> baseFeature;
    Ptr<GraphicsFeature::GraphicsFeatureUnit> graphicsFeature;
    Ptr<PhysicsFeature::PhysicsFeatureUnit> physicsFeature;
    Ptr<Toolkit::EditorBlueprintManager> blueprintManager;
    Ptr<ScriptingFeature::ScriptingFeatureUnit> scriptingFeature;
    Ptr<PostEffect::PostEffectEntity> postEffectEntity;
	Ptr<PostEffect::PostEffectFeatureUnit> postEffectFeature;
    Ptr<Navigation::NavigationFeatureUnit> navigationFeature;
	Ptr<InputFeature::InputFeatureUnit> inputFeature;
    Ptr<LevelEditor2::Level> level;
    LevelEditor2Window* editorWindow;
    Ptr<Navigation::NavigationServer> navigation;
    ToolkitUtil::ProjectInfo projInfo;
    Ptr<Attr::AttrContainerXMLStorage> globalAttrs;
	ToolkitUtil::Logger logger;
	Util::HashTable < Util::String, Util::Array<PropertyCallbackEntry>> propertyCallbacks;
	Util::Dictionary<QAction*, Util::String> scriptCallbacks;
}; 

//------------------------------------------------------------------------------
/**
*/
inline LevelEditor2Window* 
LevelEditor2App::GetWindow() const
{
	return this->editorWindow;
}

//------------------------------------------------------------------------------
/**
*/
inline Ptr<LevelEditorState>
LevelEditor2App::GetEditorState()
{
	return this->editorState;
}
//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PostEffect::PostEffectEntity>& 
LevelEditor2App::GetPostEffectEntity() const
{
	return this->postEffectEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Attr::AttrContainerXMLStorage>& 
LevelEditor2App::GetGlobalAttrs() const
{
    return this->globalAttrs;
}

//------------------------------------------------------------------------------
/**
*/
inline ToolkitUtil::Logger*
LevelEditor2App::GetLogger()
{
	return &this->logger;
}
} // namespace LevelEditor2
//------------------------------------------------------------------------------