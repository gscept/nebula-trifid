#pragma once
//------------------------------------------------------------------------------
/**
    @class RakNet::RakNetSessionInfo

    Information about sessions from a game search or a invitation.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "network/base/sessioninfobase.h"
#include "raknet/RakNetTypes.h"

//------------------------------------------------------------------------------
namespace RakNet
{
class RakNetSessionInfo : public Base::SessionInfoBase
{
public:
    /// constructor
    RakNetSessionInfo();        
    /// get RakNetGUID	
    const RakNetGUID& GetRakNetGUID() const;
    /// set RakNetGUID
    void SetRakNetGUID(const RakNetGUID& val);
    /// get ExternalIp	
    const SystemAddress& GetExternalIp() const;
    /// set ExternalIp
    void SetExternalIp(const SystemAddress& val);
    /// get ip address and port as string
    Util::String GetIpAddressaAsString() const;

protected:
    RakNetGUID address; 
    SystemAddress externalIp;
};

//------------------------------------------------------------------------------
/**
*/
inline const RakNetGUID& 
RakNetSessionInfo::GetRakNetGUID() const
{
    return address;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RakNetSessionInfo::SetRakNetGUID(const RakNetGUID& val)
{
    this->address = val;
}

//------------------------------------------------------------------------------
/**
*/
inline const SystemAddress& 
RakNetSessionInfo::GetExternalIp() const
{
    return externalIp;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
RakNetSessionInfo::SetExternalIp(const SystemAddress& val)
{
    this->externalIp = val;
}
} // namespace InternalMultiplayer
//-------------------