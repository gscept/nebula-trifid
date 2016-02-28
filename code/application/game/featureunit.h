#pragma once
//------------------------------------------------------------------------------
/**
    @class Game::FeatureUnit

    A FeatureUnit is an encapsulated feature which can be
    added to an application.
    E.g. game features can be core features of Nebula3 like Render or Network, 
    or it can be some of the addons like db or physics.

    To add a new feature, derive from this class and add it to 
    the Game::GameServer on application or statehandler startup.

    The Game::GameServer will start, load, save, trigger and close your feature.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "game/manager.h"
#include "core/singleton.h"
#include "util/commandlineargs.h"

//------------------------------------------------------------------------------
namespace Game
{

class FeatureUnit : public Core::RefCounted    
{
    __DeclareClass(FeatureUnit);
public:
    /// constructor
    FeatureUnit();
    /// destructor
    virtual ~FeatureUnit();

    /// called from GameServer::AttachGameFeature()
    virtual void OnActivate();
    /// called from GameServer::RemoveGameFeature()
    virtual void OnDeactivate();
    /// return true if property is currently active
    bool IsActive() const;
    
    /// called from within GameServer::Load() after attributes are loaded
    virtual void OnLoad();
    /// called from within GameServer::OnStart() after OnLoad when the complete world exist
    virtual void OnStart();
    /// called from within GameServer::Save() before attributes are saved back to database
    virtual void OnSave();
    
	/// called from within GameServer::NotifyBeforeLoad() before the database is loaded
	virtual void OnBeforeLoad();
	/// called from within GameServer::NotifyBeforeCleanup() before shutting down a level
	virtual void OnBeforeCleanup();

    /// called on begin of frame
    virtual void OnBeginFrame();    
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();
    /// called at the end of the feature trigger cycle
    virtual void OnEndFrame();

    /// start render debug
    virtual void StartRenderDebug();
    /// called when game debug visualization is on
    virtual void OnRenderDebug();
    /// stop render debug
    virtual void StopRenderDebug();

    /// attach a manager to the game world
    virtual void AttachManager(const Ptr<Manager>& manager);
    /// remove a manager from the game world
    virtual void RemoveManager(const Ptr<Manager>& manager);

    /// set command line args
    void SetCmdLineArgs(const Util::CommandLineArgs& a);
    /// get command line args
    const Util::CommandLineArgs& GetCmdLineArgs() const;
        
	/// set flag for rendering debug information
	void SetRenderDebug(bool b);
	/// get flag for rendering debug information
	bool HasRenderDebug() const;    

protected:

    Util::Array<Ptr<Manager> > managers;
	bool active;
	bool renderDebug;

    // cmdline args for configuration from cmdline
    Util::CommandLineArgs args;
    
    #if __MANAGERS_STATS__
    Util::Array<nProfiler> managerProfilers;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline void
FeatureUnit::SetRenderDebug(bool b)
{
	this->renderDebug = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FeatureUnit::HasRenderDebug() const
{
	return this->renderDebug;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FeatureUnit::IsActive() const
{
    return this->active;
}

//------------------------------------------------------------------------------
/**
*/
inline void
FeatureUnit::SetCmdLineArgs(const Util::CommandLineArgs& a)
{
    this->args = a;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::CommandLineArgs&
FeatureUnit::GetCmdLineArgs() const
{
    return this->args;
}

} // namespace FeatureUnit
//------------------------------------------------------------------------------
