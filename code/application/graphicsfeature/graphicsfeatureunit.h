#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::GraphicsFeatureUnitUnit
    
    The GraphicsFeatures provides everything for rendering graphicentities
    from different views in different stages.

    For the default use it creates one default view and one default stage which are
    used in the graphic and camera properties.    
    The BaseGameFeatureUnit uses this features to build up a default graphic world.

    Additonal to the rendering the graphicsfeature creates and triggers 
    the input server.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file	
*/
#include "game/featureunit.h"
#include "graphics/graphicsinterface.h"
#include "graphics/graphicsserver.h"
#include "graphics/display.h"
#include "graphics/stage.h"
#include "graphics/view.h"
#include "graphics/cameraentity.h"
#include "input/inputserver.h"
#include "debugrender/debugshaperenderer.h"
#include "debugrender/debugtextrenderer.h"
#include "properties/pathanimproperty.h"
#include "posteffect/posteffectmanager.h"
#include "graphicsfeature/managers/attachmentmanager.h"
#include "debug/debugtimer.h"
#include "framesync/framesynctimer.h"
#include "resources/resourcemanager.h"
#include "util/blob.h"
#include "graphicsutil/animutil.h"
//#include "posteffect/posteffectmanager.h"
#include "gameanimeventhandler.h"
#include "resources/simpleresourcemapper.h"



//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class GraphicsFeatureUnit : public Game::FeatureUnit    
{
    __DeclareClass(GraphicsFeatureUnit);
    __DeclareSingleton(GraphicsFeatureUnit);   
public:
    /// constructor
    GraphicsFeatureUnit();
    /// destructor
    virtual ~GraphicsFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();           
	/// called from within GameServer::NotifySetupDefault() before the database is loaded
	virtual void OnBeforeLoad();
	/// called from within GameServer::NotifyCleanup() before shutting down a level
	virtual void OnBeforeCleanup();
	/// called from within GameServer::Load() after attributes are loaded
	virtual void OnLoad();
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();        
    /// called at the end of the feature trigger cycle
    virtual void OnEndFrame();
    /// start render debug
    virtual void StartRenderDebug();
    /// stop render debug
    virtual void StopRenderDebug();

	/// sets up display
	void SetupDisplay();
	/// destroys display
	void DiscardDisplay();

    /// create default stage, view and camera
    void SetupDefaultGraphicsWorld();  
    /// discard default graphics world
    void DiscardDefaultGraphicsWorld();
    /// get the default stage
    const Ptr<Graphics::Stage>& GetDefaultStage() const;
    /// get the default view
    const Ptr<Graphics::View>& GetDefaultView() const;
    /// get default camera entity
    const Ptr<Graphics::CameraEntity>& GetDefaultCamera() const;	
    /// get default game anim event handler
    const Ptr<GraphicsFeature::GameAnimEventHandler>& GetDefaultAnimEventHandler() const;
	/// get global light entity
	const Ptr<Graphics::GlobalLightEntity>& GetGlobalLightEntity() const;
	/// get world bounding box
	const Math::bbox& GetWorldBoundingBox() const;    
    /// on setup resource mappers
    void OnSetupResourceMappers();

	/// set main frame shader, only viable before running SetupDefaultGraphicsWorld()
	void SetFrameShader(const Resources::ResourceId& frameShader);
	/// set type of view to be used
	void SetViewClass(const Core::Rtti& type);
	/// sets display to render in full screen
	void SetFullscreen(bool b);
	/// gets fullscreen
	bool GetFullscreen() const;
    /// sets decoration flag
    void SetDecorated(bool b);
    /// sets allow resize
    void SetResizeable(bool b);
	/// sets the AA settings
	void SetAntiAliasing(const Util::String& aa);
	/// sets the parent window
	void SetWindowData(const Util::Blob& data);
	/// set whether or not the graphics feature unit should setup a default graphics world (true by default)
	void SetDefaultGraphicsWorld(bool b);
	/// updates the display
	void ApplyDisplaySettings();
	/// get reference to current display mode object
	CoreGraphics::DisplayMode& DisplayMode();

	/// create a new window and view
	Ptr<Graphics::View> CreateWindowAndView(const Util::StringAtom& viewName, const Util::StringAtom& frameShader, const Util::Blob& optWindowData = 0);
	
protected:
    /// called to configure display device
    virtual void OnConfigureDisplay();

	Resources::ResourceId frameShader;
    Ptr<Graphics::GraphicsInterface> graphicsInterface;
	Ptr<Resources::ResourceManager> resManager;
    Ptr<Graphics::Display> display;
	Ptr<FrameSync::FrameSyncTimer> frameSyncTimer;
    Util::Blob windowData;
    Ptr<Graphics::GraphicsServer> graphicsServer;
    Ptr<Graphics::CameraEntity> defaultCamera;	
    Ptr<Graphics::Stage> defaultStage;
    Ptr<Graphics::View> defaultView;
	Ptr<Graphics::GlobalLightEntity> globalLight;
    Ptr<Debug::DebugShapeRenderer> debugShapeRenderer;
    Ptr<Debug::DebugTextRenderer> debugTextRenderer;
	Math::bbox worldBoundingBox;	  
    Ptr<GraphicsFeature::AttachmentManager> attachmentManager;
    Ptr<GraphicsFeature::GameAnimEventHandler> animEventHandler;
	Ptr<EnvEntityManager> envEntityManager;
	Ptr<Resources::SimpleResourceMapper> animPathMapper;

	Core::Rtti & viewClass;

    bool decorated;
    bool resizable;
	bool defaultGraphicsWorld;
};


//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::View>&
GraphicsFeatureUnit::GetDefaultView() const
{
    return this->defaultView;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::Stage>&
GraphicsFeatureUnit::GetDefaultStage() const
{
    return this->defaultStage;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::CameraEntity>&
GraphicsFeatureUnit::GetDefaultCamera() const
{
    return this->defaultCamera;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::GlobalLightEntity>&
GraphicsFeatureUnit::GetGlobalLightEntity() const
{
	return this->globalLight;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<GraphicsFeature::GameAnimEventHandler>&
GraphicsFeatureUnit::GetDefaultAnimEventHandler() const
{
    return this->animEventHandler;
}
//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsFeatureUnit::SetFrameShader(const Resources::ResourceId& frameShader)
{
	n_assert(frameShader.IsValid());
	this->frameShader = frameShader;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsFeatureUnit::SetViewClass(const Core::Rtti& type)
{
	n_assert(type.IsDerivedFrom(Graphics::View::RTTI));
	this->viewClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GraphicsFeatureUnit::SetAntiAliasing( const Util::String& aa )
{
	this->display->Settings().SetAntiAliasQuality(CoreGraphics::AntiAliasQuality::FromString(aa));	
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GraphicsFeatureUnit::SetWindowData(const Util::Blob& data)
{
	this->windowData = data;	
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox& 
GraphicsFeatureUnit::GetWorldBoundingBox() const
{
	return this->worldBoundingBox;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsFeatureUnit::SetResizeable(bool b)
{
    this->resizable = b;
    if(this->display.isvalid())
    {
        this->display->Settings().SetResizable(b);    
    }    
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsFeatureUnit::SetDecorated(bool b)
{
    this->decorated = b;
    if(this->display.isvalid())
    {        
        this->display->Settings().SetDecorated(b);    
    }        
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsFeatureUnit::SetDefaultGraphicsWorld(bool b)
{
	this->defaultGraphicsWorld = b;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::DisplayMode&
GraphicsFeatureUnit::DisplayMode()
{
	return this->display->Settings().DisplayMode();
}
} // namespace GraphicsFeatureUnit
//------------------------------------------------------------------------------
