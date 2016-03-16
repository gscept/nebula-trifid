#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UIFeatureUnit
    
    The UI feature unit is responsible for keeping track of the currently loaded layouts.
    Layouts are also created through this feature unit.

    The feature unit is also responsible for continously update the UI subsystem through its perframe callback.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/featureunit.h"
#include "ui/uiserver.h"
#include "uieventhandler.h"
#include "base/uiplugin.h"
#include "uielementgroup.h"

//------------------------------------------------------------------------------
namespace UI
{

class UiInputHandler;
class UiRTPlugin;
class UiFeatureUnit : public Game::FeatureUnit    
{
    __DeclareClass(UiFeatureUnit);
    __DeclareSingleton(UiFeatureUnit);   

public:
    /// constructor
    UiFeatureUnit();
    /// destructor
    virtual ~UiFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();

    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();

	/// get flag for rendering debug information
	bool HasRenderDebug() const;    
	/// start render debug
	virtual void StartRenderDebug();
	/// stop render debug
	virtual void StopRenderDebug();
	
	/// Register a layout object
    Ptr<UI::UiLayout> CreateLayout(const Util::String& layoutId, const Resources::ResourceId& resId);
	/// Unload a layout object
	void FreeLayout(const Util::String& layoutId);
	/// get layout using name
	const Ptr<UI::UiLayout>& GetLayout(const Util::String& layoutId);
	/// set visibility of layout
	void SetLayoutVisible(const Util::String & layoutid, bool visible);
	/// layout is loaded
	bool HasLayout(const Util::String& layoutId) const;	
	/// Set the application specific gui logic
	void SetUIEventHandler(const Ptr<UiEventHandler>& handler);
    /// process event
    void ProcessEvent(const UiEvent& event);
	/// load Font
	void LoadFont(const Util::String& resource, const Util::String& family, UI::FontStyle style, UI::FontWeight weight);
	/// load font and try to autodetect information about it
	void LoadFont(const Util::String& resource);
	/// load all font files (with autodetect) within a folder hierarchy
	void LoadAllFonts(const Util::String & folder);
	/// enable auto load of resources on startup, default is true
	void SetAutoload(bool enable);
	/// Load UI fonts and layouts from database
	void LoadUITables();

    /// set loading screen resource
    void SetLoadingScreenFilename(const Util::String& res);
    /// has loading resource id
	bool HasLoadingScreenReourceId() const;

	/// register an additional ui plugin
	void RegisterUIRenderPlugin(const Ptr<UI::UiPlugin> & uiRt);
	/// deregister ui plugin again
	void UnregisterUIRenderPlugin(const Ptr<UI::UiPlugin> & uiRt);

    /// add group to dictionary
    void AddGroup(uint key, const Ptr<UiElementGroup>& group);
    /// remove group from dictionary
    void RemoveGroup(uint key, const Ptr<UiElementGroup>& group);
    /// get group by key
    const Ptr<UiElementGroup>& GetGroup(uint key) const;

	/// enable input handling
	void SetHandleInput(bool handleInput);
	/// 
	bool GetHandleInput();

	/// get a uielement
	static Ptr<UiElement> GetLayoutElement(const Util::String & layout, const Util::String & element);
	
	friend UI::UiRTPlugin;
protected:

	/// render plugins (called from ui rt plugin)
	void RenderPlugins(const Ptr<Frame::FrameBatch>& frameBatch);

    Ptr<UI::UiServer> server;	
	Ptr<UiEventHandler> uiEventHandler;
	Ptr<UI::UiInputHandler> inputHandler;
    Util::Dictionary<uint, Ptr<UiElementGroup>> groups;
	Util::Dictionary<Util::String, Util::String> uiScripts;
	
    Util::String loadingWindowFilename;
	Util::Array<Ptr<UI::UiPlugin>> plugins;	

	bool autoLoad;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
UiFeatureUnit::SetLoadingScreenFilename(const Util::String& res)
{
    this->loadingWindowFilename = res;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
UiFeatureUnit::HasLoadingScreenReourceId() const
{
    return this->loadingWindowFilename.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline bool
UiFeatureUnit::HasRenderDebug() const
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
UiFeatureUnit::HasLayout(const Util::String& layoutId) const
{
	return this->server->HasLayout(layoutId);
}

//------------------------------------------------------------------------------
/**
*/
inline void
UiFeatureUnit::StartRenderDebug() 
{
	this->server->SetRenderDebug(true);
}

//------------------------------------------------------------------------------
/**
*/
inline void
UiFeatureUnit::StopRenderDebug() 
{
	this->server->SetRenderDebug(false);
}


}; // namespace UI
//------------------------------------------------------------------------------