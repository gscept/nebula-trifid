#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::GraphicsServer
    
    The graphics server maintains a the "graphics world" consisting of 
    one or more "stages" and one or more "views" which are attached to
    the stages.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/stringatom.h"
#include "util/array.h"
#include "math/line.h"
#include "math/float2.h"
#include "timing/time.h"
#include "coregraphics/shadervariable.h"
#include "graphics/graphicsentity.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "rendermodules/rendermodule.h"
#include "debug/debugtimer.h"
#include "visibility/visibilitysystems/visibilitysystembase.h"
#include "globallightentity.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class GlobalLightEntity;
class Stage;
class View;
class StageBuilder;
    
class GraphicsServer : public Core::RefCounted
{
    __DeclareClass(GraphicsServer);
    __DeclareSingleton(GraphicsServer);
public:
    /// constructor
    GraphicsServer();
    /// destructor
    virtual ~GraphicsServer();
    
    /// open the graphics server
    void Open();
    /// close the graphics server
    void Close();
    /// return true if graphics server is open
    bool IsOpen() const;
    
    /// create a stage object
    Ptr<Stage> CreateStage(const Util::StringAtom& name, const Util::Array<Ptr<Visibility::VisibilitySystemBase> >& visSystems);
    /// discard a stage object
    void DiscardStage(const Ptr<Stage>& stage);
    /// discard all stage objects
    void DiscardAllStages();
    /// return true if a stage exists by name
    bool HasStage(const Util::StringAtom& name) const;
    /// lookup a stage by name
    const Ptr<Stage>& GetStageByName(const Util::StringAtom& name) const;
    /// get all stages
    const Util::Array<Ptr<Stage> >& GetStages() const;

    /// create a view object
    Ptr<View> CreateView(const Core::Rtti& viewClass, const Util::StringAtom& name, const IndexT windowId, bool isDefaultView=false, bool updatePerFrame=true);    
    /// discard a view object
    void DiscardView(const Ptr<View>& view);
    /// discard all view objects
    void DiscardAllViews();
    /// return true if a view exists by name
    bool HasView(const Util::StringAtom& name) const;
    /// lookup a view by name
    const Ptr<View>& GetViewByName(const Util::StringAtom& name) const;
    /// get all views
    const Util::Array<Ptr<View> >& GetViews() const;
    /// set the default view
    void SetDefaultView(const Ptr<View>& defView);
    /// get the default view
    const Ptr<View>& GetDefaultView() const;
    /// gets the current view (this is the view which is currently rendering)
    const Ptr<View>& GetCurrentView() const;

    /// return true if an entity exists by its unique id
    bool HasEntity(GraphicsEntity::Id id) const;
    /// lookup an entity by its unique id
    const Ptr<GraphicsEntity>& GetEntityById(GraphicsEntity::Id id) const;
    /// get all entities
    const Util::Array<Ptr<GraphicsEntity> >& GetEntities() const;

    /// register a render module
    void RegisterRenderModule(const Ptr<RenderModules::RenderModule>& renderModule);
    /// unregister a render module
    void UnregisterRenderModule(const Ptr<RenderModules::RenderModule>& renderModule);
    /// get array of all currently registered render modules
    const Util::Array<Ptr<RenderModules::RenderModule> >& GetRenderModules() const;
    
    /// call per-frame, this renders the default view
    void OnFrame(Timing::Time curTime, Timing::Time globalTimeFactor);
    /// set render debug flag
    void SetRenderDebug(bool b);
    /// get render debug flag
    bool GetRenderDebug() const;

    /// gets the current global light entity (may return INVALID ptr)
    Ptr<Graphics::GlobalLightEntity> GetCurrentGlobalLightEntity() const;

private:
    friend class GraphicsEntity;

    /// register a graphics entity
    void RegisterEntity(const Ptr<GraphicsEntity>& entity);
    /// unregister a graphics entity
    void UnregisterEntity(const Ptr<GraphicsEntity>& entity);

    Ptr<RenderModules::RTPluginRegistry> rtPluginRegistry;
    Util::Array<Ptr<RenderModules::RenderModule> > renderModules;
    Util::Array<Ptr<GraphicsEntity> > entities;
    Util::Dictionary<GraphicsEntity::Id, IndexT> entityIndexMap;
    Util::Array<Ptr<Stage> > stages;                         
    Util::Dictionary<Util::StringAtom, IndexT> stageIndexMap; 
    Util::Array<Ptr<View> > views;
    Util::Dictionary<Util::StringAtom, IndexT> viewIndexMap;
    Ptr<View> defaultView;
    Ptr<View> currentView;
    Ptr<CoreGraphics::ShaderVariable> timeShaderVar;
    bool isOpen;
    bool renderDebug;
	_declare_timer(GfxServerRenderViews);
	_declare_timer(GfxServerPresentFrame);
};

//------------------------------------------------------------------------------
/**
*/
inline bool
GraphicsServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GraphicsServer::SetRenderDebug(bool b)
{
    this->renderDebug = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
GraphicsServer::GetRenderDebug() const
{
    return this->renderDebug;
}


//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<GraphicsEntity> >&
GraphicsServer::GetEntities() const
{
    return this->entities;
}


//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<RenderModules::RenderModule> >&
GraphicsServer::GetRenderModules() const
{
    return this->renderModules;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<View>& 
GraphicsServer::GetCurrentView() const
{
    return this->currentView;
}

} // namespace Graphics
//------------------------------------------------------------------------------

