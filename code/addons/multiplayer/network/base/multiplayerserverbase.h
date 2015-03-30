#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::MultiplayerServerBase

    The platform specific MultiplayerServer 
    creates and manages the network system of the platform.
        
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "network/multiplayertype.h"
#include "network/session.h"
#include "http/html/htmlpagewriter.h"

//------------------------------------------------------------------------------
namespace Base
{
class MultiplayerServerBase : public Core::RefCounted
{
    __DeclareClass(MultiplayerServerBase);
    __DeclareSingleton(MultiplayerServerBase);
public:
    /// constructor
    MultiplayerServerBase();
    /// destructor
    virtual ~MultiplayerServerBase();

    /// open the Multiplayer server 
    void Open();
    /// close the Multiplayer server
    void Close();
    /// return true if Multiplayer server is open
    bool IsOpen() const;
    /// perform client-side per-frame updates
    void OnFrame();

    /// on create player
    void OnCreatePlayers(const Ptr<Multiplayer::CreatePlayers>& msg);
    /// on host a session
    void OnSetGameType(/*const Ptr<Multiplayer::SetGameType>& msg*/);
    /// on host a session
    void OnHostSession(const Ptr<Multiplayer::HostSession>& msg);
    /// on kick a player
    void OnKickPlayer(/*const Ptr<Multiplayer::KickPlayer>& msg*/);
    /// on delete session call
    void OnDeleteSession(const Ptr<Multiplayer::DeleteSession>& msg);

    /// on start game search
    void OnStartGameSearch(const Ptr<Multiplayer::StartGameSearch>& msg);

    /// on join a given session
    void OnJoinSession(const Ptr<Multiplayer::JoinSession>& msg);
    /// on set player ready
    void OnSetPlayerReady(const Ptr<Multiplayer::SetPlayerReady>& msg);
    /// on leave current session
    void OnLeaveSession(const Ptr<Multiplayer::LeaveSession>& msg);

    /// add a local player
    void OnAddLocalPlayer(const Ptr<Multiplayer::AddLocalPlayer>& msg);
    /// remove a local player
    void OnRemoveLocalPlayer(const Ptr<Multiplayer::RemoveLocalPlayer>& msg);

    /// on start game
    void OnStartGame(const Ptr<Multiplayer::StartGame>& msg);
    /// on end game called from host
    void OnEndGame(const Ptr<Multiplayer::EndGame>& msg);
    /// client: on ready of starting a game
    void OnStartingGameFinished(const Ptr<Multiplayer::StartingGameFinished>& msg);

    /// on send data to specific player
    void OnSendDataToPlayer(const Ptr<Multiplayer::SendDataToPlayer>& msg);
    /// on send data to all players
    void OnBroadcastData(const Ptr<Multiplayer::BroadcastData>& msg);    
    /// on send data to host
    void OnSendDataToHost(const Ptr<Multiplayer::SendDataToHost>& msg);

    /// get player by unique id
    Ptr<Multiplayer::Player> GetPlayerByUniqueId(const Multiplayer::UniquePlayerId& uid);

    /// attach notification handler
    void OnAttachNotificationHandler(const Ptr<Multiplayer::AttachNotificationHandler>& msg);
    /// remove notification handler
    void OnRemoveNotificationHandler(const Ptr<Multiplayer::RemoveNotificationHandler>& msg);
    /// write statistics to html writer, overwrite in subclass
    void WriteStatistics(const Ptr<Http::HtmlPageWriter>& writer);
    /// get current session
    const Ptr<Multiplayer::Session>& GetSession() const;

protected:

    /// put notification
    void PutNotification(const Ptr<Messaging::Message>& msg);
    /// add player, could be local or not, could be in session or not
    void AddLocalPlayer(const Ptr<Multiplayer::Player>& player);
    /// add player, could be local or not, could be in session or not
    void AddPlayer(const Ptr<Multiplayer::Player>& player);
    /// add player, could be local or not, could be in session or not
    void AddPlayerHandles(const Util::Array<Multiplayer::PlayerHandle>& playerHandles);
    /// add multiple players, could be local or not, could be in session or not
    void AddPlayers(const Util::Array<Ptr<Multiplayer::Player> >& players);
    /// remove player
    void RemovePlayer(const Ptr<Multiplayer::Player>& player);
    /// remove player
    void RemoveLocalPlayer(const Ptr<Multiplayer::Player>& player);
    /// is system hosting?
    bool IsSystemHosting() const;
    /// update statistics, overwrite in subclass
    virtual void UpdateStatistics();

    bool isOpen;
    bool isHosting;
    Ptr<Multiplayer::Session> activeSession;  
    Multiplayer::MultiplayerType::Code curMultiplayerType;
    Util::Array<Ptr<Multiplayer::Player> > players;
    Util::Array<Ptr<Multiplayer::Player> > localPlayers;
    Ptr<Base::MultiplayerNotificationHandlerBase> notificationHandler;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MultiplayerServerBase::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Multiplayer::Session>& 
MultiplayerServerBase::GetSession() const
{
    return this->activeSession;
}
} // namespace Multiplayer
//------------------------------------------------------------------------------
