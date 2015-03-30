//------------------------------------------------------------------------------
//  RakNetmultiplayerserver.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/raknet/raknetmultiplayerserver.h"
#include "notificationprotocol.h"
#include "raknet/RakNetworkFactory.h"
#include "raknet/RakNetTypes.h"
#include "raknet/RakMemoryOverride.h"
#include "raknet/RakNetStatistics.h"
#include "network/bitreader.h"
#include "network/bitwriter.h"
#include "debug/debugserver.h"            
#include "http/html/htmlpagewriter.h"
#include "http/svg/svglinechartwriter.h"
#include "threading/thread.h"
#include <time.h>

namespace RakNet
{
__ImplementClass(RakNet::RakNetMultiplayerServer, 'RINS', Base::MultiplayerServerBase);
__ImplementSingleton(RakNet::RakNetMultiplayerServer);

using namespace Util;
using namespace Base;
using namespace Messaging;
using namespace Timing;
using namespace Http;
using namespace Debug;
using namespace Math;

#define SERVERPORT 6000
#define CLIENTPORT 6001
#define GAMESEARCH_PING_INTERVALL 0.25 // in seconds

#if NEBULA3_DEBUG
#define CONNECTION_TIMEOUT 3000000 // ms 3000s
#else
#define CONNECTION_TIMEOUT 10000 // ms 10s
#endif

#ifdef _USE_RAK_MEMORY_OVERRIDE
#error("RakNet memory override doesn't function properly, but Nebula overloads new and delete. Therefore no extra override is needed!")
#endif

#ifdef _USE_RAK_MEMORY_OVERRIDE
//------------------------------------------------------------------------------
/**
*/
void* 
NetworkAlloc(size_t size)
{
    return Memory::Alloc(Memory::NetworkHeap, size);
}

//------------------------------------------------------------------------------
/**
*/
void* 
NetworkRealloc(void* p, size_t size)
{
    return Memory::Realloc(Memory::NetworkHeap, p, size);
}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkFree(void* p)
{
    Memory::Free(Memory::NetworkHeap, p);
}

//------------------------------------------------------------------------------
/**
*/
void* 
NetworkMalloc_Ex(size_t size, const char *file, unsigned int line)
{
    return Memory::Alloc(Memory::NetworkHeap, size);
}

//------------------------------------------------------------------------------
/**
*/
void* 
NetworkRealloc_Ex(void* p, size_t size, const char *file, unsigned int line)
{
    return Memory::Realloc(Memory::NetworkHeap, p, size);
}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkFree_Ex(void* p, const char *file, unsigned int line)
{
    Memory::Free(Memory::NetworkHeap, p);
}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkNotifyOutOfMemory(const char *file, const long line)
{
    n_error("Raknet: Out of Memory (%s, line %l)", file, line);
}
#endif

//------------------------------------------------------------------------------
/**
*/
RakNetMultiplayerServer::RakNetMultiplayerServer():
    curNetworkState(IDLE),
    activeGameSearchMsg(0),
    updateStatistics(false),
    lastStatsUpdated(0)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
RakNetMultiplayerServer::~RakNetMultiplayerServer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
RakNetMultiplayerServer::Open()
{
    MultiplayerServerBase::Open();

    // overwrite raknet memory functions
#ifdef _USE_RAK_MEMORY_OVERRIDE
    rakMalloc = NetworkAlloc;
    rakRealloc = NetworkRealloc;
    rakFree = NetworkFree;
    rakMalloc_Ex = NetworkMalloc_Ex;
    rakRealloc_Ex = NetworkRealloc_Ex;
    rakFree_Ex = NetworkFree_Ex;
    notifyOutOfMemory = NetworkNotifyOutOfMemory;
#endif

    // init raknet
    this->raknetInterface = RakNetworkFactory::GetRakPeerInterface();
    // for accurate timestamps, uses small bandwidth
    this->raknetInterface->SetOccasionalPing(true);
}

//------------------------------------------------------------------------------
/**
*/
void
RakNetMultiplayerServer::Close()
{
    // close connection to all
    this->CloseConnectionToAll();
    // shutdown raknet
    this->raknetInterface->Shutdown(300);
    // free interface
    RakNetworkFactory::DestroyRakPeerInterface(this->raknetInterface);
    this->raknetInterface = 0;

    // call parent class 
    MultiplayerServerBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnFrame()
{        
    // handle any received packets
    this->ReceivePackets();

    // handle active game search
    this->CheckActiveGameSearch();

	if(this->isHosting) //Only host shall check
	{
		// handle game starting
		this->CheckGameStarting();

		// handle game ending
		this->CheckGameEnding();
	}
    // push all pending notifications to mainthread
    this->SendPendingNotifications();

    // call parent class 
    MultiplayerServerBase::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Invoked from MAINTHREAD
*/
void 
RakNetMultiplayerServer::OnHostSession(const Ptr<Multiplayer::HostSession>& msg)
{
    n_assert(!this->activeSession.isvalid());
   
    ushort numPublicSlots = msg->GetPublicSlots();
    ushort numPrivateSlots = msg->GetPrivateSlots();
    ushort maxConnections = numPrivateSlots + numPublicSlots - 1; // host is always in session
    
    // hold message until our state changes to 
    this->hostSessionMsg = msg;

    // start rakpeer as server
    SocketDescriptor socketDesc(SERVERPORT,0);
    bool success = this->raknetInterface->Startup(maxConnections, 30, &socketDesc, 1);
    n_assert2(success, "RakNet startup failed!");
    this->raknetInterface->SetMaximumIncomingConnections(maxConnections);

    // set high timeout for longer debugging to 10 minutes = 600 sec = 600000 ms
    this->raknetInterface->SetTimeoutTime(CONNECTION_TIMEOUT, UNASSIGNED_SYSTEM_ADDRESS);
    this->raknetInterface->SetUnreliableTimeout(500);

    this->SetNetworkState(SESSION_CREATING);

    if (!success)
    {
        n_printf("RakNetMultiplayerServer::OnHostSession: RakPeer::Startup failed! \n");
        this->HandleHostSessionFailed();
    }
    else
    {
        // for now: immediate return
        this->HandleHostSessionSucceeded();
    }    
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleHostSessionSucceeded()
{
    n_assert(this->hostSessionMsg.isvalid());
    n_assert2(!this->activeSession.isvalid(), "A Session is already active!");
    // create a new session and set in msg
    Ptr<Multiplayer::Session> session = Multiplayer::Session::Create();    
    this->activeSession = session;
    // for local multiplayer types, answer to broadcast searches
    ushort numPublicSlots = this->hostSessionMsg->GetPublicSlots();
    ushort numPrivateSlots = this->hostSessionMsg->GetPrivateSlots();
    Multiplayer::SessionInfo sessionInfo;
    sessionInfo.SetMultiplayerType(this->hostSessionMsg->GetMultiplayerType());
    sessionInfo.SetOpenPublicSlots(numPublicSlots);
    sessionInfo.SetFilledPublicSlots(0);
    sessionInfo.SetOpenPrivateSlots(numPrivateSlots);
    sessionInfo.SetFilledPrivateSlots(0); 
    this->activeSession->SetSessionInfo(sessionInfo);

    const Ptr<Multiplayer::Player>& hostPlayer = this->hostSessionMsg->GetPlayerHandle().cast<Multiplayer::Player>();    
    // our player is the host
    hostPlayer->SetHost(true);
	hostPlayer->SetReady(false);

    Util::Array< Ptr<Multiplayer::Player> > playerArray;
    playerArray.Append(hostPlayer);
    // add all local players to session
    this->AddPlayers(playerArray);

    // set additional data with number of players in session    
    this->UpdateSharedSessionInfo(this->hostSessionMsg->GetParameterResolver().downcast<RakNetParameterResolver>());    

    // clear host msg
    this->hostSessionMsg = 0;   

    // send notification             
    Ptr<Multiplayer::HostSessionSucceeded> notifyMsg = Multiplayer::HostSessionSucceeded::Create();
    notifyMsg->SetSessionHandle(this->activeSession.get());
    notifyMsg->SetSessionInfo(sessionInfo);
    this->PutNotification(notifyMsg.cast<Message>());

    this->SetNetworkState(GAME_LOBBY);
    this->isHosting = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleHostSessionFailed()
{
    n_assert(this->hostSessionMsg.isvalid());
    // send notification             
    Ptr<Multiplayer::HostSessionFailed> notifyMsg = Multiplayer::HostSessionFailed::Create();       
    notifyMsg->SetErrorMsg(Util::String("TODO: Get error msg for failed host game!"));
    this->PutNotification(notifyMsg.cast<Message>());
    
    // clear host msg
    this->hostSessionMsg = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnStartGameSearch(const Ptr<Multiplayer::StartGameSearch>& msg)
{
    n_assert(!this->activeGameSearchMsg.isvalid());
    this->activeGameSearchMsg = msg;

    // start rakpeer as client, if already called, will be ignored by raknet    
#ifdef DEBUG
	int offset = Threading::Thread::GetMyThreadId() % 50;
	SocketDescriptor socketDesc(CLIENTPORT+ offset,0);
#else
	SocketDescriptor socketDesc(CLIENTPORT,0);
#endif
    bool success = this->raknetInterface->Startup(1, 30, &socketDesc, 1);
    n_assert2(success, "RakNet startup failed!");

    // TODO: search thru lobby / matchmaking system
    // for now: just search for local connections
    // ping for any open sessions    
	success = this->raknetInterface->Ping(msg->GetIP().AsCharPtr(), SERVERPORT, true);
    n_assert(success);
    
    // clear any previous results
    this->sessionInfos.Clear();
    this->curSearchTime = msg->GetTimeout();
    this->pingIntervall = GAMESEARCH_PING_INTERVALL;
}
               
//------------------------------------------------------------------------------
/**
*/
bool 
RakNetMultiplayerServer::HaveSessionInfoFrom(const SystemAddress& address)
{
    IndexT i;
    for (i = 0; i < this->sessionInfos.Size(); ++i)
    {
        if (this->sessionInfos[i].GetExternalIp() == address)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    if raknet receives answer to our ping, sort into our sessioninfo array
*/
void 
RakNetMultiplayerServer::HandleGameSearchResultReceived(Packet* packet)
{
    // is gamesearch active, answer could be received after timeout
    // do we already have any result from this host?
    if (this->activeGameSearchMsg.isvalid() 
        && !this->HaveSessionInfoFrom(packet->systemAddress))
    {               
        // copy custom data to struct
        RakNetTime time;
        Memory::Copy(packet->data+1, (void*)&time, sizeof(RakNetTime));
        Memory::Copy(packet->data+1+sizeof(RakNetTime), (void*)&this->sharedSessionData, sizeof(this->sharedSessionData));

        BitStream bitStream(this->sharedSessionData.additionalData, SEARCH_INFO_ADDITONAL_DATA_SIZE, true);
        const Ptr<RakNetParameterResolver>& myResolver = this->activeGameSearchMsg->GetParameterResolver().downcast<RakNetParameterResolver>();        

        // check if this is the search we wanted
        if (myResolver->CompareParameterSetWithBitstream(&bitStream))
        {
            Multiplayer::SessionInfo info;
            //set all received infos about game session
            info.SetOpenPublicSlots(this->sharedSessionData.openPublicSlots);
            info.SetOpenPrivateSlots(this->sharedSessionData.openPrivateSlots);
            info.SetFilledPublicSlots(this->sharedSessionData.filledPublicSlots);
            info.SetFilledPrivateSlots(this->sharedSessionData.filledPrivateSlots);
            info.SetMultiplayerType((Multiplayer::MultiplayerType::Code)this->sharedSessionData.sessionType);
            // info.SetRakNetGUID(packet->guid); guid isn't valid until connection ist established
            info.SetExternalIp(packet->systemAddress);
            this->sessionInfos.Append(info);  
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleGameSearchCompleted()
{   
    // send notification             
    Ptr<Multiplayer::StartGameSearchFinished> notifyMsg = Multiplayer::StartGameSearchFinished::Create();
    notifyMsg->SetResults(this->sessionInfos);
    this->PutNotification(notifyMsg.cast<Message>());

    // shutdown raknet immediately
    this->raknetInterface->Shutdown(0);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnJoinSession(const Ptr<Multiplayer::JoinSession>& msg)
{
    const Multiplayer::SessionInfo& sessionInfo = msg->GetSessionInfo();
    // save ptr
    this->joinMsg = msg;
    
    // start rakpeer as client, if already called, will be ignored by raknet
#ifdef DEBUG
	int offset = Threading::Thread::GetMyThreadId() % 50;
	SocketDescriptor socketDesc(CLIENTPORT+ offset,0);
#else
	SocketDescriptor socketDesc(CLIENTPORT,0);
#endif
    bool success = this->raknetInterface->Startup(1, 30, &socketDesc, 1); 
    n_assert2(success, "RakNet startup failed!");

    // set high timeout for longer debugging to 10 minutes = 600 sec = 600000 ms
    this->raknetInterface->SetTimeoutTime(CONNECTION_TIMEOUT, UNASSIGNED_SYSTEM_ADDRESS);

    // join from game search result    
    success = this->raknetInterface->Connect(sessionInfo.GetExternalIp().ToString(false), SERVERPORT, 0, 0);
    n_assert2(success, "RakNet conenction failed!");

    this->SetNetworkState(SESSION_JOINING);
    //// join from invite
    //else if (sessionInfo.GetInviteInfo())
    //{
    //    HRESULT hr = this->iQNet->JoinGameFromInviteInfo(playerIndex,                   // dwUserIndex
    //                                                     localUserIndices,              // dwUserMask
    //                                                     sessionInfo.GetInviteInfo());  // pInviteInfo
    //    if( FAILED( hr ) )
    //    {
    //        n_printf( "Failed joining game from invite (err = 0x%08x)!\n", hr );            
    //    }
    //}    
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleConnectionSucceeded(Packet* packet)
{
    this->SetNetworkState(SESSION_JOINING_RECEVING_PLAYERS);
    // session succcessfully joined, only wait for players
    this->HandleJoinSessionSucceeded(packet);

    // send player info, host will send me all other players in this session
    n_assert(this->localPlayers.Size() == 1);
    this->SendPlayerInfoTo(RakNetPackedId::NebulaPlayerinfo, packet->guid, this->localPlayers);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleConnectionFailed()
{
    this->SetNetworkState(IDLE);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleJoinSessionSucceeded(Packet* packet)
{
    // create new  session and add all local players to it
    this->activeSession = Multiplayer::Session::Create();

    // update session info, we will get player joined messages from all players that are already in session 
    // the slot count will be counted on the notification messages
    const Multiplayer::SessionInfo& sessionInfo = this->joinMsg->GetSessionInfo();
    Multiplayer::SessionInfo startupSessionInfo;
    startupSessionInfo.SetOpenPublicSlots(sessionInfo.GetOpenPublicSlots() + sessionInfo.GetFilledPublicSlots());
    startupSessionInfo.SetOpenPrivateSlots(sessionInfo.GetOpenPrivateSlots() + sessionInfo.GetFilledPrivateSlots());
    startupSessionInfo.SetFilledPublicSlots(0);
    startupSessionInfo.SetFilledPrivateSlots(0);
    // guid is now valid
    startupSessionInfo.SetRakNetGUID(packet->guid);
    startupSessionInfo.SetExternalIp(packet->systemAddress);
    this->activeSession->SetSessionInfo(startupSessionInfo);  

    // add our local player to session
    const Ptr<Multiplayer::Player>& player = this->joinMsg->GetPlayerHandle().cast<Multiplayer::Player>();
	// Set us as explicitly as a client since we joining a server! Otherwise this will fail when changing host, the system thinking several players want to be host.
	player->SetHost(false);
	player->SetReady(false);
    this->AddPlayer(player);    

    // clear msg
    this->joinMsg = 0;

    // send notification             
    Ptr<Multiplayer::JoinSessionSucceeded> notifyMsg = Multiplayer::JoinSessionSucceeded::Create();
    notifyMsg->SetSessionHandle(this->activeSession.get());
    notifyMsg->SetSessionInfo(startupSessionInfo);
    this->PutNotification(notifyMsg.cast<Message>());

    this->SetNetworkState(GAME_LOBBY);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleJoinSessionFailed()
{

}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnCreatePlayers(const Ptr<Multiplayer::CreatePlayers>& msg)
{
    // cleanup
    this->playerInfo.Clear();
    
	bool createNames = (msg->GetMaxLocalPlayers() != msg->GetPlayerTags().Size());

    // create local players
    IndexT index;
    for (index = 0; index < msg->GetMaxLocalPlayers(); index++)
    {
        // create  players
        Ptr<Multiplayer::Player> player = Multiplayer::Player::Create();    
        RakNetGUID ownGuid = this->raknetInterface->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
        Multiplayer::UniquePlayerId playerId(ownGuid);    
        player->SetUniqueId(playerId);
        Util::String address(this->raknetInterface->GetLocalIP(0));
        address.SubstituteString("192.168.0.", "_");
		if(createNames)
		{
			player->SetPlayerName(Util::StringAtom("TestPlayer" + Util::String::FromInt( time(0) % 50)+ address + Util::String::FromInt(index)));
		}
		else
		{
			player->SetPlayerName(msg->GetPlayerTags()[index]);
		}
        player->SetSignInState(Multiplayer::Player::SignedInOnline);
        player->SetLocal(true);
        // directly add to our server
        this->AddLocalPlayer(player); 

        // show sign in gui to allow user to sign in
        // TODO: handle sign in, need friend / buddy system
        Multiplayer::PlayerInfo newInfo;
        newInfo.gamerTag = player->GetPlayerName();
        newInfo.playerHandle = player.get();
        newInfo.signInState = player->GetSignInState();
        this->playerInfo.Add(index, newInfo);
    }

    // send notification             
    Ptr<Multiplayer::CreatePlayersSucceeded> notifyMsg = Multiplayer::CreatePlayersSucceeded::Create();
    notifyMsg->SetPlayerInfo(this->playerInfo);
    this->PutNotification(notifyMsg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::ReceivePackets()
{
    Packet *p = this->raknetInterface->Receive();
    while (p)
    {
        this->updateStatistics = true;
        MessageID packetIdentifier = p->data[0];
        if (packetIdentifier == ID_TIMESTAMP)
        {
            // timestamp in packet overjump it
            packetIdentifier = *(p->data + sizeof(RakNetPackedId::PacketIdType) + sizeof(Timing::Tick));
        }
        switch (packetIdentifier)
        {
        //////////////////////////////////////////////////////////////////////////
        // HOST 
        //////////////////////////////////////////////////////////////////////////   
        case ID_NEW_INCOMING_CONNECTION:
            {
                n_assert(this->activeSession.isvalid());                
            }
            break;
        case RakNetPackedId::NebulaPlayerinfo:
            {
                // FIXME: HMMMM, if we are host any playerinfo comes with a join attempt                
                if (this->isHosting)
                {
                    this->HandlePlayerJoined(p);
                }                
            }
            break;
        case RakNetPackedId::NebulaClientStartgameFinished:
            {
                this->HandlePlayerGameStartFinished(p);
            }
            break;
		case RakNetPackedId::NebulaClientEndgameFinished:
			{
				this->HandlePlayerGameEndFinished(p);
			}
			break;
        //////////////////////////////////////////////////////////////////////////
        // CLIENT
        //////////////////////////////////////////////////////////////////////////
        // on search for sessions
        case ID_PONG:
            {
                // save the answer from other host
                this->HandleGameSearchResultReceived(p);
            }
            break;
        case ID_CONNECTION_REQUEST_ACCEPTED:
            {
                n_assert2(this->joinMsg.isvalid(), "Connection accepted received, without join request!")

                    const Multiplayer::SessionInfo& sessionInfo = this->joinMsg->GetSessionInfo();
                if (p->systemAddress == sessionInfo.GetExternalIp())
                {
                    this->HandleConnectionSucceeded(p); // will send IdNebula3Playerinfo to server
                }
                else
                {
                    n_error("ID_CONNECTION_REQUEST_ACCEPTED: SystemAddress of packet doesn't match our request SystemAddress!");
                }
            }
            break;
        case ID_CONNECTION_ATTEMPT_FAILED:
            {
                n_assert2(this->joinMsg.isvalid(), "Connection accepted received, without join request!")
                    this->HandleConnectionFailed();                
            }
            break;
        case ID_REMOTE_NEW_INCOMING_CONNECTION:
            {                   
                this->HandleRemotePlayerJoined(p);                
            }
            break;
        case ID_REMOTE_CONNECTION_LOST:
        case ID_REMOTE_DISCONNECTION_NOTIFICATION:
            {
                // remove player, notify mainthread
                this->HandleRemotePlayerLeft(p);
            }
            break;
        case RakNetPackedId::NebulaStartGame:
            {
                this->HandleGameStartInvoked();
            }
            break;
		case RakNetPackedId::NebulaEndgame:
			{
				this->HandleGameEndInvoked();
			}
			break;
        case RakNetPackedId::NebulaAllStartgameFinished:
            {
                this->HandleAllPlayersStartFinished();
            }
            break;
		case RakNetPackedId::NebulaAllEndgameFinished:
			{
				this->HandleAllPlayersEndFinished();
			}
			break;
        case RakNetPackedId::NebulaNewHost:
            {
                //TODO a new host was chosen
            }
            break;
        case RakNetPackedId::NebulaMigrateHost:
            {
                //TODO: this system will be new host
            }
            break;
        //////////////////////////////////////////////////////////////////////////
        // BOTH
        //////////////////////////////////////////////////////////////////////////
        case ID_CONNECTION_LOST:
        case ID_DISCONNECTION_NOTIFICATION:
            {
                // remove player, notify mainthread
                this->HandleDisconnection(p);
            }
            break;
        case RakNetPackedId::NebulaPlayerReadynessChanged:
            {                   
                this->HandlePlayerReadinessChanged(p);
            }
            break;
		case RakNetPackedId::NebulaChatMessage:
			{    
				// send to mainthread
				this->HandleChatMessageReceived(p);
			}
			break;
        case RakNetPackedId::NebulaMessage:
            {    
                // send to mainthread
                this->HandleDataReceived(p);
            }
            break;
        }
        this->raknetInterface->DeallocatePacket(p);
        p = this->raknetInterface->Receive();
    }
}

//------------------------------------------------------------------------------
/**
    Will be send from server to all clients (the new connected and the already connected clients)
*/
void 
RakNetMultiplayerServer::HandlePlayerJoined(Packet* packet)
{
    // we are host
    n_assert(this->isHosting)
    Util::Array<Ptr<Multiplayer::Player> > oldPlayersInSession;
    oldPlayersInSession = this->activeSession->GetPlayers(false);
    // send infos about our current players in session to new joined player    
    this->SendPlayerInfoTo(ID_REMOTE_NEW_INCOMING_CONNECTION, packet->guid, oldPlayersInSession);    

    Util::Array<Ptr<Multiplayer::Player> > oldPlayersInSessionWithoutHost = this->activeSession->GetPlayers(true);

    // add player(s) to session                
    Util::Array<Ptr<Multiplayer::Player> > newPlayers;
    newPlayers = this->CreateSessionPlayers(packet);   

    // send infos about our current players in session to new joined player
    n_assert(newPlayers.Size() == 1);
    IndexT i;
    for (i = 0; i < oldPlayersInSessionWithoutHost.Size(); ++i)
    {
        const Multiplayer::UniquePlayerId& id = oldPlayersInSessionWithoutHost[i]->GetUnqiueId();
        this->SendPlayerInfoTo(ID_REMOTE_NEW_INCOMING_CONNECTION, id.GetRaknetGuid(), newPlayers);        	
    } 

    // update shared session info
    this->UpdateSharedSessionInfo();

    // if we have already started, notify new player to also start his game and start gameplay
    if (this->activeSession->IsStarted())
    {
        this->SendSystemMessageTo(RakNetPackedId::NebulaStartGame, packet->guid);
        this->SendSystemMessageTo(RakNetPackedId::NebulaAllStartgameFinished, packet->guid);
    }
}

//------------------------------------------------------------------------------
/**
    
*/
void 
RakNetMultiplayerServer::HandleRemotePlayerJoined(Packet* packet)
{
    // add player(s) to session                
    Util::Array<Ptr<Multiplayer::Player> > newPlayers;
    newPlayers = this->CreateSessionPlayers(packet); 

    //// we are in joining state
    //if (this->curNetworkState == SESSION_JOINING_RECEVING_PLAYERS)
    //{
    //    this->HandleJoinSessionSucceeded();
    //}
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleDisconnection(Packet* packet)
{
    n_assert(this->activeSession.isvalid());    
    if (this->isHosting)
    {      
        // a player has disconnected
        Ptr<Multiplayer::Player> player = this->activeSession->GetPlayerByUniqueId(packet->guid);
        n_assert(player.isvalid());

        // remove from session, local and non-local and
        // remove from our player list if player was not local
        this->RemovePlayer(player);
        
        // send notification to multiplayer layer on mainthread
        Ptr<Multiplayer::PlayerLeftSession> notifyMsg = Multiplayer::PlayerLeftSession::Create();
        notifyMsg->SetPlayerHandle(player.get());
        this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   
    
        // notify all other players of left player    
        this->NotifyOfLeftPlayer(player);
        // update shared session info
        this->UpdateSharedSessionInfo();
    }
    else
    {
		// unless host migration works, we end our session
		// If already in the lobby, no need to change to it.
		if(this->curNetworkState != GAME_LOBBY)
		{ 
			// FIXME, what is this
			if (this->curNetworkState != GAME_ENDING)
			{
				this->SetNetworkState(GAME_ENDING);
			}
			this->SetNetworkState(GAME_LOBBY);
		}
        this->SetNetworkState(SESSION_LEAVING);     
        this->SetNetworkState(SESSION_DELETING);     
        // server disconnection to server, clear everything
        this->activeSession->ClearSession();
        this->activeSession = 0;
        this->players.Clear();
        this->SetNetworkState(IDLE);

        // shutdown raknet
        this->raknetInterface->Shutdown(500);

        // send notification to multiplayer layer on mainthread
        Ptr<Multiplayer::DisconnectFromServer> notifyMsg = Multiplayer::DisconnectFromServer::Create();
        this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleRemotePlayerLeft(Packet* packet)
{
    n_assert(this->activeSession.isvalid());
    n_assert(this->isHosting);

    // a player has left
    Ptr<Multiplayer::Player> player = this->activeSession->GetPlayerByUniqueId(packet->guid);
    n_assert(player.isvalid());

    // remove from session
    this->RemovePlayer(player);

    // send notification to multiplayer layer on mainthread
    Ptr<Multiplayer::PlayerLeftSession> notifyMsg = Multiplayer::PlayerLeftSession::Create();
    notifyMsg->SetPlayerHandle(player.get());    
    this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   
}

//------------------------------------------------------------------------------
/**
    Invoked from MAINTHREAD, local player has left our session
*/
void 
RakNetMultiplayerServer::OnLeaveSession(const Ptr<Multiplayer::LeaveSession>& msg)
{
	if(!this->activeSession.isvalid())
	{
		return;
	}
    n_assert(this->activeSession.isvalid());
    const Ptr<Multiplayer::Player>& player = msg->GetPlayerHandle().cast<Multiplayer::Player>();
    this->RemovePlayer(player);

    // clear our session
    this->activeSession->ClearSession();
    this->activeSession = 0;

    // disconnect from server
    // shutdown raknet
    this->raknetInterface->Shutdown(500);

    // send notification to multiplayer layer on mainthread
    Ptr<Multiplayer::PlayerLeftSession> notifyLeftMsg = Multiplayer::PlayerLeftSession::Create();
    notifyLeftMsg->SetPlayerHandle(player.get());    
    this->pendingNotifcations.Append(notifyLeftMsg.cast<Messaging::Message>());   

    // send notification to multiplayer layer on mainthread
    Ptr<Multiplayer::LeaveSessionSucceeded> notifyMsg = Multiplayer::LeaveSessionSucceeded::Create();
    notifyMsg->SetPlayerHandle(player.get());
    this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());  

    // we have deleted our session and disconnect from host
    this->SetNetworkState(IDLE);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnSetPlayerReady(const Ptr<Multiplayer::SetPlayerReady>& msg)
{
    n_assert(msg->GetPlayerHandle().isvalid());
    const Ptr<Multiplayer::Player>& player = msg->GetPlayerHandle().cast<Multiplayer::Player>();
    player->SetReady(msg->GetReady());

    /// inform server
    this->BroadcastPlayerReadiness(msg->GetReady(),player);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnStartGame(const Ptr<Multiplayer::StartGame>& msg)
{
    n_assert(this->isHosting);
    n_assert(this->activeSession.isvalid());
    this->activeSession->StartSession();
   
    // broadcast starting game
    bool success = this->BroadcastSystemMessage(RakNetPackedId::NebulaStartGame);

    // for now, return msg immediately
    if (success)
    {
        this->HandleStartGameSucceded(false);
    }
    else
    {
        this->HandleStartGameFailed();
    }
}

//------------------------------------------------------------------------------
/**
    After host calls OnStartGame: client gets notification from network 
*/
void 
RakNetMultiplayerServer::HandleGameStartInvoked()
{
    n_assert(this->activeSession.isvalid());
    this->activeSession->StartSession();

    // notify mainthread to start game
    Ptr<Multiplayer::GameStarting> notifyMsg = Multiplayer::GameStarting::Create();
    notifyMsg->SetSessionHandle(this->activeSession.get());
    this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   

    this->SetNetworkState(GAME_STARTING);
}
//------------------------------------------------------------------------------
/**
    After host calls OnEndGame: client gets notification from network 
*/
void 
RakNetMultiplayerServer::HandleGameEndInvoked()
{
    n_assert(this->activeSession.isvalid());
    this->activeSession->EndSession();

    // notify mainthread to start game
    Ptr<Multiplayer::GameEnding> notifyMsg = Multiplayer::GameEnding::Create();
    notifyMsg->SetSessionHandle(this->activeSession.get());
    this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   

    this->SetNetworkState(GAME_ENDING);
}
//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleStartGameSucceded(bool ranked)
{    
    // send notification             
    Ptr<Multiplayer::StartGameSucceeded> notifyMsg = Multiplayer::StartGameSucceeded::Create();    
    this->PutNotification(notifyMsg.cast<Message>());

    // set all local players started 
    IndexT i;
    for (i = 0; i < this->localPlayers.Size(); ++i)
    {
        this->localPlayers[i]->SetStarted(true);	
    }    

    this->SetNetworkState(GAME_STARTING);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleStartGameFailed()
{
    // send notification             
    Ptr<Multiplayer::StartGameFailed> notifyMsg = Multiplayer::StartGameFailed::Create();    
    this->PutNotification(notifyMsg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
    Call from client mainthread, client has finished game start (level load etc)
*/
void 
RakNetMultiplayerServer::OnStartingGameFinished(const Ptr<Multiplayer::StartingGameFinished>& msg)
{
    // send notification to host
    const Multiplayer::SessionInfo& sessionInfo = this->activeSession->GetSessionInfo();       
    this->SendSystemMessageTo(RakNetPackedId::NebulaClientStartgameFinished, sessionInfo.GetRakNetGUID());
}

//------------------------------------------------------------------------------
/**
    Call from client mainthread, client has finished game end (stuff etc)
*/
void 
RakNetMultiplayerServer::OnEndingGameFinished(const Ptr<Multiplayer::EndingGameFinished>& msg)
{
    // send notification to host
    const Multiplayer::SessionInfo& sessionInfo = this->activeSession->GetSessionInfo();       
	this->SendSystemMessageTo(RakNetPackedId::NebulaClientEndgameFinished, sessionInfo.GetRakNetGUID());
}
//------------------------------------------------------------------------------
/**
    host receives notification from all clients that they have finsihed its game start
*/
void 
RakNetMultiplayerServer::HandlePlayerGameStartFinished(Packet* packet)
{
    n_assert(this->activeSession.isvalid());
    n_assert(this->isHosting);

    // a player has finished starting
    Ptr<Multiplayer::Player> player = this->activeSession->GetPlayerByUniqueId(packet->guid);
    n_assert(player.isvalid());
    player->SetStarted(true);
}
//------------------------------------------------------------------------------
/**
    host receives notification from all clients that they have finished its game end
*/
void 
RakNetMultiplayerServer::HandlePlayerGameEndFinished(Packet* packet)
{
    n_assert(this->activeSession.isvalid());
    n_assert(this->isHosting);

    // a player has finished ending
    Ptr<Multiplayer::Player> player = this->activeSession->GetPlayerByUniqueId(packet->guid);
    n_assert(player.isvalid());
    player->SetStarted(false);
}
//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::CheckGameStarting()
{
    if (this->curNetworkState == GAME_STARTING)
    {
        /// get players in session
        Util::Array<Ptr<Multiplayer::Player> > sessionPlayers = this->activeSession->GetPlayers(false);
        bool allStarted = true;
        IndexT i;
        for (i = 0; i < sessionPlayers.Size(); ++i)
        {
        	allStarted &= sessionPlayers[i]->GetStarted();
        }
        
        if (allStarted)
        {
            // inform all players that all have finished starting
            this->BroadcastSystemMessage(RakNetPackedId::NebulaAllStartgameFinished);            
			HandleAllPlayersStartFinished();
        }
    }    
}
//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::CheckGameEnding()
{
	if (this->curNetworkState == GAME_ENDING)
	{
		/// get players in session
		Util::Array<Ptr<Multiplayer::Player> > sessionPlayers = this->activeSession->GetPlayers(false);
		bool allStarted = true;
		IndexT i;
		for (i = 0; i < sessionPlayers.Size(); ++i)
		{
			allStarted &= !sessionPlayers[i]->GetStarted();
		}

		if (allStarted)
		{
			// inform all players that all have finished starting
			this->BroadcastSystemMessage(RakNetPackedId::NebulaAllEndgameFinished);            
			HandleAllPlayersEndFinished();
		}
	}    
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleAllPlayersStartFinished()
{
    Ptr<Multiplayer::GameStarted> notifyMsg = Multiplayer::GameStarted::Create();
    this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   

    this->SetNetworkState(GAME_PLAY);
}
//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleAllPlayersEndFinished()
{
	Ptr<Multiplayer::GameEnded> notifyMsg = Multiplayer::GameEnded::Create();
    this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   

    this->SetNetworkState(GAME_LOBBY);
}
//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnEndGame(const Ptr<Multiplayer::EndGame>& msg)
{
	n_assert(this->isHosting);
	n_assert(this->activeSession.isvalid());
    this->activeSession->EndSession();
   
    // broadcast ending game, client will only send to server
    bool success = this->BroadcastSystemMessage(RakNetPackedId::NebulaEndgame);

    // for now, return msg immediately
    if (success)
    {
        this->HandleEndGameSucceded();
    }
    else
    {
        this->HandleEndGameFailed();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleEndGameSucceded()
{
    // send notification             
    Ptr<Multiplayer::EndGameSucceeded> notifyMsg = Multiplayer::EndGameSucceeded::Create();    
    this->PutNotification(notifyMsg.cast<Message>());

	// set all local players started 
	IndexT i;
	for (i = 0; i < this->localPlayers.Size(); ++i)
	{
		this->localPlayers[i]->SetStarted(false);	
	}    


    this->SetNetworkState(GAME_ENDING);
   // this->HandleReturnToLobbySuccedded();
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleEndGameFailed()
{
    // send notification             
    Ptr<Multiplayer::EndGameFailed> notifyMsg = Multiplayer::EndGameFailed::Create();    
    this->PutNotification(notifyMsg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleReturnToLobbySuccedded()// Not used anymore.
{
    Ptr<Multiplayer::GameEnded> notifyMsg = Multiplayer::GameEnded::Create();
    this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   

    this->SetNetworkState(GAME_LOBBY);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnSendDataToPlayer(const Ptr<Multiplayer::SendDataToPlayer>& msg)
{
    const Ptr<Multiplayer::Player>& target = msg->GetTargetPlayerHandle().cast<Multiplayer::Player>();
    const Ptr<Multiplayer::NetStream>& stream = msg->GetStream();
    const BitStream* raknetStream = stream->GetBitstream();
   
    PacketPriority priority = Multiplayer::PacketPriority::AsRaknetPriority(msg->GetPriority());
    PacketReliability reliability = msg->GetReliable() && msg->GetSequential() ?  RELIABLE_SEQUENCED :
                                   !msg->GetReliable() && msg->GetSequential() ?  UNRELIABLE_SEQUENCED :
                                    msg->GetReliable() && msg->GetOrdered() ?  RELIABLE_ORDERED :
                                    msg->GetReliable() && !msg->GetSequential() ?  RELIABLE : UNRELIABLE;
    char orderingChannel = Multiplayer::PacketChannel::AsRakNetChannel(msg->GetChannel());
    SystemAddress targetAddress = this->raknetInterface->GetSystemAddressFromGuid(target->GetUnqiueId().GetRaknetGuid());
    n_assert2(targetAddress != UNASSIGNED_SYSTEM_ADDRESS, "Target isn't connected to us!");
    
    this->raknetInterface->Send(raknetStream, priority, reliability, orderingChannel, targetAddress, false);
    this->updateStatistics = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnBroadcastData(const Ptr<Multiplayer::BroadcastData>& msg)
{   
    const Ptr<Multiplayer::NetStream>& stream = msg->GetStream();
    const BitStream* raknetStream = stream->GetBitstream();

    PacketPriority priority = Multiplayer::PacketPriority::AsRaknetPriority(msg->GetPriority());
    PacketReliability reliability = msg->GetReliable() && msg->GetSequential() ?  RELIABLE_SEQUENCED :
        !msg->GetReliable() && msg->GetSequential() ?  UNRELIABLE_SEQUENCED :    
        msg->GetReliable() && msg->GetOrdered() ?  RELIABLE_ORDERED :
        msg->GetReliable() && !msg->GetSequential() ?  RELIABLE : UNRELIABLE;
    char orderingChannel = Multiplayer::PacketChannel::AsRakNetChannel(msg->GetChannel());

    this->raknetInterface->Send(raknetStream, priority, reliability, orderingChannel, UNASSIGNED_SYSTEM_ADDRESS, true);
    this->updateStatistics = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandlePlayerReadinessChanged(Packet* packet)
{
    Ptr<Multiplayer::Player> sender = this->activeSession->GetPlayerByUniqueId(packet->guid);   
    n_assert(sender.isvalid());
    Ptr<Multiplayer::NetStream> newStream = this->CreateStreamFromPacket(packet);
    Ptr<Multiplayer::BitReader> reader = Multiplayer::BitReader::Create();
    reader->SetStream(newStream.cast<IO::Stream>());
    if (reader->Open())
    {  
		RakNetGUID guid;
		guid.FromString(reader->ReadString().AsCharPtr());
        bool ready = reader->ReadBool();

		Ptr<Multiplayer::Player> player = this->activeSession->GetPlayerByUniqueId(guid);   
		n_assert(player.isvalid());

        player->SetReady(ready);

        Ptr<Multiplayer::PlayerReadyChanged> notifyMsg = Multiplayer::PlayerReadyChanged::Create();
        notifyMsg->SetPlayerHandle(player.get());
        notifyMsg->SetReadyState(ready);
        this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());

		if(this->isHosting)
		{
			/// broadcast to everyone except sender
			this->BroadcastPlayerReadiness(ready, player);
		}

        reader->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnDeleteSession(const Ptr<Multiplayer::DeleteSession>& msg)
{
    n_assert(this->activeSession.isvalid());
    n_assert(this->isHosting);
    // TODO: allow host migration
    if (this->activeSession->GetPlayers(true).Size() > 0)
    {    
        this->MigrateHost();
    }

    // we must be in lobby state or session ending state
    this->SetNetworkState(IDLE);
    
    // close every connection, kick everyone out
    this->CloseConnectionToAll();

    // shutdown raknet
    this->raknetInterface->Shutdown(500);

    // remove our session and all players in it
    if (this->activeSession->IsStarted())
    {
        this->activeSession->EndSession();
    }
    // kick all players in session
    IndexT i;
    for (i = 0; i < this->activeSession->GetPlayers(false).Size(); ++i)
    {
    	this->RemovePlayer(this->activeSession->GetPlayers(false)[i]);
    }

    this->activeSession->ClearSession();
    this->activeSession = 0;
    this->isHosting = false;    

    // immediate send notification
    Ptr<Multiplayer::DeleteSessionSucceded> notifyMsg = Multiplayer::DeleteSessionSucceded::Create();
    this->PutNotification(notifyMsg.cast<Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleDataReceived(Packet* packet)
{
    n_assert(this->activeSession.isvalid());
    // identify sender, receiver will always only this system
    Ptr<Multiplayer::Player> sender = this->activeSession->GetPlayerByUniqueId(packet->guid);   
    Ptr<Multiplayer::DataReceived> dataMsg = Multiplayer::DataReceived::Create();
    dataMsg->SetFromPlayerHandle(sender.get());   

    // create stream 
    Ptr<Multiplayer::NetStream> newStream = this->CreateStreamFromPacket(packet);
    dataMsg->SetStream(newStream);

    // send to mainthread    
    this->pendingNotifcations.Append(dataMsg.cast<Messaging::Message>());   
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleChatMessageReceived(Packet* packet)
{
	n_assert(this->activeSession.isvalid());		
	Ptr<Multiplayer::ChatReceived> chatMsg = Multiplayer::ChatReceived::Create();	
	// create stream 
	Ptr<Multiplayer::NetStream> newStream = this->CreateStreamFromPacket(packet);
	chatMsg->SetStream(newStream);

	// send to mainthread    
	this->pendingNotifcations.Append(chatMsg.cast<Messaging::Message>());   
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::CheckActiveGameSearch()
{
    if (this->activeGameSearchMsg.isvalid())
    {        
        if (this->curSearchTime <= 0.0)
        {
            this->HandleGameSearchCompleted();
            this->activeGameSearchMsg = 0;
        }
        else if (this->pingIntervall <= 0.0)
        {
            // re-ping, to get all available hosts
            bool success = this->raknetInterface->Ping("255.255.255.255", SERVERPORT, true);
            n_assert(success);
            this->pingIntervall = GAMESEARCH_PING_INTERVALL;
        }
        Timing::Time frameTime = FrameSync::FrameSyncTimer::Instance()->GetFrameTime();
        this->curSearchTime -= frameTime;        
        this->pingIntervall -= frameTime;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::SendPlayerInfoTo(MessageID infoType, const RakNetGUID& guid, const Util::Array<Ptr<Multiplayer::Player> >& playerArray)
{    
    n_assert(infoType == RakNetPackedId::NebulaPlayerinfo || infoType == ID_REMOTE_NEW_INCOMING_CONNECTION);
    Ptr<Multiplayer::NetStream> stream = Multiplayer::NetStream::Create();
    Ptr<Multiplayer::BitWriter> writer = Multiplayer::BitWriter::Create();
    writer->SetPacketId((Multiplayer::PacketId::PacketIdCode)infoType);
    writer->SetStream(stream.cast<IO::Stream>());    
    if (writer->Open())
    {        
        writer->WriteUInt(playerArray.Size());
        IndexT i;
        for (i = 0; i < playerArray.Size(); ++i)
        {
            playerArray[i]->AppendPlayerInfoToStream(writer);	
        }        
    }
    writer->Close();
    BitStream* raknetStream = stream->GetBitstream();

    PacketPriority priority = Multiplayer::PacketPriority::AsRaknetPriority(Base::PacketPriorityBase::HighPriority);
    PacketReliability reliability = RELIABLE_ORDERED;
    char orderingChannel = Multiplayer::PacketChannel::AsRakNetChannel(Base::PacketChannelBase::DefaultChannel);
    SystemAddress targetAddress = this->raknetInterface->GetSystemAddressFromGuid(guid);
    n_assert2(targetAddress != UNASSIGNED_SYSTEM_ADDRESS, "Target isn't connected to us!");

    this->raknetInterface->Send(raknetStream, priority, reliability, orderingChannel, targetAddress, false);
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Multiplayer::Player> > 
RakNetMultiplayerServer::CreateSessionPlayers(Packet* packet)
{
    Ptr<Multiplayer::NetStream> newStream = this->CreateStreamFromPacket(packet);
    Util::Array<Ptr<Multiplayer::Player> > newPlayers;
    Ptr<Multiplayer::BitReader> reader = Multiplayer::BitReader::Create();
    reader->SetStream(newStream.cast<IO::Stream>());
    if (reader->Open())
    {        
        SizeT numPlayers = reader->ReadUInt();
        IndexT i;
        for (i = 0; i < numPlayers; ++i)
        {
            Ptr<Multiplayer::Player> player = Multiplayer::Player::Create();
            player->SetLocal(false);
            player->SetPlayerInfoFromStream(reader);                        
            this->AddPlayer(player);
            newPlayers.Append(player);

            // notify mainthread 
            Ptr<Multiplayer::PlayerJoinedSession> notifyMsg = Multiplayer::PlayerJoinedSession::Create();
            notifyMsg->SetPlayerHandle(player.get());
            notifyMsg->SetPlayerName(player->GetPlayerName());
            notifyMsg->SetIsPlayerReady(player->IsReady());
            notifyMsg->SetUnqiueIdBlob(player->GetUnqiueId().AsBlob());
            this->pendingNotifcations.Append(notifyMsg.cast<Messaging::Message>());   
        }
    }
    reader->Close();

    return newPlayers;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::NotifyOfLeftPlayer(const Ptr<Multiplayer::Player>& player)
{
    Ptr<Multiplayer::NetStream> stream = Multiplayer::NetStream::Create();
    Ptr<Multiplayer::BitWriter> writer = Multiplayer::BitWriter::Create();
    writer->SetPacketId((Multiplayer::PacketId::PacketIdCode)ID_REMOTE_DISCONNECTION_NOTIFICATION);
    writer->SetStream(stream.cast<IO::Stream>());
    if (writer->Open())
    {        
        player->AppendPlayerInfoToStream(writer);	                
    }
    writer->Close();
    BitStream* raknetStream = stream->GetBitstream();
    PacketPriority priority = Multiplayer::PacketPriority::AsRaknetPriority(Base::PacketPriorityBase::HighPriority);
    PacketReliability reliability = RELIABLE_ORDERED;
    char orderingChannel = Multiplayer::PacketChannel::AsRakNetChannel(Base::PacketChannelBase::DefaultChannel);
    
    // broadcast to all except to left player
    SystemAddress excludeAddress = this->raknetInterface->GetSystemAddressFromGuid(player->GetUnqiueId().GetRaknetGuid());    
    this->raknetInterface->Send(raknetStream, priority, reliability, orderingChannel, excludeAddress, true);
}

//------------------------------------------------------------------------------
/**
*/
bool 
RakNetMultiplayerServer::BroadcastSystemMessage(Multiplayer::PacketId::PacketIdCode code, const Ptr<Multiplayer::Player>& excludePlayer)
{
    Ptr<Multiplayer::NetStream> stream = Multiplayer::NetStream::Create();
    Ptr<Multiplayer::BitWriter> writer = Multiplayer::BitWriter::Create();
    writer->SetPacketId(code);
    writer->SetStream(stream.cast<IO::Stream>());
    writer->Open();
    writer->Close();
    BitStream* raknetStream = stream->GetBitstream();
    SystemAddress excludeAddress = UNASSIGNED_SYSTEM_ADDRESS;
    if (excludePlayer.isvalid())
    {
        excludeAddress = this->raknetInterface->GetSystemAddressFromGuid(excludePlayer->GetUnqiueId().GetRaknetGuid());
    }
    bool success = this->raknetInterface->Send(raknetStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, excludeAddress, true);

    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool 
RakNetMultiplayerServer::SendSystemMessageTo(Multiplayer::PacketId::PacketIdCode code, const RakNetGUID& guid)
{
    SystemAddress hostAddress = this->raknetInterface->GetSystemAddressFromGuid(guid);

    Ptr<Multiplayer::NetStream> stream = Multiplayer::NetStream::Create();
    Ptr<Multiplayer::BitWriter> writer = Multiplayer::BitWriter::Create();
    writer->SetPacketId(code);
    writer->SetStream(stream.cast<IO::Stream>());
    writer->Open();     
    writer->Close();
    BitStream* raknetStream = stream->GetBitstream();
    bool success = this->raknetInterface->Send(raknetStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, hostAddress, false);
    this->updateStatistics = true;

    return success;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::CloseConnectionToAll()
{
    if (this->activeSession.isvalid())
    {
        IndexT i;
        Util::Array<Ptr<Multiplayer::Player> > players = this->activeSession->GetPlayers(true);
        for (i = 0; i < players.Size(); ++i)
        {
            const RakNetGUID& guid = players[i]->GetUnqiueId().GetRaknetGuid();
            this->raknetInterface->CloseConnection(raknetInterface->GetSystemAddressFromGuid(guid), true);
        }
    }     
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::UpdateSharedSessionInfo(Ptr<RakNetParameterResolver> paraResolver)
{
    n_assert(this->activeSession.isvalid());
    const Multiplayer::SessionInfo& sessionInfo = this->activeSession->GetSessionInfo();
    this->sharedSessionData.openPublicSlots = sessionInfo.GetOpenPublicSlots();
    this->sharedSessionData.openPrivateSlots = sessionInfo.GetOpenPrivateSlots();
    this->sharedSessionData.filledPublicSlots= sessionInfo.GetFilledPublicSlots();
    this->sharedSessionData.filledPrivateSlots = sessionInfo.GetFilledPrivateSlots();
    this->sharedSessionData.sessionType = (ushort)sessionInfo.GetMultiplayerType();

    // check if is there additional info
    if (paraResolver.isvalid())
    {
        // get data
        const BitStream* bitStream = paraResolver->EncodeToBitstream();
        n_assert(bitStream && bitStream->GetNumberOfBytesUsed() <= SEARCH_INFO_ADDITONAL_DATA_SIZE);
        int usedBytes = bitStream->GetNumberOfBytesUsed();

        // clear old
        Memory::Clear(this->sharedSessionData.additionalData, SEARCH_INFO_ADDITONAL_DATA_SIZE);     
        // copy over
        Memory::Copy(bitStream->GetData(), this->sharedSessionData.additionalData, usedBytes);
    }
    // clear 
    else
    {
		// this breaks checking for parameter sets
#if 0
        if (this->sharedSessionData.additionalData)
        {
            Memory::Clear(this->sharedSessionData.additionalData, SEARCH_INFO_ADDITONAL_DATA_SIZE);
        }
#endif
    }
    SizeT lengthInBytes = sizeof(this->sharedSessionData);

    this->raknetInterface->SetOfflinePingResponse((const char*)&this->sharedSessionData, lengthInBytes); 
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::SendPendingNotifications()
{
    // only progress notifications if we are in a valid state
    if (this->curNetworkState != SESSION_JOINING_RECEVING_PLAYERS)
    {
        // TODO: send one batched notification message 
        IndexT i;
        for (i = 0; i < this->pendingNotifcations.Size(); ++i)
        {
            this->PutNotification(this->pendingNotifcations[i]);            
        }
        this->pendingNotifcations.Clear();
    }    
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::BroadcastPlayerReadiness(bool ready, const Ptr<Multiplayer::Player>& excludePlayer)
{
    Ptr<Multiplayer::NetStream> stream = Multiplayer::NetStream::Create();
    Ptr<Multiplayer::BitWriter> writer = Multiplayer::BitWriter::Create();
    writer->SetPacketId((Multiplayer::PacketId::PacketIdCode)RakNet::RakNetPackedId::NebulaPlayerReadynessChanged);
    writer->SetStream(stream.cast<IO::Stream>());
    writer->Open();
	writer->WriteString(excludePlayer->GetUnqiueId().GetRaknetGuid().ToString());
    writer->WriteBit(ready);
    writer->Close();
    BitStream* raknetStream = stream->GetBitstream();

    SystemAddress excludeAddress = UNASSIGNED_SYSTEM_ADDRESS;
    if (excludePlayer.isvalid())
    {
        excludeAddress = this->raknetInterface->GetSystemAddressFromGuid(excludePlayer->GetUnqiueId().GetRaknetGuid());
    }
    bool success = this->raknetInterface->Send(raknetStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, excludeAddress, true);
    n_assert(success);
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::SetNetworkState(NetworkState newState)
{
    switch (this->curNetworkState)
    {
        case IDLE:
            n_assert(newState == SESSION_CREATING || newState == SESSION_JOINING);
            break;
        case SESSION_CREATING:
            n_assert(newState == GAME_LOBBY);
            break;
        case SESSION_JOINING:
            n_assert(newState == SESSION_JOINING_RECEVING_PLAYERS);
            break;
        case SESSION_JOINING_RECEVING_PLAYERS:
            n_assert(newState == GAME_LOBBY);
            break;
        case GAME_LOBBY:
            n_assert(newState == GAME_STARTING || newState == GAME_REGISTERING ||newState == IDLE || newState == SESSION_LEAVING);
            break;
        case GAME_REGISTERING:
            n_assert(newState == GAME_STARTING);
            break;            
        case GAME_STARTING:
            n_assert(newState == GAME_PLAY);
            break;
        case GAME_PLAY:
            n_assert(newState == GAME_ENDING );
            break;
        case GAME_ENDING:
            n_assert(newState == GAME_LOBBY || newState == SESSION_LEAVING);
            break;
        case SESSION_LEAVING:
            n_assert(newState == SESSION_DELETING || newState == IDLE);
            break;
        case SESSION_DELETING:
            n_assert(newState == IDLE);
            break;
    }
    this->curNetworkState = newState;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::HandleEndGameReceived(Packet* packet)
{
    // player has ended his game, mark as not started
    // player will be back in lobby state
    Multiplayer::UniquePlayerId id(packet->guid);
    const Ptr<Multiplayer::Player>& player = this->activeSession->GetPlayerByUniqueId(id);
    player->SetStarted(false);

    if (this->isHosting)
    {
        //inform other player of ended player
        this->BroadcastSystemMessage(RakNetPackedId::NebulaEndgame, player);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::MigrateHost()
{
    // choose player as new host
    Util::Array<Ptr<Multiplayer::Player> > otherPlayers = this->activeSession->GetPlayers(true);
    IndexT newHostIndex = 0; //TODO: choose best system
    Ptr<Multiplayer::Player> newHost = otherPlayers[newHostIndex];
    const Multiplayer::UniquePlayerId& id = newHost->GetUnqiueId();
    this->SendSystemMessageTo(RakNetPackedId::NebulaMigrateHost, id.GetRaknetGuid());

    // send new host to other players
    Ptr<Multiplayer::NetStream> stream = Multiplayer::NetStream::Create(); 
    Ptr<Multiplayer::BitWriter> writer = Multiplayer::BitWriter::Create();
    writer->SetPacketId(RakNetPackedId::NebulaNewHost);
    writer->SetStream(stream.cast<IO::Stream>());
    if (writer->Open())
    {                
        char buffer[128];
        id.GetRaknetGuid().ToString(buffer);
        writer->WriteString(Util::String(buffer));
    }
    writer->Close();
    BitStream* raknetStream = stream->GetBitstream();
    SystemAddress excludeAddress = this->raknetInterface->GetSystemAddressFromGuid(id.GetRaknetGuid());
    this->raknetInterface->Send(raknetStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, excludeAddress, true);
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Multiplayer::NetStream> 
RakNetMultiplayerServer::CreateStreamFromPacket(Packet* packet)
{ 
    Ptr<Multiplayer::NetStream> newStream = Multiplayer::NetStream::Create();   
    newStream->Open();
    newStream->FromData(packet->data, packet->bitSize);
    newStream->Close();

    return newStream;
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::OnSendDataToHost(const Ptr<Multiplayer::SendDataToHost>& msg)
{
    if (this->activeSession.isvalid())
    {
        const Ptr<Multiplayer::NetStream>& stream = msg->GetStream();
        const BitStream* raknetStream = stream->GetBitstream();

        PacketPriority priority = Multiplayer::PacketPriority::AsRaknetPriority(msg->GetPriority());
        PacketReliability reliability = msg->GetReliable() && msg->GetSequential() ?  RELIABLE_SEQUENCED :
            !msg->GetReliable() && msg->GetSequential() ?  UNRELIABLE_SEQUENCED :
            msg->GetReliable() && msg->GetOrdered() ?  RELIABLE_ORDERED :
            msg->GetReliable() && !msg->GetSequential() ?  RELIABLE : UNRELIABLE;
        char orderingChannel = Multiplayer::PacketChannel::AsRakNetChannel(msg->GetChannel());

        const Ptr<Multiplayer::Player>& target = this->activeSession->GetHostPlayer();
        SystemAddress targetAddress = this->raknetInterface->GetSystemAddressFromGuid(target->GetUnqiueId().GetRaknetGuid());
        n_assert2(targetAddress != UNASSIGNED_SYSTEM_ADDRESS, "Target isn't connected to us!");                            

        this->raknetInterface->Send(raknetStream, priority, reliability, orderingChannel, targetAddress, false);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::UpdateStatistics()
{   
#if NEBULA3_ENABLE_PROFILING
    if (this->activeSession.isvalid()
        && this->updateStatistics)
    {  
        Timing::Tick curTime = FrameSync::FrameSyncTimer::Instance()->GetTicks();
        if (this->lastStatsUpdated + 1000 < curTime)
        {
            n_assert(this->raknetInterface != 0);
            this->updateStatistics = false;   
            this->lastStatsUpdated = curTime;
            const Util::Array<Ptr<Multiplayer::Player> > player = this->activeSession->GetPlayers(true);

            IndexT i;
            for (i = 0; i < player.Size(); ++i)
            {  
                const Multiplayer::UniquePlayerId& uniqueId = player[i]->GetUnqiueId();
                const RakNetGUID& guid = uniqueId.GetRaknetGuid();
                const SystemAddress& systemAddress = this->raknetInterface->GetSystemAddressFromGuid(guid); 
                RakNetStatistics* statistics = this->raknetInterface->GetStatistics(systemAddress);

                Util::String prioNames[Multiplayer::PacketPriority::NumPacketPriorities] = {"LowPriority","NormalPriority","HighPriority"};
                Util::Dictionary<Util::StringAtom, Multiplayer::Player::StatisticValue>& counterSet = player[i]->GetDebugCounterSet();
                IndexT i;
                for (i = 0; i < Multiplayer::PacketPriority::NumPacketPriorities; ++i)
                {
                    counterSet["waitingSendMessages" + prioNames[i]].Add((int)statistics->messageSendBuffer[i]);
                    counterSet["sentMessages" + prioNames[i]].Add((int)statistics->messagesSent[i]);
                    counterSet["sentUserBits" + prioNames[i]].Add((int)statistics->messageDataBitsSent[i]);
                    counterSet["sentFullBits" + prioNames[i]].Add((int)statistics->messageTotalBitsSent[i]);                
                }      
                counterSet["onlyAckSent"].Add((int)statistics->packetsContainingOnlyAcknowlegements);
                counterSet["acknowlegdementSent"].Add((int)statistics->acknowlegementsSent);
                counterSet["acknowlegdementPending"].Add((int)statistics->acknowlegementsPending);
                counterSet["acknowlegdementBitsSent"].Add((int)statistics->acknowlegementBitsSent);
                counterSet["onlyAckSentAndResend"].Add((int)statistics->packetsContainingOnlyAcknowlegementsAndResends);
                counterSet["packetLoss"].Add((int)statistics->messageResends);
                counterSet["packetUserBitsLoss"].Add((int)statistics->messageDataBitsResent);
                counterSet["packetFullBitsLoss"].Add((int)statistics->messagesTotalBitsResent);
                counterSet["messagesOnResendQueue"].Add((int)statistics->messagesOnResendQueue);
                counterSet["unsplitMessages"].Add((int)statistics->numberOfUnsplitMessages);
                counterSet["splitMessages"].Add((int)statistics->numberOfSplitMessages);
                counterSet["numTotalSplits"].Add((int)statistics->totalSplits);
                counterSet["numPacketsSent"].Add((int)statistics->packetsSent);
                counterSet["encryptionBitsSent"].Add((int)statistics->encryptionBitsSent);
                counterSet["totalBitsSent"].Add((int)statistics->totalBitsSent);
                counterSet["sequencedMessagesOutOfOrder"].Add((int)statistics->sequencedMessagesOutOfOrder);
                counterSet["sequencedMessagesInOrder"].Add((int)statistics->sequencedMessagesInOrder);
                counterSet["orderedMessagesOutOfOrder"].Add((int)statistics->orderedMessagesOutOfOrder);
                counterSet["orderedMessagesInOrder"].Add((int)statistics->orderedMessagesInOrder);
                counterSet["numberPacketsReceived"].Add((int)statistics->packetsReceived);       
                counterSet["numberBitsReceived"].Add((int)statistics->bitsReceived);
                counterSet["numberPacketsBadCRCReceived"].Add((int)statistics->packetsWithBadCRCReceived);       
                counterSet["numberBitsBadCRCReceived"].Add((int)statistics->bitsWithBadCRCReceived);
                counterSet["numberAckReceived"].Add((int)statistics->acknowlegementsReceived);
                counterSet["numberDupAckReceived"].Add((int)statistics->duplicateAcknowlegementsReceived);
                counterSet["numberMessagesReceived"].Add((int)statistics->messagesReceived);
                counterSet["invalidMessagesReceived"].Add((int)statistics->invalidMessagesReceived);
                counterSet["duplicateMessagesReceived"].Add((int)statistics->duplicateMessagesReceived);
                counterSet["messagesWaitingForReassembly"].Add((int)statistics->messagesWaitingForReassembly);
                counterSet["OutputQueueSize"].Add((int)statistics->OutputQueueSize);
                counterSet["bitsPerSecond"].Add((int)statistics->bitsPerSecondSent);
                counterSet["bandwidthExceeded"].Add((int)statistics->bandwidthExceeded);
            }  
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::WriteStatistics(const Ptr<Http::HtmlPageWriter>& htmlWriter)
{    
#if NEBULA3_ENABLE_PROFILING
    if (this->activeSession.isvalid())
    {  
        n_assert(this->raknetInterface != 0);
        const Util::Array<Ptr<Multiplayer::Player> > player = this->activeSession->GetPlayers(false);

        if (player.Size() == 0)
        {
            htmlWriter->Element(HtmlElement::Heading3, "No Player connected!");
        }
        IndexT playerIndex;
        for (playerIndex = 0; playerIndex < player.Size(); ++playerIndex)
        {  
            const Multiplayer::UniquePlayerId& uniqueId = player[playerIndex]->GetUnqiueId();
            const RakNetGUID& guid = uniqueId.GetRaknetGuid();
            const SystemAddress& systemAddress = this->raknetInterface->GetSystemAddressFromGuid(guid); 
            Util::String addressAsString(systemAddress.ToString(false));
            
            // display debug counters
            htmlWriter->Element(HtmlElement::Heading3, Util::String("Statistics for ") + player[playerIndex]->GetPlayerName().Value() + " at " + addressAsString + " guid: " + guid.ToString());
            htmlWriter->AddAttr("border", "1");
            htmlWriter->AddAttr("rules", "cols");
            htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow); 
            htmlWriter->Element(HtmlElement::TableHeader, "Name");
            htmlWriter->Element(HtmlElement::TableHeader, "Min");
            htmlWriter->Element(HtmlElement::TableHeader, "Max");
            htmlWriter->Element(HtmlElement::TableHeader, "Avg");
            htmlWriter->Element(HtmlElement::TableHeader, "Description");
            htmlWriter->End(HtmlElement::TableRow);

            // iterate through all debug counters
            Util::Dictionary<Util::StringAtom, Multiplayer::Player::StatisticValue>& counterSet = player[playerIndex]->GetDebugCounterSet();            
            IndexT counterIndex;
            for (counterIndex = 0; counterIndex < counterSet.Size(); counterIndex++)
            {
                StringAtom name = counterSet.KeyAtIndex(counterIndex);
                String nameWithoutAddress = name.AsString().ExtractFileName();
                Array<int> history = counterSet.ValueAtIndex(counterIndex).ValuesAsArray();
                int minCount, maxCount, avgCount;
                this->ComputeMinMaxAvgCounts(history, minCount, maxCount, avgCount);
                htmlWriter->Begin(HtmlElement::TableRow);        
                htmlWriter->AddAttr("height", "30");
                htmlWriter->Begin(HtmlElement::TableData);
                htmlWriter->AddAttr("href", "/network?counterChart=" + Util::String::FromInt(playerIndex) + "_" + name.AsString());
                htmlWriter->Element(HtmlElement::Anchor, nameWithoutAddress);
                htmlWriter->End(HtmlElement::TableData);
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(minCount));
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(maxCount));
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(avgCount));
                Util::String firstDescLine = counterSet.ValueAtIndex(counterIndex).GetDescription();
                firstDescLine.Strip("\n");
                htmlWriter->Element(HtmlElement::TableData, firstDescLine);
                htmlWriter->End(HtmlElement::TableRow);
            }
            htmlWriter->End(HtmlElement::Table);
        }                                     
    }
#endif
}     

//------------------------------------------------------------------------------
/**
*/
void 
RakNetMultiplayerServer::ComputeMinMaxAvgCounts(const Array<int>& counterValues, int& outMin, int& outMax, int& outAvg) const
{
    if (counterValues.Size() > 0)
    {
        outMin = (1<<30);
        outMax = -(1<<30);
        outAvg = 0;
        IndexT i;
        for (i = 0; i < counterValues.Size(); i++)
        {
            outMin = n_min(outMin, counterValues[i]);
            outMax = n_max(outMax, counterValues[i]);
            outAvg += counterValues[i];
        }
        outAvg /= counterValues.Size();
    }
    else
    {
        outMin = 0;
        outMax = 0;
        outAvg = 0;
    }
}
} // namespace RakNet
