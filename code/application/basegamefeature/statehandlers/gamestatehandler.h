#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::GameStateHandler

    The GameStateHandler creates the basegamefeature and attached it 
    to the gameserver. It uses the basegamefeature to load a level,
    a savegame or setups a new game or just an empty world.

    The basegamefeature does the specific game logic stuff, 
    like handling entities with the entitymanager and factorymanager and so on.
    
    (C) 2003 RadonLabs GmbH
*/
#include "appgame/statehandler.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class GameStateHandler : public App::StateHandler
{
    __DeclareClass(GameStateHandler);
public:    
    /// setup modes
    enum SetupMode
    {
        EmptyWorld,
        NewGame,
        ContinueGame,
        LoadLevel,
		LoadNetworkedLevel,
        LoadSaveGame,
    };

    /// constructor
    GameStateHandler();
    /// destructor
    virtual ~GameStateHandler();

    /// called when the state represented by this state handler is entered
    virtual void OnStateEnter(const Util::String& prevState);
    /// called when the state represented by this state handler is left
    virtual void OnStateLeave(const Util::String& nextState);
    /// called each frame as long as state is current, return new state
    virtual Util::String OnFrame();

    /// set the setup mode
    void SetSetupMode(SetupMode mode);
    /// get the setup mode
    SetupMode GetSetupMode() const;   
    /// set level filename, required by setup mode LoadLevel
    void SetLevelName(const Util::String& n);
    /// get level name
    const Util::String& GetLevelName() const;
    /// set save game name, required by setup mode LoadSaveGame
    void SetSaveGame(const Util::String& n);
    /// get save game name
    const Util::String& GetSaveGame() const;
	/// networklevel loaded callback
	virtual void OnNetworkStarted();
    
private:    
    SetupMode setupMode;
    Util::String exitState;
    Util::String levelName;
    Util::String saveGame;	
};

//------------------------------------------------------------------------------
/**
*/
inline void
GameStateHandler::SetSetupMode(SetupMode mode)
{
    this->setupMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline GameStateHandler::SetupMode
GameStateHandler::GetSetupMode() const
{
    return this->setupMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GameStateHandler::SetLevelName(const Util::String& n)
{
    this->levelName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
GameStateHandler::GetLevelName() const
{
    return this->levelName;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GameStateHandler::SetSaveGame(const Util::String& n)
{
    this->saveGame = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
GameStateHandler::GetSaveGame() const
{
    return this->saveGame;
}
} // namespace BaseGameFeature
//------------------------------------------------------------------------------
