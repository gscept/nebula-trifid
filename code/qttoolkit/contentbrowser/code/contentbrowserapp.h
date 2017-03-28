#pragma once
//------------------------------------------------------------------------------
/**
    @class ContentBrowser::ContentBrowserApp
    
    Content browser entry point
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "application/appgame/gameapplication.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "qtfeature/qtfeatureunit.h"
#include "basegamefeature/statehandlers/gamestatehandler.h"
#include "basegamefeature/basegamefeatureunit.h"
#include "contentbrowserwindow.h"
#include "previewer/previewstate.h"
#include "physicsfeature/physicsfeatureunit.h"
#include "ui/uifeatureunit.h"
#include "materials/materialdatabase.h"
#include "actions/baseaction.h"
#include "qtaddons/remoteinterface/qtremoteclient.h"
#include "qtaddons/remoteinterface/qtremoteserver.h"
#include "posteffect/posteffectentity.h"
#include "projectinfo.h"
#include "qtaddons/splash/code/splash.h"
#include "posteffect/posteffectfeatureunit.h"
#include "effects/effectsfeatureunit.h"
#include "scriptingfeature/scriptingfeature.h"
#include "addons/dynui/imguiaddon.h"
#include "grid/gridaddon.h"
#include "inputfeature/inputfeatureunit.h"

namespace ContentBrowser
{
class ContentBrowserApp : public App::GameApplication
{
	__DeclareSingleton(ContentBrowserApp);
public:
	/// constructor
	ContentBrowserApp();
	/// destructor
	virtual ~ContentBrowserApp();

	/// opens the app
	bool Open();
	/// closes the app
	void Close();

	/// returns pointer to previewer state
	const Ptr<PreviewState>& GetPreviewState() const;
	/// returns pointer to post effect entity
	const Ptr<PostEffect::PostEffectEntity>& GetPostEffectEntity() const;

	/// returns pointer to window
	ContentBrowserWindow* GetWindow() const;

	/// pushes action onto undo stack
	void PushAction(const Ptr<Actions::BaseAction>& action);
	/// clears actions
	void ClearActions();
	/// undoes current action and puts it in redo stack
	void Undo();
	/// redoes current action and puts it in the undo stack
	void Redo();

private:
	/// setup application state handlers
	virtual void SetupStateHandlers();
	/// setup game features
	virtual void SetupGameFeatures();
	/// cleanup game features
	virtual void CleanupGameFeatures();

	Ptr<SplashAddon::Splash> splash;
	Ptr<MaterialDatabase> materialDatabase;
	Ptr<BaseGameFeature::BaseGameFeatureUnit> baseFeature;
	Ptr<PreviewState> previewState;
	Ptr<QtFeature::QtFeatureUnit> qtFeature;
	Ptr<GraphicsFeature::GraphicsFeatureUnit> graphicsFeature;
	Ptr<PhysicsFeature::PhysicsFeatureUnit> physicsFeature;
	Ptr<UI::UiFeatureUnit> uiFeature;
	Ptr<PostEffect::PostEffectFeatureUnit> postEffectFeature;
	Ptr<PostEffect::PostEffectEntity> postEffectEntity;
	Ptr<EffectsFeature::EffectsFeatureUnit> effectsFeature;
	Ptr<ScriptingFeature::ScriptingFeatureUnit> scriptFeature;
	Ptr<InputFeature::InputFeatureUnit> inputFeature;
	Ptr<Dynui::ImguiAddon> imgui;
	Ptr<Grid::GridAddon> grid;
	ContentBrowserWindow* browserWindow;

	Ptr<QtRemoteInterfaceAddon::QtRemoteServer> remoteServer;
	Ptr<QtRemoteInterfaceAddon::QtRemoteClient> remoteClient;
	Util::Stack<Ptr<Actions::BaseAction> > undoStack;
	Util::Stack<Ptr<Actions::BaseAction> > redoStack;
	ToolkitUtil::ProjectInfo projInfo;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PreviewState>&
ContentBrowserApp::GetPreviewState() const
{
	return this->previewState;
}


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<PostEffect::PostEffectEntity>& 
ContentBrowserApp::GetPostEffectEntity() const
{
	return this->postEffectEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline ContentBrowserWindow* 
ContentBrowserApp::GetWindow() const
{
	return this->browserWindow;
}


} // namespace ContentBrowser
//------------------------------------------------------------------------------