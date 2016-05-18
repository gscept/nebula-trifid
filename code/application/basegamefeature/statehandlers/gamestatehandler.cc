//------------------------------------------------------------------------------
//  application/statehandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "appgame/gameapplication.h"
#include "game/gameserver.h"
#include "basegamefeature/basegamefeatureunit.h"
#include "basegamefeature/statehandlers/gamestatehandler.h"
#include "multiplayer/multiplayerfeatureunit.h"
#include "multiplayer/replicationmanager.h"

namespace BaseGameFeature
{
__ImplementClass(BaseGameFeature::GameStateHandler, 'AGST', App::StateHandler);

using namespace Game;
using namespace App;

//------------------------------------------------------------------------------
/**
*/
GameStateHandler::GameStateHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GameStateHandler::~GameStateHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called when the state associated with this state handler
    is entered. The parameter indicates the previously active state.

    @param  prevState   previous state
*/
void
GameStateHandler::OnStateEnter(const Util::String& prevState)
{         
    BaseGameFeatureUnit::Instance()->SetRenderDebug(true);
    
    // setup the game
    switch (this->setupMode)
    {
        case EmptyWorld:
            BaseGameFeatureUnit::Instance()->SetupEmptyWorld();
            break;

        case NewGame:   
            BaseGameFeatureUnit::Instance()->NewGame();
            break;

        case ContinueGame:
            BaseGameFeatureUnit::Instance()->ContinueGame();
            break;

        case LoadLevel:
            BaseGameFeatureUnit::Instance()->LoadLevel(this->GetLevelName());
            break;

		case LoadNetworkedLevel:
			n_assert2(MultiplayerFeature::MultiplayerFeatureUnit::HasInstance(), "No multiplayerfeature singleton exists");
			BaseGameFeatureUnit::Instance()->LoadNetworkedLevel(this->GetLevelName());			
			break;

        case LoadSaveGame:
            BaseGameFeatureUnit::Instance()->LoadGame(this->GetSaveGame());
            break;

        default:
            n_error("GameStateHandler: Unkown setup mode !!!");
    }

    // clear the startup level and save game name
    this->SetLevelName("");
    this->SetSaveGame("");

    // start game world
	Game::GameServer::Instance()->Start();
}

//------------------------------------------------------------------------------
/**
    This method is called when the state associated with this state handler
    is left. The parameter indicates the next active state.

    @param  nextState   next state
*/
void
GameStateHandler::OnStateLeave(const Util::String& nextState)
{
    BaseGameFeatureUnit::Instance()->CleanupWorld();

    // stop game world
    Game::GameServer::Instance()->Stop();
}

//------------------------------------------------------------------------------
/**
    This method is called once a frame while the state is active. The method
    must return a state identifier. If this is different from the current
    state, a state switch will occur after the method returns. 

    @return     a state identifier
*/
Util::String
GameStateHandler::OnFrame()
{	
    if (Game::GameServer::Instance()->IsQuitRequested())
    {
        return "Exit";
    }
    else
    {
        return this->GetName();
    }
}

//------------------------------------------------------------------------------
/**
	Called once all clients loaded the level into memory
*/
void
GameStateHandler::OnNetworkStarted()
{
	// empty
}

} // namespace Application