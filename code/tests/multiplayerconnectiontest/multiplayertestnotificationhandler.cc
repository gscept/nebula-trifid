//------------------------------------------------------------------------------
//  network/multiplayer/multiplayertestnotificationhandler.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "multiplayertestnotificationhandler.h"
#include "multiplayer/multiplayermanager.h"
#include "multiplayerconnectiontest.h"

namespace Test
{
__ImplementClass(MultiplayerTestNotificationHandler, 'DINH', Multiplayer::DefaultMultiplayerNotificationHandler);

using namespace Multiplayer;

uint MultiplayerTestNotificationHandler::playerLeftCounter = 0;       
uint MultiplayerTestNotificationHandler::reconnections = 0;

//------------------------------------------------------------------------------
/**
*/
MultiplayerTestNotificationHandler::MultiplayerTestNotificationHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MultiplayerTestNotificationHandler::~MultiplayerTestNotificationHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandleNotification(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(PlayerSignInChanged::Id))
    {
        this->HandlePlayerSignInStateChanged(msg.cast<PlayerSignInChanged>());
    }
    else if (msg->CheckId(PlayerJoinedSession::Id))
    {
        this->HandlePlayerJoinedSession(msg.cast<PlayerJoinedSession>());
    }
    else if (msg->CheckId(PlayerLeftSession::Id))
    {
        this->HandlePlayerLeftSession(msg.cast<PlayerLeftSession>());
    }
    else if (msg->CheckId(GameStarted::Id))
    {
        this->HandleGameStarted(msg.cast<GameStarted>());
    }
    else if (msg->CheckId(GameEnded::Id))
    {
        this->HandleGameEnded(msg.cast<GameEnded>());
    }
    else if (msg->CheckId(PlayerReadyChanged::Id))
    {
        this->HandlePlayerReadyChanged(msg.cast<PlayerReadyChanged>());
    }
    else if (msg->CheckId(DataReceived::Id))
    {
        this->HandleDataReceived(msg.cast<DataReceived>());
    }
    else if (msg->CheckId(DisconnectFromServer::Id))
    {
        this->HandleDisconnectionFromServer();
    }
    else if (msg->CheckId(GameStarting::Id))
    {
        MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::SessionStarting);   
        this->HandleGameStarting(msg.cast<GameStarting>());
    }
    else if (msg->IsA(ReturnNotification::RTTI))
    {
        if (msg->CheckId(CreatePlayersSucceeded::Id))
        {                            
            n_printf("Logged In!\n");
            MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::LoggedIn);
        }
        else if (msg->CheckId(StartGameSearchFinished::Id))
        {             
            n_printf("Gamesearch finished!\n");
            MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::GameSearchFinished);   
        }
        else if (msg->CheckId(JoinSessionSucceeded::Id))
        {            
            n_printf("Session joined!\n");
            MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::Joined);
        }  
        else if (msg->CheckId(HostSessionSucceeded::Id))
        {             
            n_printf("Session created!\n");
            MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::HostSession);
        }   
        else if (msg->CheckId(StartGameSucceeded::Id))
        {               
            n_printf("Session started!\n");
            MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::SessionStarted);     
        }   
        else if (msg->CheckId(EndGameSucceeded::Id))
        {               
            // delete or leave session
            if (Multiplayer::MultiplayerManager::Instance()->GetGameFunction() == MultiplayerManager::HostingGame)
            {
                Multiplayer::MultiplayerManager::Instance()->DeleteSession();  
                n_printf("Session Deleting ... \n");                 
            }
            else 
            {
                Multiplayer::MultiplayerManager::Instance()->LeaveSession(0);  
                n_printf("Session Leaving ... \n");                 
            }       
            MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::SessionLeaving); 
        }   
        else if (msg->CheckId(DeleteSessionSucceded::Id)
            || msg->CheckId(LeaveSessionSucceeded::Id))
        {                                                                                          
            MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::LoggedIn); 
        }   
        
        // always pass return notifiction to base class 
        DefaultMultiplayerNotificationHandler::HandleNotification(msg);  
    }                                                    
    else
    {
        DefaultMultiplayerNotificationHandler::HandleNotification(msg);
    }
}
//------------------------------------------------------------------------------
/**
*/
void
MultiplayerTestNotificationHandler::HandlePlayerLeftSession(const Ptr<PlayerLeftSession>& msg)
{
    const Ptr<Player>& player = MultiplayerManager::Instance()->GetPlayer(msg->GetPlayerHandle());
    n_printf("Player %s left!\n", player->GetGamerTag().AsString().AsCharPtr());
    playerLeftCounter++;

    // send player left to multiplayer system last to delete player afterwards
    DefaultMultiplayerNotificationHandler::HandlePlayerLeftSession(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandlePlayerJoinedSession(const Ptr<Multiplayer::PlayerJoinedSession>& msg)
{                            
    // send player joined to multiplayer system first for registration
    DefaultMultiplayerNotificationHandler::HandlePlayerJoinedSession(msg);
    MultiplayerTestNotificationHandler::reconnections++;

    const Ptr<Player>& player = MultiplayerManager::Instance()->GetPlayer(msg->GetPlayerHandle());
    n_printf("Player %s joined!\n", player->GetGamerTag().AsString().AsCharPtr());
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandlePlayerSignInStateChanged(const Ptr<PlayerSignInChanged>& msg)
{               
    DefaultMultiplayerNotificationHandler::HandlePlayerSignInStateChanged(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandleGameStarting(const Ptr<GameStarting>& msg)
{       
    MultiplayerManager::Instance()->StartingFinished();
    DefaultMultiplayerNotificationHandler::HandleGameStarting(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandleGameStarted(const Ptr<GameStarted>& msg)
{                                        
    MultiplayerConnectionTest::Instance()->SetTestState(MultiplayerConnectionTest::SessionStarted);   
    DefaultMultiplayerNotificationHandler::HandleGameStarted(msg); 
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandleGameEnded(const Ptr<GameEnded>& msg)
{                                       
    DefaultMultiplayerNotificationHandler::HandleGameEnded(msg);   
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandlePlayerReadyChanged(const Ptr<PlayerReadyChanged>& msg)
{                                      
    DefaultMultiplayerNotificationHandler::HandlePlayerReadyChanged(msg);  
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandleDataReceived(const Ptr<DataReceived>& msg)
{                                     
    DefaultMultiplayerNotificationHandler::HandleDataReceived(msg);        
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerTestNotificationHandler::HandleDisconnectionFromServer()
{                                   
    DefaultMultiplayerNotificationHandler::HandleDisconnectionFromServer();
}
} // namespace Multiplayer