#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::SessionInfoBase

    Information about sessions from a game search

	(C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
//------------------------------------------------------------------------------
#include "core/types.h"
#include "network/multiplayertype.h"
#include "util/string.h"

namespace Base
{
class SessionInfoBase
{
public:
    /// constructor
    SessionInfoBase();

    /// get OpenPublicSlots	
    ushort GetOpenPublicSlots() const;
    /// set OpenPublicSlots
    void SetOpenPublicSlots(ushort val);
    /// get OpenPrivateSlots	
    ushort GetOpenPrivateSlots() const;
    /// set OpenPrivateSlots
    void SetOpenPrivateSlots(ushort val);
    /// get FilledPublicSlots	
    ushort GetFilledPublicSlots() const;
    /// set FilledPublicSlots
    void SetFilledPublicSlots(ushort val);
    /// get FilledPrivateSlots	
    ushort GetFilledPrivateSlots() const;
    /// set FilledPrivateSlots
    void SetFilledPrivateSlots(ushort val);
    /// get MultiplayerType	
    Multiplayer::MultiplayerType::Code GetMultiplayerType() const;
    /// set MultiplayerType
    void SetMultiplayerType(Multiplayer::MultiplayerType::Code val);
    /// get ip address and port as string
    Util::String GetIpAddressaAsString() const;

protected:
    ushort openPublicSlots;
    ushort openPrivateSlots;
    ushort filledPublicSlots;
    ushort filledPrivateSlots;
    Multiplayer::MultiplayerType::Code curSessionType;
};

//------------------------------------------------------------------------------
/**
*/
inline ushort 
SessionInfoBase::GetOpenPublicSlots() const
{
    return this->openPublicSlots;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
SessionInfoBase::SetOpenPublicSlots(ushort val)
{
    this->openPublicSlots = val;
}

//------------------------------------------------------------------------------
/**
*/
inline ushort 
SessionInfoBase::GetOpenPrivateSlots() const
{
    return this->openPrivateSlots;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
SessionInfoBase::SetOpenPrivateSlots(ushort val)
{
    this->openPrivateSlots = val;
}

//------------------------------------------------------------------------------
/**
*/
inline ushort 
SessionInfoBase::GetFilledPublicSlots() const
{
    return this->filledPublicSlots;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
SessionInfoBase::SetFilledPublicSlots(ushort val)
{
    this->filledPublicSlots = val;
}

//------------------------------------------------------------------------------
/**
*/
inline ushort 
SessionInfoBase::GetFilledPrivateSlots() const
{
    return this->filledPrivateSlots;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
SessionInfoBase::SetFilledPrivateSlots(ushort val)
{
    this->filledPrivateSlots = val;
}

//------------------------------------------------------------------------------
/**
*/
inline Multiplayer::MultiplayerType::Code
SessionInfoBase::GetMultiplayerType() const
{
    return this->curSessionType;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
SessionInfoBase::SetMultiplayerType(Multiplayer::MultiplayerType::Code val)
{
    this->curSessionType = val;
} 
}
// namespace Multiplayer
