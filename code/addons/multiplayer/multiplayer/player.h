#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::Player

    This class represents a player. its a logical copy of the internalplayer
    in the network namespace running in the network thread
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "internalmultiplayer/base/internalplayerbase.h"
#include "internalmultiplayer/handle.h"
#include "util/blob.h"

//------------------------------------------------------------------------------
namespace Multiplayer
{
class MultiplayerManager;
class DefaultMultiplayerNotificationHandler;

class Player : public Core::RefCounted
{
    __DeclareClass(Player);
public:
    /// constructor
    Player();
    /// destructor
    virtual ~Player();       

    /// get gamer tag
    const Util::StringAtom& GetGamerTag() const;
    /// get login state
    Base::InternalPlayerBase::SignInState GetSignState() const;
    /// get the player handle
    const InternalMultiplayer::PlayerHandle& GetPlayerHandle() const;
    /// get player ready 
    bool IsReady() const;
    /// get is local flag
    bool IsLocal() const; 
    /// get UniqueIdBlob	
    const Util::Blob& GetUniqueIdBlob() const;

private:
    friend class MultiplayerManager;
    friend class DefaultMultiplayerNotificationHandler;

    /// set gamer tag
    void SetGamerTag(const Util::StringAtom& tag);
    /// set login state
    void SetSignInState(Base::InternalPlayerBase::SignInState state);
    /// set the player handle
    void SetHandle(const InternalMultiplayer::PlayerHandle& handle);
    /// set player ready
    void SetReady(bool r);
    /// set is local flag
    void SetLocal(bool val);       
    /// set UniqueIdBlob
    void SetUniqueIdBlob(const Util::Blob& val);

    InternalMultiplayer::PlayerHandle handle;

    Util::StringAtom gamerTag;
    Base::InternalPlayerBase::SignInState loginState;
    bool ready;
    bool local;
    Util::Blob uniqueIdBlob;
};

//------------------------------------------------------------------------------
/**
*/
inline void
Player::SetHandle(const InternalMultiplayer::PlayerHandle& handle)
{
    this->handle = handle;
}

//------------------------------------------------------------------------------
/**
*/
inline const InternalMultiplayer::PlayerHandle&
Player::GetPlayerHandle() const
{
    return this->handle;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Player::SetGamerTag(const Util::StringAtom& tag)
{
    this->gamerTag = tag;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
Player::GetGamerTag() const
{
    return this->gamerTag;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Player::SetSignInState(Base::InternalPlayerBase::SignInState state)
{
    this->loginState = state;
}

//------------------------------------------------------------------------------
/**
*/
inline Base::InternalPlayerBase::SignInState
Player::GetSignState() const
{
    return this->loginState;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Player::SetReady(bool r)
{
    this->ready = r;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
Player::IsReady() const
{
    return this->ready;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Player::SetLocal(bool val)
{
    this->local = val;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Player::IsLocal() const
{
    return this->local;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Player::SetUniqueIdBlob(const Util::Blob& val)
{
    this->uniqueIdBlob = val;
}
         
//------------------------------------------------------------------------------
/**
*/
inline const Util::Blob&
Player::GetUniqueIdBlob() const
{
    return this->uniqueIdBlob;
}
}; // namespace Multiplayer
//------------------------------------------------------------------------------