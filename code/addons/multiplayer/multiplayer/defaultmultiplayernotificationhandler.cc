//------------------------------------------------------------------------------
//  network/multiplayer/defaultmultiplayerNotificationhandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "multiplayer/defaultmultiplayernotificationhandler.h"
#include "multiplayer/multiplayermanager.h"

namespace Multiplayer
{
__ImplementClass(DefaultMultiplayerNotificationHandler, 'DMNH', Base::MultiplayerNotificationHandlerBase);

//------------------------------------------------------------------------------
/**
*/
DefaultMultiplayerNotificationHandler::DefaultMultiplayerNotificationHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
DefaultMultiplayerNotificationHandler::~DefaultMultiplayerNotificationHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
DefaultMultiplayerNotificationHandler::HandleNotification(const Ptr<Messaging::Message>& msg)
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
        this->HandleGameStarting(msg.cast<GameStarting>());
	}
	else if(msg->CheckId(GameEnding::Id))
	{
		this->HandleGameEnding(msg.cast<GameEnding>());
	}
    else if (msg->CheckId(LocalPlayerAdded::Id))
    {
        this->HandleLocalPlayerAdded(msg.cast<LocalPlayerAdded>());
    }
    else if (msg->CheckId(LocalPlayerRemoved::Id))
    {
        this->HandleLocalPlayerRemoved(msg.cast<LocalPlayerRemoved>());
    }	
	else if (msg->CheckId(ChatReceived::Id))
	{
		Ptr<ChatReceived> chatMsg = msg.cast<ChatReceived>();
		Ptr<InternalMultiplayer::BitReader> reader = InternalMultiplayer::BitReader::Create();
		reader->SetStream(chatMsg->GetStream().cast<IO::Stream>());
		if(reader->Open())
		{
			Ptr<ChatMessage> cmsg = ChatMessage::Create();
			cmsg->Decode(reader.cast<IO::BinaryReader>());
			MultiplayerManager::Instance()->HandleChatMessage(cmsg, true);
			this->HandleChatMessage(cmsg);
		}		
	}
    // all return notifications are directly passed to multiplayer manager
    else if (msg->IsA(ReturnNotification::RTTI))
    {
        MultiplayerManager::Instance()->HandleNotification(msg);
    }
    else
    {   
        n_error("DefaultMultiplayerNotificationHandler::HandleNotification() unknown message received!");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
DefaultMultiplayerNotificationHandler::HandlePlayerSignInStateChanged(const Ptr<PlayerSignInChanged>& msg)
{
    MultiplayerManager::Instance()->GetPlayer(msg->GetPlayerHandle())->SetSignInState(msg->GetSignInState());
}

//------------------------------------------------------------------------------
/**
*/
void
DefaultMultiplayerNotificationHandler::HandlePlayerJoinedSession(const Ptr<PlayerJoinedSession>& msg)
{
    Ptr<Player> player = Player::Create();
    player->SetGamerTag(msg->GetPlayerName());
    player->SetHandle(msg->GetPlayerHandle());
    player->SetLocal(false);
    player->SetReady(msg->GetIsPlayerReady());
    player->SetUniqueIdBlob(msg->GetUnqiueIdBlob());
    //player->SetSignInState()
    MultiplayerManager::Instance()->GetSession()->AddPlayerPublic(player);
}

//------------------------------------------------------------------------------
/**
*/
void
DefaultMultiplayerNotificationHandler::HandlePlayerLeftSession(const Ptr<PlayerLeftSession>& msg)
{
    MultiplayerManager::Instance()->GetSession()->RemovePlayer(msg->GetPlayerHandle());
}

//------------------------------------------------------------------------------
/**
    Game started on each player in session, we are ready to run gameplay
*/
void
DefaultMultiplayerNotificationHandler::HandleGameStarted(const Ptr<GameStarted>& msg)
{
    MultiplayerManager::Instance()->SetGameState(MultiplayerManager::Running);
}
//------------------------------------------------------------------------------
/**
*/
void
DefaultMultiplayerNotificationHandler::HandleGameEnded(const Ptr<GameEnded> &msg)
{
	MultiplayerManager::Instance()->SetGameState(MultiplayerManager::Idle);
}

//------------------------------------------------------------------------------
/**
*/
void
DefaultMultiplayerNotificationHandler::HandlePlayerReadyChanged(const Ptr<PlayerReadyChanged>& msg)
{
    MultiplayerManager::Instance()->GetPlayer(msg->GetPlayerHandle())->SetReady(msg->GetReadyState());
}

//------------------------------------------------------------------------------
/**
*/
void
DefaultMultiplayerNotificationHandler::HandleDataReceived(const Ptr<DataReceived>& msg)
{
    // error
    n_error("DefaultMultiplayerNotificationHandler::HandleDataReceived() -> not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void
DefaultMultiplayerNotificationHandler::HandleChatMessage(const Ptr<ChatMessage>& msg)
{
	n_error("DefaultMultiplayerNotificationHandler::HandleChatMessage() -> not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
DefaultMultiplayerNotificationHandler::HandleDisconnectionFromServer()
{
    MultiplayerManager::Instance()->OnDisconnectFromServer();
}

//------------------------------------------------------------------------------
/**
    host invoked a game start, load level and everything to get ready for gameplay
*/
void 
DefaultMultiplayerNotificationHandler::HandleGameStarting(const Ptr<GameStarting>& msg)
{
    MultiplayerManager::Instance()->SetGameState(MultiplayerManager::Starting);
}
//------------------------------------------------------------------------------
/**
    host invoked a game end, unload level and everything to get ready for NOT gameplay
*/
void 
DefaultMultiplayerNotificationHandler::HandleGameEnding(const Ptr<GameEnding>& msg)
{
	MultiplayerManager::Instance()->SetGameState(MultiplayerManager::Stopping);
}
//------------------------------------------------------------------------------
/**
*/
void 
DefaultMultiplayerNotificationHandler::HandleLocalPlayerAdded( const Ptr<LocalPlayerAdded>& msg )
{
    MultiplayerManager::Instance()->HandleLocalPlayerAdded(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
DefaultMultiplayerNotificationHandler::HandleLocalPlayerRemoved( const Ptr<LocalPlayerRemoved>& msg )
{
    MultiplayerManager::Instance()->HandleLocalPlayerRemoved(msg);
}

} // namespace Multiplayer
