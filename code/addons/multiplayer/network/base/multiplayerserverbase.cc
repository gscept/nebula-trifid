//------------------------------------------------------------------------------
//  multiplayerserverbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/base/multiplayerserverbase.h"
#include "network/base/multiplayernotificationhandlerbase.h"
#include "network/playerinfo.h"

namespace Base
{
__ImplementClass(Base::MultiplayerServerBase, 'IMSB', Core::RefCounted);
__ImplementSingleton(Base::MultiplayerServerBase);

using namespace Util;
using namespace Multiplayer;

//------------------------------------------------------------------------------
/**
*/
MultiplayerServerBase::MultiplayerServerBase() :
    isOpen(false),
    isHosting(false),
    curMultiplayerType(MultiplayerType::CoachCoop)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MultiplayerServerBase::~MultiplayerServerBase()
{
    n_assert(!this->isOpen);
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerServerBase::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MultiplayerServerBase::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}    

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnFrame()
{
#if NEBULA3_ENABLE_PROFILING
    this->UpdateStatistics();
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnHostSession(const Ptr<Multiplayer::HostSession>& msg)
{
    n_error("MultiplayerServerBase::OnHostSession called");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnStartGameSearch(const Ptr<Multiplayer::StartGameSearch>& msg)
{
    n_error("MultiplayerServerBase::OnStartGameSearch called");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnJoinSession(const Ptr<Multiplayer::JoinSession>& msg)
{
    n_error("MultiplayerServerBase::OnJoinSession called");
}


//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnAddLocalPlayer( const Ptr<Multiplayer::AddLocalPlayer>& msg )
{
    // session exists? one slot free?
    n_assert(this->activeSession.isvalid());
    n_assert(this->activeSession->GetSessionInfo().GetOpenPrivateSlots() > 0 ||
             this->activeSession->GetSessionInfo().GetOpenPublicSlots() > 0);

    // create  players
    Ptr<Multiplayer::Player> player = Multiplayer::Player::Create();    
    player->SetPlayerName(Util::StringAtom("LocalTestPlayer" + Util::String::FromInt(this->localPlayers.Size())));
    player->SetSignInState(Multiplayer::Player::SignedInOnline);
    player->SetLocal(true);
    // directly add to our server
    this->AddLocalPlayer(player); 

    this->activeSession->AddPlayer(player);

    // show sign in gui to allow user to sign in
    // TODO: handle sign in, need friend / buddy system
    Multiplayer::PlayerInfo newInfo;
    newInfo.gamerTag = player->GetPlayerName();
    newInfo.playerHandle = player.get();
    newInfo.signInState = player->GetSignInState();

    // send notification             
    Ptr<Multiplayer::LocalPlayerAdded> notifyMsg = Multiplayer::LocalPlayerAdded::Create();
    notifyMsg->SetPlayerInfo(newInfo);
    this->PutNotification(notifyMsg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnRemoveLocalPlayer( const Ptr<Multiplayer::RemoveLocalPlayer>& msg )
{
    // from server
    this->RemoveLocalPlayer(msg->GetPlayerHandle().cast<Multiplayer::Player>());

    // from active session
    this->activeSession->RemovePlayer(msg->GetPlayerHandle().cast<Multiplayer::Player>());

    // send notification             
    Ptr<Multiplayer::LocalPlayerRemoved> notifyMsg = Multiplayer::LocalPlayerRemoved::Create();
    notifyMsg->SetPlayerHandle(msg->GetPlayerHandle());
    this->PutNotification(notifyMsg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Multiplayer::Player> 
MultiplayerServerBase::GetPlayerByUniqueId(const Multiplayer::UniquePlayerId& uid)
{
    IndexT i;
    for (i = 0; i < this->players.Size(); ++i)
    {
        const UniquePlayerId& playerId = this->players[i]->GetUnqiueId();
        if (uid == playerId)
        {
            return this->players[i];
        }
    }
    return Ptr<Player>();
}

//------------------------------------------------------------------------------
/**
    Invoked from MAINTHREAD
*/
void 
MultiplayerServerBase::OnAttachNotificationHandler(const Ptr<Multiplayer::AttachNotificationHandler>& msg)
{
    n_assert(msg->GetNotificationHandler().isvalid());
    this->notificationHandler = msg->GetNotificationHandler();
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnRemoveNotificationHandler(const Ptr<Multiplayer::RemoveNotificationHandler>& msg)
{
    this->notificationHandler = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::PutNotification(const Ptr<Messaging::Message>& msg)
{
    n_assert(this->notificationHandler.isvalid());
    this->notificationHandler->PutNotification(msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnSetPlayerReady(const Ptr<Multiplayer::SetPlayerReady>& msg)
{
    n_error("MultiplayerServerBase::OnSetPlayerReady called");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnStartGame(const Ptr<Multiplayer::StartGame>& msg)
{
    n_error("MultiplayerServerBase::OnStartGame called");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnEndGame(const Ptr<Multiplayer::EndGame>& msg)
{
    n_error("MultiplayerServerBase::OnEndGame called");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnSendDataToPlayer(const Ptr<Multiplayer::SendDataToPlayer>& msg)
{
    n_error("MultiplayerServerBase::OnSendDataToPlayer called");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnBroadcastData(const Ptr<Multiplayer::BroadcastData>& msg)
{
    n_error("MultiplayerServerBase::OnBroadcastData called");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::AddPlayer(const Ptr<Multiplayer::Player>& player)
{    
    // all players 
    if (this->players.FindIndex(player) == InvalidIndex)
    {
        this->players.Append(player);
    }
    
    if (this->activeSession.isvalid())
    {
        // if any non local player joins we must have a valid session         
        this->activeSession->AddPlayer(player);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::AddPlayers(const Util::Array<Ptr<Multiplayer::Player> >& players)
{
    IndexT i;
    for (i = 0; i < players.Size(); ++i)
    {      
        this->AddPlayer(players[i]);	
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::AddPlayerHandles(const Util::Array<Multiplayer::PlayerHandle>& playerHandles)
{
    IndexT i;
    for (i = 0; i < playerHandles.Size(); ++i)
    {
        const Ptr<Player>& player = playerHandles[i].cast<Player>();        
        this->AddPlayer(player);	
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::RemovePlayer(const Ptr<Multiplayer::Player>& player)
{
    // first remove from any session
    if (player->IsInSession())
    {
        // if any non local player leaves we must have a valid session
        n_assert(this->activeSession.isvalid());
        this->activeSession->RemovePlayer(player);
    }

    IndexT arrayIndex = this->players.FindIndex(player);
    n_assert(arrayIndex != InvalidIndex);
    this->players.EraseIndex(arrayIndex);
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::AddLocalPlayer(const Ptr<Multiplayer::Player>& player)
{
    n_assert(this->localPlayers.FindIndex(player) == InvalidIndex);
    n_assert(player->IsLocal());
    this->localPlayers.Append(player);    
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::RemoveLocalPlayer(const Ptr<Multiplayer::Player>& player)
{
    IndexT arrayIndex = this->localPlayers.FindIndex(player);
    n_assert(arrayIndex != InvalidIndex);
    this->localPlayers.EraseIndex(arrayIndex);
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnStartingGameFinished(const Ptr<Multiplayer::StartingGameFinished>& msg)
{
    n_error("MultiplayerServerBase::OnStartingGameFinished called!");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::OnSendDataToHost(const Ptr<Multiplayer::SendDataToHost>& msg)
{                                      
    n_error("MultiplayerServerBase::OnSendDataToHost called!");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::UpdateStatistics()
{     
    n_error("MultiplayerServerBase::UpdateStatistics called!");
}

//------------------------------------------------------------------------------
/**
*/
void 
MultiplayerServerBase::WriteStatistics(const Ptr<Http::HtmlPageWriter>& writer)
{      
    n_error("MultiplayerServerBase::WriteStatistics called!");      
}
} // namespace Multiplayer
