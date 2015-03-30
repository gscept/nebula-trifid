#pragma once
//------------------------------------------------------------------------------
/**
    @class Game::GameServer

    The game server setups and runs the game world, consisting of a number
    of active "game entities". Functionality and queries on the game world
    are divided amongst several Game Features.
    This keeps the game server's interface small and clean, and lets Mangalore 
    applications easily extend functionality by implementing new, or deriving 
    from existing game features.

    To add or replace FeatureUnit objects, derive from Game::FeatureUnit and 
    add your features on application start or gamestatehandler enter.

    The GameServer triggers all attached features. Start and Stop is called within 
    the gamestatehandler to allow all features do stuff after everything is loaded 
    and initialized. Load and Save is invoked from the BaseGameFeature which allows
    begining a new game, load or save a game.
    
    (C) 2007 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "math/bbox.h"
#include "appgame/appconfig.h"
#include "core/singleton.h"
#include "game/featureunit.h"
#include "debug/debugtimer.h"

//------------------------------------------------------------------------------
namespace Game
{
class Entity;
class Manager;

class GameServer : public Core::RefCounted
{
    __DeclareClass(GameServer);
    __DeclareSingleton(GameServer);
public:
    /// constructor
    GameServer();
    /// destructor
    virtual ~GameServer();

    /// open the game world
    virtual bool Open();
    /// close the game world
    virtual void Close();
    /// start the game world
    virtual bool Start();
    /// has the game world already started
    bool HasStarted() const;
    /// stop the game world
    virtual void Stop();
	/// trigger actions before rendering the game world
	virtual void OnBeginFrame();
    /// trigger the game world
    virtual void OnFrame();
	/// trigger actions after rendering the game world
	virtual void OnEndFrame();
   
    /// call OnLoad on all game features
    virtual void NotifyGameLoad();
    /// call OnSave on all game features
    virtual void NotifyGameSave();
        
    /// add game feature
    void AttachGameFeature(const Ptr<FeatureUnit>& feature);
    /// remove game feature
    void RemoveGameFeature(const Ptr<FeatureUnit>& feature);
    /// is feature attached
    bool IsFeatureAttached(const Util::String& stringName) const;

    /// is quit requested
    bool IsQuitRequested() const;
    /// request quit
    void SetQuitRequested();

protected:
	
	/// check input for render debug and return feature for rendering
	void CheckDebugRendering();

    bool isOpen;
    bool isStarted;
    bool quitRequested;		//< obsolete, see closeEvent() method
    Util::Array<Ptr<FeatureUnit> > gameFeatures;
	Ptr<FeatureUnit> debugRenderFeature;

	_declare_timer(GameServerOnBeginFrame);
    _declare_timer(GameServerOnFrame);
	_declare_timer(GameServerOnEndFrame);
};

//------------------------------------------------------------------------------
/**
*/
inline bool 
GameServer::IsQuitRequested() const
{
    return this->quitRequested;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
GameServer::SetQuitRequested()
{
    this->quitRequested = true;
}

}; // namespace Game
//------------------------------------------------------------------------------


