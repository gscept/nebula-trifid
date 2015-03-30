//------------------------------------------------------------------------------
//  ps3test.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "multiplayerconnectiontest/multiplayerconnectiontest.h"
#include "multiplayer/multiplayermanager.h"
#include "internalmultiplayer/internalmultiplayerinterface.h"
#include "multiplayer/defaultmultiplayernotificationhandler.h"  
#include "multiplayer/multiplayermanager.h"
#include "internalmultiplayer/internalmultiplayerinterface.h"
#include "multiplayer/defaultmultiplayernotificationhandler.h"
#include "multiplayertestnotificationhandler.h"
using namespace Util;
using namespace IO;
using namespace Multiplayer;
using namespace InternalMultiplayer;

namespace Test
{
__ImplementClass(MultiplayerConnectionTest, 'MCOT', Test::TestCase);
__ImplementSingleton(MultiplayerConnectionTest);
#define WAITTIME 1.0        

//------------------------------------------------------------------------------
/**
*/    
MultiplayerConnectionTest::MultiplayerConnectionTest()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/  
MultiplayerConnectionTest::~MultiplayerConnectionTest()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerConnectionTest::Run()
{ 
    this->timer.Start();
    this->curState = Start;
    this->waitTime = 0;

    // setup multiplayer thread
    Ptr<InternalMultiplayer::InternalMultiplayerInterface> internalMpInterface = InternalMultiplayerInterface::Create();
    internalMpInterface->Open();

    // setup manager
    Ptr<Multiplayer::MultiplayerManager> multiplayerManager = Multiplayer::MultiplayerManager::Create();
    multiplayerManager->Open();	

    // create and attach default handler
    Ptr<MultiplayerTestNotificationHandler> handler = MultiplayerTestNotificationHandler::Create();    
    multiplayerManager->RegisterNotificationHandler(handler.cast<Base::MultiplayerNotificationHandlerBase>());

    while (this->curState != Exit)
    {         
        // trigger multiplayer stuff
        handler->HandlePendingNotifications();
        // manager
        multiplayerManager->Trigger();
        // flush all messages
        internalMpInterface->FlushBatchedMessages();
        
        // first search for sessions for a given time, if one found join first, otherwise create own session
        // if hosting session, wait till someone connects, then start session
        // if joined and game started, leave after random time
        // and re-join after some time
        // if hosting session and re-join has occurred, end session
        // if hosting session and re-join has occurred and session was ended, restart session
        // if hosting session and re-join has occurred and session was re-started, delete session and exit
        if (waitTime <= 0)
        {
            switch (curState)
            {
            case Start:
                this->Login();
                break;
            case LoggingIn:
                break;
            case LoggedIn: 
                this->StartGameSearch();
                break;
            case GameSearchRunning:
                break;
            case GameSearchFinished:
                // if something found join, otherwise create own session
                if (multiplayerManager->GetLastGameSearchResults().Size() > 0)
                {
                    this->Join();     
                }
                else
                {
                    this->CreateSession();   
                }
                break; 
            case Joining:
                break;
            case Joined:
                break;
            case CreatingSession:
                break;
            case HostSession:
                if (multiplayerManager->GetSession()->GetNumRemotePlayers() > 0)
                {
                    this->StartSession();
                }
                break;   
            case SessionStarting:
                break;          
            case SessionStarted:
                if (multiplayerManager->GetGameFunction() == MultiplayerManager::HostingGame)
                {
                    if (MultiplayerTestNotificationHandler::playerLeftCounter > 1)
                    {
                        Multiplayer::MultiplayerManager::Instance()->EndGame();                    
                        this->waitTime = WAITTIME;         
                        this->curState = SessionEnding;
                        n_printf("Host: Session Ending ... \n");
                    }
                }
                else
                {
                    // leave game  
                    if (MultiplayerTestNotificationHandler::reconnections < 3)
                    {                                
                        Multiplayer::MultiplayerManager::Instance()->EndGame();                    
                        this->waitTime = WAITTIME;         
                        this->curState = SessionEnding;
                        n_printf("Session Ending ... \n");
                    }
                }
                break; 
            case SessionEnding:   
                break;
            case SessionLeaving:
                break;
            }
        }
        
        Timing::Time curTime = this->timer.GetTime();
        if (this->waitTime >= 0)
        {
            this->waitTime -= (curTime - this->time);
        }           
        this->time = curTime;
    }    
}      

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::StartGameSearch()
{
    n_printf("Start Game Search ... \n");
    Ptr<Base::ParameterResolverBase> para = Base::ParameterResolverBase::Create();
    Multiplayer::MultiplayerManager::Instance()->StartGameSearch(para, 2, 1);   
    this->waitTime = WAITTIME;    
    this->curState = GameSearchRunning;
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::Login()
{
    n_printf("Logging in ... \n");
    Multiplayer::MultiplayerManager::Instance()->CreatePlayers(1);
    this->waitTime = WAITTIME;      
    this->curState = LoggingIn;
}    

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::Join()
{     
    const Util::Array<InternalMultiplayer::SessionInfo>& sessionInfos = MultiplayerManager::Instance()->GetLastGameSearchResults();

    IndexT i;
    for (i = 0; i < sessionInfos.Size(); ++i)
    {
        Util::String ip(sessionInfos[i].GetExternalIp().ToString(false));
    	if (sessionInfos[i].GetOpenPublicSlots() > 0
            && ip == "192.168.0.68")
        {     
            n_printf("Found %d results, joining first one...\n", MultiplayerManager::Instance()->GetLastGameSearchResults().Size());
            MultiplayerManager::Instance()->JoinSession(sessionInfos[i], 0); 
            this->waitTime = WAITTIME;    
            this->curState = Joining;
            break;                                                    
        }
    } 
    MultiplayerManager::Instance()->ClearLastGameSearch();     
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::CreateSession()
{
    n_printf("Found no results, Create own session...\n");
    // create session on local network with 4 slots
    Ptr<Base::ParameterResolverBase> para = Base::ParameterResolverBase::Create();
    MultiplayerManager::Instance()->HostSession(para, 2, 2, InternalMultiplayer::MultiplayerType::LocalNetwork, 0);
    MultiplayerManager::Instance()->ClearLastGameSearch();
    this->waitTime = WAITTIME;    
    this->curState = CreatingSession;
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::StartSession()
{                                     
    n_printf("Starting Session...\n");
    MultiplayerManager::Instance()->StartGame();
    this->waitTime = WAITTIME;   
    this->curState = SessionStarting;
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::EndSession()
{

}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::DeleteSession()
{

}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerConnectionTest::SetTestState(MultiplayerConnectionTest::TestState state)
{
    this->curState = state;
}
}