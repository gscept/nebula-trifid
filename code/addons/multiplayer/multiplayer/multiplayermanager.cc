//------------------------------------------------------------------------------
//  network/multiplayer/multiplayermanager.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "multiplayer/multiplayermanager.h"
#include "internalmultiplayer/internalmultiplayerinterface.h"
#include "core/config.h"
#include "debugrender/debugrender.h"
#include "basegametiming/gametimesource.h"
#include "multiplayerfeature/distributionid.h"
#include "internalmultiplayer/raknet/raknetpacketid.h"

namespace Multiplayer
{
__ImplementClass(MultiplayerManager, 'MPMM', Core::RefCounted);
__ImplementSingleton(MultiplayerManager);

using namespace InternalMultiplayer;

//------------------------------------------------------------------------------
/**
*/
MultiplayerManager::MultiplayerManager() :  maxLocalPlayers(1),
                                            inCreatePlayers(false),
                                            isOpen(false),
                                            sessionState(NoSession),
                                            gameFunction(None),
                                            gameState(Idle),                                            
                                            gamesearchState(GameSearchNoResults)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MultiplayerManager::~MultiplayerManager()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
MultiplayerManager::Open()
{
    n_assert(!this->isOpen);
	this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::Close()
{
    n_assert(this->isOpen);

    this->isOpen = false;
    this->DeletePlayers();
    this->gamesearchResults.Clear();
}

//------------------------------------------------------------------------------
/**
    Trigger the manager
*/
void
MultiplayerManager::Trigger()
{
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::RenderDebug()
{
    Math::float2 pos(0.05f, 0.2f);

    // game state
    Util::String func = "GameState: ";
    switch (this->gameFunction)
    {
        case Idle:
            func.Append("Idle");
            break;
        case InitializingAsHost:
            func.Append("InitializingAsHost");
            break;
        case HostingGame:
            func.Append("HostingGame");
            break;
        case InitializingAsClient:
            func.Append("InitializingAsClient");
            break;
        case JoinedGame:
            func.Append("JoinedGame");
            break;
        default:
            func.Append("UNKNOWN");
            break;
    }
    _debug_text(func, pos, Math::float4(1,1,1,1));
    pos += Math::float2(0,0.025f);

    // game function
    Util::String state = "Function: ";
    switch (this->gameState)
    {
        case None:
            state.Append("None");
            break;
        case Starting:
            state.Append("Starting");
            break;
        case Running:
            state.Append("Running");
            break;
        case Stopping:
            state.Append("Stopping");
            break;
        default:
            state.Append("UNKNOWN");
            break;
    }
    _debug_text(state, pos, Math::float4(1,1,1,1));
    pos += Math::float2(0,0.025f);

    // game search
    Util::String search = "GameSearch: ";
    switch (this->gamesearchState)
    {
    case GameSearchNoResults:
        search.Append("GameSearchNoResults");
        break;
    case GameSearchActive:
        search.Append("GameSearchActive");
        break;
    case GameSearchFinished:
        search.Append("GameSearchFinished");
        break;
    default:
        search.Append("UNKNOWN");
        break;
    }
    _debug_text(search, pos, Math::float4(1,1,1,1));
    pos += Math::float2(0,0.025f);

    // session info
    Util::String session = "Session: ";
    if (this->activeSession.isvalid())
    {
        switch (this->activeSession->GetType())
        {
        case InternalMultiplayer::MultiplayerType::CoachCoop:
            session.Append("CoachCoop");
            break;
        case InternalMultiplayer::MultiplayerType::LocalNetwork:
            session.Append("LocalNetwork");
            break;
        case InternalMultiplayer::MultiplayerType::OnlineStandard:
            session.Append("OnlineStandard");
            break;
        case InternalMultiplayer::MultiplayerType::OnlineRanked:
            session.Append("OnlineRanked");
            break;
        default:
            session.Append("UNKNOWN");
            break;
        }
    }
    _debug_text(session, pos, Math::float4(1,1,1,1));
    pos += Math::float2(0,0.025f);

    // all local players
    Util::String lps;
    int index;
    for (index = 0; index < localPlayers.Size(); index++)
    {
        if (this->localPlayers[index].isvalid())
        {
            lps.Append("\nlocalPlayer ");
            lps.Append(Util::String::FromInt(index));
            lps.Append(": ");
            lps.Append(this->localPlayers[index]->GetGamerTag().Value());
    
            switch (this->localPlayers[index]->GetSignState())
            {
            case Base::InternalPlayerBase::NotSignedIn:
                lps.Append(", NotSignedIn");
                break;
            case Base::InternalPlayerBase::SignedInLocally:
                lps.Append(", SignedInLocally");
                break;
            case Base::InternalPlayerBase::SignedInOnline:
                lps.Append(", SignedInOnline");
                break;
            default:
                lps.Append("UNKNOWN");
                break;
            }
        }
    }
    _debug_text(lps, pos, Math::float4(1,1,1,1));
    pos += Math::float2(0,0.025f * index);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::RegisterNotificationHandler(const Ptr<Base::MultiplayerNotificationHandlerBase>& handler)
{
    n_assert(handler.isvalid());
    Ptr<Multiplayer::AttachNotificationHandler> attach = Multiplayer::AttachNotificationHandler::Create();
    attach->SetNotificationHandler(handler);
    InternalMultiplayerInterface::Instance()->SendBatched(attach.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::UnregisterNotificationHandler(const Ptr<Base::MultiplayerNotificationHandlerBase> &handler)
{
    n_assert(handler.isvalid());
    Ptr<Multiplayer::RemoveNotificationHandler> remove = Multiplayer::RemoveNotificationHandler::Create();
    remove->SetNotificationHandler(handler);
    InternalMultiplayerInterface::Instance()->SendBatched(remove.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::CreatePlayers(SizeT playerCount)
{
	Util::Array<Util::String> dummy;
	this->CreatePlayers(playerCount,dummy);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::CreatePlayers(SizeT playerCount, const Util::Array<Util::String> & names)
{
	n_assert2(!this->inCreatePlayers, "CreatePlayers already active, wait till process has succeded!");
	n_assert(playerCount <= NEBULA3_MAX_LOCAL_PLAYERS);

	// send message to internal multiplayer thread
	Ptr<Multiplayer::CreatePlayers> createMsg = Multiplayer::CreatePlayers::Create();
	createMsg->SetMaxLocalPlayers(playerCount);
	createMsg->SetPlayerTags(names);
	InternalMultiplayer::InternalMultiplayerInterface::Instance()->SendBatched(createMsg.cast<Messaging::Message>());

	this->inCreatePlayers = true;
}
//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::StartGameSearch(Ptr<Base::ParameterResolverBase> parameterResolver,
                                    SizeT maxResults,
									Util::String IP,
                                    Timing::Time timeOut /*= 3.0*/)
{
    n_assert(this->localPlayers.Size() > 0);

	//If no IP is provided then do a broadcast to find local games (LAN or same computer)
	if (IP == "")
	{
		IP = "255.255.255.255";
	}
    // send game search message to internal multiplayer thread
    Ptr<Multiplayer::StartGameSearch> searchMsg = Multiplayer::StartGameSearch::Create();
    searchMsg->SetParameterResolver(parameterResolver);
    searchMsg->SetMaxResults(maxResults);
    searchMsg->SetLocalPlayers(this->GetLocalPlayerHandles());
    searchMsg->SetTimeout(timeOut);
	searchMsg->SetIP(IP);
    searchMsg->SetPlayerHandle(this->localPlayers[0]->GetPlayerHandle());        // use first as "hoster of search"
    InternalMultiplayerInterface::Instance()->SendBatched(searchMsg.cast<Messaging::Message>());

    // game search can be done every time
    this->gamesearchState = GameSearchActive;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HostSession(Ptr<Base::ParameterResolverBase> parameterResolver,
                                ushort publicSlots,
                                ushort privateSlots,
                                InternalMultiplayer::MultiplayerType::Code type,
                                IndexT playerIndex)
{
    n_assert(this->GetLocalPlayerHandles().Size() > 0)
    n_assert(this->localPlayers[playerIndex].isvalid());

    Ptr<Multiplayer::HostSession> hostMsg = Multiplayer::HostSession::Create();
    hostMsg->SetPlayerHandle(this->localPlayers[playerIndex]->GetPlayerHandle());
    hostMsg->SetLocalPlayers(this->GetLocalPlayerHandles());
    hostMsg->SetMultiplayerType(type);
    hostMsg->SetPrivateSlots(privateSlots);
    hostMsg->SetPublicSlots(publicSlots);
    hostMsg->SetParameterResolver(parameterResolver);
    InternalMultiplayer::InternalMultiplayerInterface::Instance()->SendBatched(hostMsg.cast<Messaging::Message>());

    this->SetGameFunction(InitializingAsHost);
    this->SetSessionState(CreatingSession);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::DeleteSession()
{
    Ptr<Multiplayer::DeleteSession> deleteMsg = Multiplayer::DeleteSession::Create();
    InternalMultiplayerInterface::Instance()->SendBatched(deleteMsg.cast<Messaging::Message>());

    this->SetSessionState(DeletingSession);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::LeaveSession(IndexT playerIndex)
{
    Ptr<Multiplayer::LeaveSession> leaveMsg = Multiplayer::LeaveSession::Create();
    leaveMsg->SetLocalPlayers(this->GetLocalPlayerHandles());
    leaveMsg->SetPlayerHandle(this->localPlayers[playerIndex]->GetPlayerHandle());
    InternalMultiplayerInterface::Instance()->SendBatched(leaveMsg.cast<Messaging::Message>());

    this->SetSessionState(LeavingSession);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::SetPlayerReady(bool ready, IndexT playerIndex)
{
    Ptr<Multiplayer::SetPlayerReady> setRdy = Multiplayer::SetPlayerReady::Create();
    setRdy->SetReady(ready);
    setRdy->SetPlayerHandle(this->localPlayers[playerIndex]->GetPlayerHandle());
    InternalMultiplayerInterface::Instance()->SendBatched(setRdy.cast<Messaging::Message>());

    this->localPlayers[playerIndex]->SetReady(ready);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::StartGame()
{
    Ptr<Multiplayer::StartGame> startGame = Multiplayer::StartGame::Create();
    InternalMultiplayerInterface::Instance()->SendBatched(startGame.cast<Messaging::Message>());

    this->SetGameState(Starting);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::StartingFinished()
{
    n_printf("MultiplayerManager::StartingFinished! \n");
    Ptr<Multiplayer::StartingGameFinished> startGame = Multiplayer::StartingGameFinished::Create();
    InternalMultiplayerInterface::Instance()->SendBatched(startGame.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::EndGame()
{
    n_printf("MultiplayerManager::EndGame! \n");
    Ptr<Multiplayer::EndGame> endGame = Multiplayer::EndGame::Create();
    InternalMultiplayerInterface::Instance()->SendBatched(endGame.cast<Messaging::Message>());

    this->SetGameState(Stopping);
}
//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::EndingFinished()
{
	n_printf("MultiplayerManager::EndingFinished! \n");
	Ptr<Multiplayer::EndingGameFinished> endGame = Multiplayer::EndingGameFinished::Create();
	InternalMultiplayerInterface::Instance()->SendBatched(endGame.cast<Messaging::Message>());
}


//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleChatMessage(const Ptr<ChatMessage>&msg, bool fromNetwork)
{
	Ptr<InternalMultiplayer::NetStream> stream = InternalMultiplayer::NetStream::Create();
	Ptr<InternalMultiplayer::BitWriter> writer = InternalMultiplayer::BitWriter::Create();
	writer->SetPacketId(PacketId::NebulaChatMessage);
	writer->SetStream(stream.cast<IO::Stream>());  		
	Timing::Tick curTime = BaseGameFeature::GameTimeSource::Instance()->GetTicks();
	writer->SetWriteTimeStamp(curTime);		
	if (writer->Open())
	{												
		msg->Encode(writer.cast<IO::BinaryWriter>());

		if(this->gameFunction == HostingGame)
		{		
			if(msg->GetToPlayer().Length())
			{
				const Util::Array<Ptr<Player>>& pls = this->activeSession->GetPublicPlayers();
				for(int i = 0 ; i < pls.Size() ; i++)
				{
					if(pls[i]->GetGamerTag().AsString() == msg->GetToPlayer())
					{
						SendStreamToPlayer(pls[i],stream,true,false,true,InternalMultiplayer::PacketChannel::ChatChannel);
						break;
					}
				}
				
			}
			else
			{
				const Util::Array<Ptr<Player>>& pls = this->activeSession->GetPublicPlayers();

				for(int i = 0 ; i < pls.Size() ; i++)
				{
					if(!pls[i]->IsLocal())
					{
						SendStreamToPlayer(pls[i],stream,true,false,true,InternalMultiplayer::PacketChannel::ChatChannel);
					}					
				}
			}
		}
		else
		{
			if(!fromNetwork)
			{
				SendStreamToHost(stream,true,false,true,InternalMultiplayer::PacketChannel::ChatChannel);
			}			
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::SendChatMessage(const Ptr<Player>& player, const Util::String & message, int action)
{
	if(this->gameFunction == HostingGame || this->gameFunction == JoinedGame)
	{
		Ptr<ChatMessage> msg = ChatMessage::Create();
		msg->SetChatAction(action);
		msg->SetChatMessage(message);
		if(player.isvalid())
		{
			msg->SetToPlayer(player->GetGamerTag().AsString());
		}								
		msg->SetFromPlayer(this->localPlayers[0]->GetGamerTag().AsString());
		this->HandleChatMessage(msg, false);
	}
}
//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::JoinSession(InternalMultiplayer::SessionInfo session,
                                IndexT playerIndex)
{
    n_assert(this->localPlayers[playerIndex].isvalid());

    Ptr<Multiplayer::JoinSession> joinMsg = Multiplayer::JoinSession::Create();
    joinMsg->SetPlayerHandle(this->localPlayers[playerIndex]->GetPlayerHandle());
    joinMsg->SetLocalPlayers(this->GetLocalPlayerHandles());
    joinMsg->SetSessionInfo(session);
    InternalMultiplayerInterface::Instance()->SendBatched(joinMsg.cast<Messaging::Message>());

    this->SetGameFunction(InitializingAsClient);
    this->SetSessionState(JoiningSession);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::SetupNewSession(SizeT publicSlots,
                                    SizeT privateSlots,
                                    InternalMultiplayer::SessionHandle sessionHandle,
                                    InternalMultiplayer::MultiplayerType::Code sessionType,
                                    bool addLocalplayers,
                                    bool addPublic)
{
    // setup new session ...
    this->activeSession = Session::Create();
    this->activeSession->SetPrivateSlotSize(privateSlots);
    this->activeSession->SetPublicSlotSize(publicSlots);
    this->activeSession->SetSessionHandle(sessionHandle);
    this->activeSession->SetType(sessionType);

    if (addLocalplayers)
    {
        // ... and add all local players
        IndexT index;
        for (index = 0; index < this->localPlayers.Size(); index++)
        {
            if (this->localPlayers[index].isvalid())
            {
                if (addPublic)
                {
                    this->activeSession->AddPlayerPublic(this->localPlayers[index]);
                }
                else
                {
                    this->activeSession->AddPlayerPrivate(this->localPlayers[index]);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<InternalMultiplayer::PlayerHandle>
MultiplayerManager::GetLocalPlayerHandles() const
{
    Util::Array<InternalMultiplayer::PlayerHandle> handles;
    IndexT index;
    for (index = 0; index < this->localPlayers.Size(); index++)
    {
        n_assert(this->localPlayers[index].isvalid());
        handles.Append(this->localPlayers[index]->GetPlayerHandle());
    }
    return handles;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Player>
MultiplayerManager::GetPlayer(const InternalMultiplayer::PlayerHandle &handle) const
{
    IndexT index;

    // first check local players
    for (index = 0; index < this->localPlayers.Size(); index++)
    {
        n_assert(this->localPlayers[index].isvalid());
        if (this->localPlayers[index]->GetPlayerHandle() == handle)
        {
            return this->localPlayers[index];
        }
    }

    // now check all players in the session (MIGHT REDUNDANT)
    if (this->activeSession.isvalid())
    {
        for (index = 0; index < this->activeSession->GetPublicPlayers().Size(); index++)
        {
            if (this->activeSession->GetPublicPlayerByIndex(index)->GetPlayerHandle() == handle)
            {
                return this->activeSession->GetPublicPlayerByIndex(index);
            }
        }

        for (index = 0; index < this->activeSession->GetPrivatePlayers().Size(); index++)
        {
            if (this->activeSession->GetPrivatePlayerByIndex(index)->GetPlayerHandle() == handle)
            {
                return this->activeSession->GetPrivatePlayerByIndex(index);
            }
        }
    }
    n_error("MultiplayerManager::GetPlayer() -> where does this handle come from?! its not existing in multiplayermanager!");
    return Ptr<Player>(0);
}

//------------------------------------------------------------------------------
/**
*/
Util::String
MultiplayerManager::GameFunctionAsString(GameFunction func)
{
    switch (func)
    {
    case InitializingAsHost:    return "InitializingAsHost";
    case HostingGame:           return "HostingGame";
    case InitializingAsClient:  return "InitializingAsClient";
    case JoinedGame:            return "JoinedGame";
    case None:                  return "None";
    }
    return "InvalidFunction";
}

//------------------------------------------------------------------------------
/**
*/
Util::String
MultiplayerManager::GameStateAsString(GameState state)
{
    switch (state)
    {
    case Idle:                  return "Idle";
    case Starting:              return "Starting";
    case Running:               return "Running";
    case Stopping:              return "Stopping";
    }
    return "InvalidGameState";
}

//------------------------------------------------------------------------------
/**
*/
Util::String
MultiplayerManager::SessionStateAsString(SessionState sessionState)
{
    switch(sessionState)
    {
    case NoSession:             return "NoSession";
    case CreatingSession:       return "CreatingSession";
    case SessionCreated:        return "SessionCreated";
    case JoiningSession:        return "JoiningSession";
    case SessionJoined:         return "SessionJoined";
    case LeavingSession:        return "LeavingSession";
    case SessionLeft:           return "SessionLeft";
    case DeletingSession:       return "DeletingSession";
    case SessionDeleted:        return "SessionDeleted";
    case Aborted:               return "Aborted";
    }
    return "InvalidSession";
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::OnDisconnectFromServer()
{
    // clear session
    this->activeSession->ClearSession();
    this->activeSession = 0;
    // clear any game search
    this->ClearLastGameSearch();
    // TODO: split leaving steps on networkthread
    this->SetSessionState(LeavingSession);
    this->SetSessionState(SessionLeft);
    this->SetSessionState(NoSession);
    this->SetGameFunction(None);

    switch(this->gameState)
    {
    case Running:
        this->SetGameState(Stopping);
        this->SetGameState(Idle);
        break;
    default:
        // empty
        break;
    }

}

//------------------------------------------------------------------------------
/**
*/
bool
MultiplayerManager::GetPlayerReady(IndexT playerIndex) const
{
    n_assert(this->localPlayers[playerIndex].isvalid());
    return this->localPlayers[playerIndex]->IsReady();
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::OnLocalPlayerLeftSessionConfirmed(const InternalMultiplayer::PlayerHandle& playerHandle)
{

}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::DeletePlayers()
{
    IndexT index;
    for (index = 0; index < this->localPlayers.Size(); index++)
    {
        if (this->localPlayers[index].isvalid())
        {
            // TODO: send to network thread
        }
        this->localPlayers[index] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::SendStreamToPlayer(const Ptr<Player>& player, 
                                       const Ptr<InternalMultiplayer::NetStream>& stream, 
                                       bool reliable, 
                                       bool sequential, 
                                       bool ordered, 
                                       InternalMultiplayer::PacketChannel::Code channel /*= InternalMultiplayer::PacketChannel::DefaultChannel*/)
{
    if (this->activeSession.isvalid())
    {                    
        n_assert(stream->GetSize() > 0);  
        n_assert2(!sequential && !ordered || sequential != ordered, "Stream can only be send sequenced OR ordered!");
        Ptr<Multiplayer::SendDataToPlayer> sendMsg = Multiplayer::SendDataToPlayer::Create();
        sendMsg->SetStream(stream);
        sendMsg->SetTargetPlayerHandle(player->GetPlayerHandle());
        sendMsg->SetReliable(reliable);
        sendMsg->SetSequential(sequential);
        sendMsg->SetOrdered(ordered);                   
        sendMsg->SetChannel(channel);
        InternalMultiplayer::InternalMultiplayerInterface::Instance()->SendBatched(sendMsg.cast<Messaging::Message>());
    }
    else
    {
        n_printf("MultiplayerManager::SendStreamToPlayer: Try to send data while not in session\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::SendStreamToHost(const Ptr<InternalMultiplayer::NetStream>& stream, 
                                     bool reliable, 
                                     bool sequential, 
                                     bool ordered, 
                                     InternalMultiplayer::PacketChannel::Code channel /*= InternalMultiplayer::PacketChannel::DefaultChannel*/)
{
    if (this->activeSession.isvalid())
    {
        n_assert(stream->GetSize() > 0);  
        n_assert2(!sequential && !ordered || sequential != ordered, "Stream can only be send sequenced OR ordered!");
        Ptr<Multiplayer::SendDataToHost> sendMsg = Multiplayer::SendDataToHost::Create();
        sendMsg->SetStream(stream);
        sendMsg->SetReliable(reliable);
        sendMsg->SetSequential(sequential);
        sendMsg->SetOrdered(ordered);
        sendMsg->SetChannel(channel);
        InternalMultiplayer::InternalMultiplayerInterface::Instance()->SendBatched(sendMsg.cast<Messaging::Message>());
    }
    else
    {
        n_printf("MultiplayerManager::SendStreamToHost: Try to send data while not in session\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerManager::AddLocalPlayerToCurrentSession()
{
    // check if its possible to add a local player
    int newLocalIndex = this->GetLocalPlayerHandles().Size();
    n_assert(newLocalIndex < this->GetMaxLocalPlayers());

    Ptr<Multiplayer::AddLocalPlayer> addMsg = Multiplayer::AddLocalPlayer::Create();
    InternalMultiplayer::InternalMultiplayerInterface::Instance()->SendBatched(addMsg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerManager::RemoveLocalPlayerFromCurrentSession( IndexT playerIndex )
{
    n_assert(playerIndex <= this->localPlayers.Size());
    Ptr<Multiplayer::RemoveLocalPlayer> removeMsg = Multiplayer::RemoveLocalPlayer::Create();
    removeMsg->SetPlayerHandle(this->localPlayers[playerIndex]->GetPlayerHandle());
    InternalMultiplayer::InternalMultiplayerInterface::Instance()->SendBatched(removeMsg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleNotification(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(CreatePlayersSucceeded::Id))
    {
        this->HandleCreatePlayersSucceeded(msg.cast<CreatePlayersSucceeded>());
    }
    else if (msg->CheckId(CreatePlayersFailed::Id))
    {
        this->HandleCreatePlayersFailed(msg.cast<CreatePlayersFailed>());
    }
    else if (msg->CheckId(HostSessionSucceeded::Id))
    {
        this->HandleHostSessionSucceeded(msg.cast<HostSessionSucceeded>());
    }
    else if (msg->CheckId(HostSessionFailed::Id))
    {
        this->HandleHostSessionFailed(msg.cast<HostSessionFailed>());
    }
    else if (msg->CheckId(StartGameSearchFinished::Id))
    {
        this->HandleStartGameSearchFinished(msg.cast<StartGameSearchFinished>());
    }
    else if (msg->CheckId(JoinSessionSucceeded::Id))
    {
        this->HandleJoinSessionSucceeded(msg.cast<JoinSessionSucceeded>());
    }
    else if (msg->CheckId(JoinSessionFailed::Id))
    {
        this->HandleJoinSessionFailed(msg.cast<JoinSessionFailed>());
    }
    else if (msg->CheckId(StartGameSucceeded::Id))
    {
        this->HandleStartGameSucceeded(msg.cast<StartGameSucceeded>());
    }
    else if (msg->CheckId(StartGameFailed::Id))
    {
        this->HandleStartGameFailed(msg.cast<StartGameFailed>());
    }
    else if (msg->CheckId(EndGameSucceeded::Id))
    {
        this->HandleEndGameSucceeded(msg.cast<EndGameSucceeded>());
    }
    else if (msg->CheckId(EndGameFailed::Id))
    {
        this->HandleEndGameFailed(msg.cast<EndGameFailed>());
    }
    else if (msg->CheckId(LeaveSessionSucceeded::Id))
    {
        this->HandleLeaveSessionSucceeded(msg.cast<LeaveSessionSucceeded>());
    }
    else if (msg->CheckId(LeaveSessionFailed::Id))
    {
        this->HandleLeaveSessionFailed(msg.cast<LeaveSessionFailed>());
    }
    else if (msg->CheckId(DeleteSessionSucceded::Id))
    {
        this->HandleDeleteSessionSucceded(msg.cast<DeleteSessionSucceded>());
    }
    else if (msg->CheckId(DeleteSessionFailed::Id))
    {
        this->HandleDeleteSessionFailed(msg.cast<DeleteSessionFailed>());
    }	
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleCreatePlayersSucceeded(const Ptr<CreatePlayersSucceeded>& msg)
{
    IndexT index;
    for (index = 0; index < msg->GetPlayerInfo().Size(); index++)
    {
        Ptr<Player> newPlayer = Player::Create();
        newPlayer->SetGamerTag(msg->GetPlayerInfo().ValueAtIndex(index).gamerTag);
        newPlayer->SetSignInState(msg->GetPlayerInfo().ValueAtIndex(index).signInState);
        newPlayer->SetHandle(msg->GetPlayerInfo().ValueAtIndex(index).playerHandle);
        newPlayer->SetLocal(true);

        this->localPlayers.Append(newPlayer);
    }
    this->inCreatePlayers = false;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleCreatePlayersFailed(const Ptr<CreatePlayersFailed>& msg)
{
    // TODO: softer error handling
    n_error("MultiplayerManager::HandleCreatePlayersFailed!");
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleHostSessionSucceeded(const Ptr<HostSessionSucceeded>& msg)
{
    // handle finished host session
    const InternalMultiplayer::SessionInfo& sessionInfo = msg->GetSessionInfo();
    this->SetupNewSession(sessionInfo.GetOpenPublicSlots(),
                          sessionInfo.GetOpenPrivateSlots(),
                          msg->GetSessionHandle(),
                          sessionInfo.GetMultiplayerType(),
                          true,
                          true);

    this->SetGameFunction(HostingGame);
    this->SetSessionState(SessionCreated);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleHostSessionFailed(const Ptr<HostSessionFailed>& msg)
{
    // TODO: softer error handling
    n_error("MultiplayerManager::HandleHostSessionFailed!");
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleStartGameSearchFinished(const Ptr<StartGameSearchFinished>& msg)
{
    // save results
    this->gamesearchResults = msg->GetResults();
    n_assert(this->gamesearchState == GameSearchActive);
    this->gamesearchState = GameSearchFinished;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleJoinSessionSucceeded(const Ptr<JoinSessionSucceeded>& msg)
{
    this->SetupNewSession(msg->GetSessionInfo().GetFilledPublicSlots() + msg->GetSessionInfo().GetOpenPublicSlots(),
                          msg->GetSessionInfo().GetFilledPrivateSlots() + msg->GetSessionInfo().GetOpenPrivateSlots(),
                          msg->GetSessionHandle(),
                          InternalMultiplayer::MultiplayerType::OnlineStandard /*msg->GetSessionInfo().GetMultiPlayerType()*/,
                          true,
                          true /*msg->GetSessionInfo().JoinPublic()*/ );

    this->SetGameFunction(JoinedGame);
    this->SetSessionState(SessionJoined);
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleJoinSessionFailed(const Ptr<JoinSessionFailed>& msg)
{
    // TODO: softer error handling
    n_error("MultiplayerManager::HandleJoinSessionFailed!");
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleStartGameSucceeded(const Ptr<StartGameSucceeded>& msg)
{
    // emtpy, "gamestarted" gamestate will be set to running through notificationhandler 
    //        if all players has really started the game, this is just the 
    //        callback if the host has triggered the gamestart
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleStartGameFailed(const Ptr<StartGameFailed>& msg)
{
    // TODO: softer error handling
    n_error("MultiplayerManager::HandleStartGameFailed!");
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleEndGameSucceeded(const Ptr<EndGameSucceeded>& msg)
{
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleEndGameFailed(const Ptr<EndGameFailed>& msg)
{
    // TODO: softer error handling
    n_error("MultiplayerManager::HandleEndGameFailed!");
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleLeaveSessionSucceeded(const Ptr<LeaveSessionSucceeded>& msg)
{
    if (this->activeSession->IsSessionEmpty(true))
    {
        this->activeSession->ClearSession();
        this->activeSession = 0;
        // clear any game search
        this->ClearLastGameSearch();
        this->SetSessionState(SessionLeft);
        this->SetSessionState(NoSession);
        this->SetGameFunction(None);

        switch(this->gameState)
        {
        case Running:
            this->SetGameState(Stopping);
            this->SetGameState(Idle);
            break;
        default:
            // emtpy
            break;
        }
    }
    else
    {
        this->SetSessionState(SessionLeft);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleLeaveSessionFailed(const Ptr<LeaveSessionFailed>& msg)
{
    // TODO: softer error handling
    n_error("MultiplayerManager::HandleLeaveSessionFailed!");
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleDeleteSessionSucceded(const Ptr<DeleteSessionSucceded>& msg)
{
    this->SetSessionState(SessionDeleted);
    this->SetSessionState(NoSession);
    this->SetGameFunction(None);
    this->SetGameState(Idle);

    // remove session and its players, is already done on network thread side
    this->activeSession->ClearSession();
    this->activeSession = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerManager::HandleDeleteSessionFailed(const Ptr<DeleteSessionFailed>& msg)
{
    // TODO: softer error handling
    n_error("MultiplayerManager::HandleDeleteSessionFailed!");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerManager::HandleLocalPlayerRemoved( const Ptr<LocalPlayerRemoved>& msg )
{
    // erase from session
    this->GetSession()->RemovePlayer(msg->GetPlayerHandle());
    
    // erase from local player array
    IndexT index;
    for (index = 0; index < this->localPlayers.Size(); index++)
    {
        if (this->localPlayers[index]->GetPlayerHandle() == msg->GetPlayerHandle())
        {
            this->localPlayers.EraseIndex(index);
            break;
        }
    }

}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerManager::HandleLocalPlayerAdded( const Ptr<LocalPlayerAdded>& msg )
{
    Ptr<Player> newPlayer = Player::Create();
    newPlayer->SetGamerTag(msg->GetPlayerInfo().gamerTag);
    newPlayer->SetSignInState(msg->GetPlayerInfo().signInState);
    newPlayer->SetHandle(msg->GetPlayerInfo().playerHandle);
    newPlayer->SetLocal(true);
    this->localPlayers.Append(newPlayer);

    // TODO, not self calculated where to put, (private or public)
    if (this->GetSession()->GetPrivatePlayers().Size() < this->GetSession()->GetPrivateSlotSize())
    {
        this->GetSession()->AddPlayerPrivate(newPlayer);
    }
    else if (this->GetSession()->GetPublicPlayers().Size() < this->GetSession()->GetPublicSlotSize())
    {
        this->GetSession()->AddPlayerPublic(newPlayer);
    }
    else
    {
        n_error("MultiplayerManager::HandleLocalPlayerAdded: something went wrong, local player successfully added, but there is no slot left?");
    }
}

} // namespace Multiplayer
