//------------------------------------------------------------------------------
//  playerbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/raknet/raknetsessioninfo.h"

namespace RakNet
{
//------------------------------------------------------------------------------
/**
*/
RakNetSessionInfo::RakNetSessionInfo()
{

}

//------------------------------------------------------------------------------
/**
*/
Util::String 
RakNetSessionInfo::GetIpAddressaAsString() const
{
    Util::String address(this->externalIp.ToString());
    return address;
}
} // namespace Multiplayer
