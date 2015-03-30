#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::Session

    This class represents a session running in the network thread
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file	
*/
#include "core/refcounted.h"
#include "internalmultiplayer/handle.h"
#include "internalmultiplayer/multiplayertype.h"
#include "multiplayer/player.h"

//------------------------------------------------------------------------------
namespace Multiplayer
{
class MultiplayerManager;
class DefaultMultiplayerNotificationHandler;

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

    /// get public player by index
    const Ptr<Player>& GetPublicPlayerByIndex(IndexT index) const;
    /// get private player by index
    const Ptr<Player>& GetPrivatePlayerByIndex(IndexT index) const;

    /// get all public players
    const Util::Array< Ptr<Player> >& GetPublicPlayers() const;
    /// get all private players
    const Util::Array< Ptr<Player> >& GetPrivatePlayers() const;

    /// get number of remote players
    SizeT GetNumRemotePlayers() const;
    /// are all players ready?
    bool AreAllPlayersReady() const;

    /// get private slot amount
    SizeT GetPrivateSlotSize() const;
    /// get public slot amount
    SizeT GetPublicSlotSize() const;

    /// get private slot amount
    InternalMultiplayer::MultiplayerType::Code GetType() const;

    /// is empty
    bool IsSessionEmpty(bool onlyLocal) const;

private:
    friend class MultiplayerManager;
    friend class DefaultMultiplayerNotificationHandler;

    /// set the session handle
    void SetSessionHandle(const InternalMultiplayer::SessionHandle& handle);
    /// get the session handle
    const InternalMultiplayer::SessionHandle& GetSessionHandle() const;

     /// add player to session
    void AddPlayerPublic(const Ptr<Player>& player);
    /// add player private to session
    void AddPlayerPrivate(const Ptr<Player>& player);

    /// remove public player
    void RemovePlayer(const InternalMultiplayer::PlayerHandle& handle);

   /// set private slot amount
    void SetPrivateSlotSize(SizeT size);
    /// set public slot amount
    void SetPublicSlotSize(SizeT size);
    /// set private slot amount
    void SetType(InternalMultiplayer::MultiplayerType::Code type);

    InternalMultiplayer::SessionHandle handle;
    Util::Array< Ptr<Player> > publicPlayers;
    Util::Array< Ptr<Player> > privatePlayers;

    SizeT privateSlots;
    SizeT publicSlots;
    InternalMultiplayer::MultiplayerType::Code type;
};  

//------------------------------------------------------------------------------
/**
*/
inline void
Session::SetSessionHandle(const InternalMultiplayer::SessionHandle& handle)
{
    this->handle = handle;
}

//------------------------------------------------------------------------------
/**
*/
inline const InternalMultiplayer::SessionHandle& 
Session::GetSessionHandle() const
{
    return this->handle;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Session::AddPlayerPrivate(const Ptr<Player> &player)
{
    n_assert(this->publicPlayers.FindIndex(player) == InvalidIndex);
    n_assert(this->privatePlayers.FindIndex(player) == InvalidIndex);
    n_assert(this->privatePlayers.Size() + 1 <= this->privateSlots);
    this->privatePlayers.Append(player);
}

//------------------------------------------------------------------------------
/**
*/
inline void
Session::AddPlayerPublic(const Ptr<Player> &player)
{
    n_assert(this->publicPlayers.FindIndex(player) == InvalidIndex);
    n_assert(this->privatePlayers.FindIndex(player) == InvalidIndex);
    n_assert(this->publicPlayers.Size() + 1 <= this->publicSlots);
    this->publicPlayers.Append(player);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Player>& 
Session::GetPublicPlayerByIndex(IndexT index) const
{
    n_assert(index < this->publicPlayers.Size());
    return this->publicPlayers[index];
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Player>& 
Session::GetPrivatePlayerByIndex(IndexT index) const
{
    n_assert(index < this->privatePlayers.Size());
    return this->privatePlayers[index];
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array< Ptr<Player> >&
Session::GetPublicPlayers() const
{
    return this->publicPlayers;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array< Ptr<Player> >&
Session::GetPrivatePlayers() const
{
    return this->privatePlayers;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Session::SetPrivateSlotSize(SizeT size)
{
    this->privateSlots = size;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
Session::GetPrivateSlotSize() const
{
    return this->privateSlots;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Session::SetPublicSlotSize(SizeT size)
{
    this->publicSlots = size;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
Session::GetPublicSlotSize() const
{
    return publicSlots;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Session::SetType(InternalMultiplayer::MultiplayerType::Code type)
{
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline InternalMultiplayer::MultiplayerType::Code
Session::GetType() const
{
    return this->type;
}
//------------------------------------------------------------------------------
/**
*/
inline SizeT 
Session::GetNumRemotePlayers() const
{
    SizeT count = 0;
    IndexT i;
    for (i = 0; i < this->publicPlayers.Size(); ++i)
    {
    	if (!this->publicPlayers[i]->IsLocal())count++;
    }
    for (i = 0; i < this->privatePlayers.Size(); ++i)
    {
        if (!this->privatePlayers[i]->IsLocal())count++;
    }

    return count;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Session::AreAllPlayersReady() const
{
    bool allReady = true;
    IndexT i;
    for (i = 0; i < this->publicPlayers.Size(); ++i)
    {
        allReady &= this->publicPlayers[i]->IsReady();
    }
    for (i = 0; i < this->privatePlayers.Size(); ++i)
    {
        allReady &= this->privatePlayers[i]->IsReady();
    }
    return allReady;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Session::IsSessionEmpty(bool onlyLocal) const
{
    if (onlyLocal)
    {
        SizeT numLocals = 0;
        IndexT i;
        for (i = 0; i < this->publicPlayers.Size(); ++i)
        {
            if (this->publicPlayers[i]->IsLocal()) numLocals++;
        }
        for (i = 0; i < this->privatePlayers.Size(); ++i)
        {
            if (this->privatePlayers[i]->IsLocal()) numLocals++;
        } 
        return numLocals == 0;
    }
    return this->publicPlayers.Size() == 0 && this->privatePlayers.Size() == 0;
}
}; // namespace Multiplayer
//------------------------------------------------------------------------------