#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::Session

    A Session allows joining and leaving of players.
    It encloses Multiplayer states like GameLobby and GamePlay. 
         
    (C) 2015 Individual contributors, see AUTHORS file
*/    
#include "core/refcounted.h"
#include "multiplayer/player.h"
#include "multiplayer/handle.h"
#include "multiplayer/sessioninfo.h"

namespace Base
{
    class MultiplayerServerBase;
}
//------------------------------------------------------------------------------
namespace Multiplayer
{

class Session : public Core::RefCounted
{
    __DeclareClass(Session);
public:
    /// constructor
    Session();
    /// destructor
    virtual ~Session();
    /// clear session, removes all players
    void ClearSession();
    /// start this session
    void StartSession();
    /// end this session
    void EndSession();
    /// is session started
    bool IsStarted() const;
    /// get SessionInfo	
    const Multiplayer::SessionInfo& GetSessionInfo() const;
    /// set SessionInfo
    void SetSessionInfo(const Multiplayer::SessionInfo& val);
    /// get players in session
    Util::Array<Ptr<Player> >  GetPlayers(bool noHost) const;
    /// get player by unique id
    const Ptr<Player>& GetPlayerByUniqueId(const UniquePlayerId& id) const;  
    /// get HostPlayer	
    const Ptr<Multiplayer::Player>& GetHostPlayer() const;

protected:
    friend class Base::MultiplayerServerBase;
    /// add player to session
    void AddPlayer(const Ptr<Multiplayer::Player>& player, bool publicSlot = true);
    /// remove player from session
    void RemovePlayer(const Ptr<Multiplayer::Player>& player);
    /// set host player
    void SetHostPlayer(const Ptr<Multiplayer::Player>& player);

    Multiplayer::SessionInfo sessionInfo;
    bool started;
    Util::Dictionary<UniquePlayerId, Ptr<Multiplayer::Player> > joinedPlayers;  
    Ptr<Multiplayer::Player> hostPlayer;
};

//------------------------------------------------------------------------------
/**
*/
inline const Multiplayer::SessionInfo& 
Session::GetSessionInfo() const
{
    return this->sessionInfo;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Session::SetSessionInfo(const Multiplayer::SessionInfo& val)
{
    this->sessionInfo = val;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Ptr<Player> > 
Session::GetPlayers(bool noLocalPlayers) const
{
    if (noLocalPlayers)
    {
        Util::Array<Ptr<Player> > playersWithoutHost = this->joinedPlayers.ValuesAsArray();
        IndexT i;
        for (i = 0; i < playersWithoutHost.Size(); ++i)
        {
        	if (playersWithoutHost[i]->IsLocal())
            {
                playersWithoutHost.EraseIndex(i);
                --i;
            }
        }
        return playersWithoutHost;
    }
    return this->joinedPlayers.ValuesAsArray();
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Session::IsStarted() const
{
    return this->started;
}  

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Multiplayer::Player>& 
Session::GetHostPlayer() const
{
    return this->hostPlayer;
}   

//------------------------------------------------------------------------------
/**
*/
inline void 
Session::SetHostPlayer(const Ptr<Multiplayer::Player>& player)
{
    this->hostPlayer = player;
}
} // namespace Multiplayer
//------------------------------------------------------------------------------