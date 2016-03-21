#pragma once
//------------------------------------------------------------------------------
/**
    @class App::GameApplication

    Nebula3's default game application. It creates and triggers the GameServer.
    For game features it creates the core and graphicsfeature which is used in every
    gamestate (such as level gamestates or only gui gamestates).

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file	
*/
#include "app/application.h"
#include "appgame/statehandler.h"
#include "game/gameserver.h"
#include "core/singleton.h"
#include "util/dictionary.h"
#include "util/stringatom.h"
#include "core/coreserver.h"
#include "debug/debuginterface.h"
#include "io/ioserver.h"
#include "io/iointerface.h"
#include "http/httpinterface.h"
#include "http/httpserverproxy.h"     
#include "remote/remoteinterface.h"
#include "remote/remotecontrolproxy.h"
#include "jobs/jobsystem.h"   
#include "io/gamecontentserver.h"
#include "resources/resourcemanager.h"
#include "http/httpclientregistry.h"

//------------------------------------------------------------------------------
namespace App
{
class GameApplication : public Application
{
    __DeclareSingleton(GameApplication);   

public:
    /// constructor
    GameApplication();
    /// destructor
    virtual ~GameApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
    /// run the application
    virtual void Run();

    /// add an application state handler
    void AddStateHandler(const Ptr<StateHandler>& state);
    /// find a state handler by name
    const Ptr<StateHandler>& FindStateHandlerByName(const Util::String& stateName) const;
    /// return pointer to current state handler
    const Ptr<StateHandler>& GetCurrentStateHandler() const;
    /// return state handler of current state
    const Util::String& GetCurrentState() const;
    /// get number of application states
    int GetNumStates() const;
    /// get state handler at index
    const Ptr<StateHandler>& GetStateHandlerAt(int index) const;
    /// request a new state which will be applied at the end of the frame
    void RequestState(const Util::String& stateName);

protected:
    /// setup application state handlers
    virtual void SetupStateHandlers();
    /// cleanup application state handlers
    virtual void CleanupStateHandlers(); 
    /// setup game features
    virtual void SetupGameFeatures();
    /// cleanup game features
    virtual void CleanupGameFeatures(); 
    /// perform a state transition
    virtual void DoStateTransition();
    /// set an application state
    void SetState(const Util::String& s);
    /// setup app from cmd lines
    virtual void SetupAppFromCmdLineArgs();

    Ptr<Core::CoreServer> coreServer;
    Ptr<Jobs::JobSystem> jobSystem;
    Ptr<IO::GameContentServer> gameContentServer;
    Ptr<IO::IoServer> ioServer;
    Ptr<IO::IoInterface> ioInterface;  
    
#if __NEBULA3_HTTP__
    Ptr<Debug::DebugInterface> debugInterface;
    Ptr<Http::HttpInterface> httpInterface;
    Ptr<Http::HttpServerProxy> httpServerProxy;

    Ptr<Remote::RemoteInterface> remoteInterface;
    Ptr<Remote::RemoteControlProxy> remoteControlProxy;
	ushort defaultTcpPort;
#endif

#if __NEBULA3_HTTP_FILESYSTEM__
	Ptr<Http::HttpClientRegistry> httpClientRegistry;
#endif      

    // state handlers
    Util::String requestedState;
    Util::String curState;
    Util::String nextState;
    Util::Dictionary<Util::StringAtom, Ptr<StateHandler> > stateHandlers;

    // game server
    Ptr<Game::GameServer> gameServer;

    // profiling
    _declare_timer(GameApplicationFrameTimeAll);
};
//------------------------------------------------------------------------------
/**
*/
inline int
GameApplication::GetNumStates() const
{
    return this->stateHandlers.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<StateHandler>&
GameApplication::GetStateHandlerAt(int index) const
{
    return this->stateHandlers.ValueAtIndex(index);
}

//------------------------------------------------------------------------------
/**
    Returns the currently active application state. Can be 0 if no valid 
    state is set.
*/
inline const Util::String&
GameApplication::GetCurrentState() const
{
    return this->curState;
}


} // namespace App
//------------------------------------------------------------------------------
