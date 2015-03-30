#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::PlayerBase
        
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "uniqueplayerid.h"

namespace Multiplayer
{
    class Session;
}

//------------------------------------------------------------------------------
namespace Base
{
class PlayerBase : public Core::RefCounted
{
    __DeclareClass(PlayerBase);
public:
    enum SignInState
    {
        NotSignedIn = 0,
        SignedInLocally,
        SignedInOnline,

        Unkown
    };
    /// constructor
    PlayerBase();
    /// destructor
    virtual ~PlayerBase();    
    /// get SignInState	
    Base::PlayerBase::SignInState GetSignInState() const;
    /// set SignInState
    void SetSignInState(Base::PlayerBase::SignInState val);
    /// get PlayerName	
    const Util::StringAtom& GetPlayerName() const;
    /// set PlayerName
    void SetPlayerName(const Util::StringAtom& val);

    /// set player ready, overwrite in subclass
    void SetReady(bool val);
    /// is player ready, overwrite in subclass
    bool IsReady() const;

    /// get Id	
    const Multiplayer::UniquePlayerId& GetUnqiueId() const;
    /// set Id
    void SetUniqueId(const Multiplayer::UniquePlayerId& val);

    /// get IsLocal	
    bool IsLocal() const;
    /// set IsLocal
    void SetLocal(bool val);
    /// get IsHost	
    bool IsHost() const;
    /// set IsHost
    void SetHost(bool val);

    /// is player in session
    bool IsInSession() const;
    /// append player info to netstream
    void AppendPlayerInfoToStream(const Ptr<Multiplayer::BitWriter>& writer);
    /// set player info from netstream
    void SetPlayerInfoFromStream(const Ptr<Multiplayer::BitReader>& reader);   
    /// on join session 
    virtual void OnJoinSession(Multiplayer::Session* sessionPtr);
    /// on leave session
    virtual void OnLeaveSession();
    
protected:
    friend class Multiplayer::Session;
    /// get IsInPublicSlot	
    bool GetIsInPublicSlot() const;
    /// set IsInPublicSlot
    void SetIsInPublicSlot(bool val);
   
    SignInState signInState;
    bool isReady;
    bool isLocal;
    bool isHost;
    bool isInPublicSlot; // if false player is in private slot
    Util::StringAtom playerName;
    Multiplayer::UniquePlayerId id;
    Multiplayer::Session* session; // back ptr
};

//------------------------------------------------------------------------------
/**
*/
inline Base::PlayerBase::SignInState 
PlayerBase::GetSignInState() const
{
    return this->signInState;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PlayerBase::SetSignInState(Base::PlayerBase::SignInState val)
{
    this->signInState = val;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
PlayerBase::GetPlayerName() const
{
    return this->playerName;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PlayerBase::SetPlayerName(const Util::StringAtom& val)
{
    this->playerName = val;
}

//------------------------------------------------------------------------------
/**
*/
inline const Multiplayer::UniquePlayerId& 
PlayerBase::GetUnqiueId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PlayerBase::SetUniqueId(const Multiplayer::UniquePlayerId& val)
{
    this->id = val;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
PlayerBase::IsLocal() const
{
    return this->isLocal;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PlayerBase::SetLocal(bool val)
{
    this->isLocal = val;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
PlayerBase::IsInSession() const
{
    return this->session != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
PlayerBase::GetIsInPublicSlot() const
{
    return this->isInPublicSlot;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PlayerBase::SetIsInPublicSlot(bool val)
{
    this->isInPublicSlot = val;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
PlayerBase::IsHost() const
{
    return this->isHost;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
PlayerBase::SetHost(bool val)
{
    this->isHost = val;
}
} // namespace Multiplayer
//------------------------------------------------------------------------------
