#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::PacketChannel

    ChannelIds for different channels where packets can be send thru.

	(C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#if __WIN32__ || __LINUX__
#include "network/raknet/raknetpacketchannel.h"
namespace Multiplayer
{
    class PacketChannel : public RakNet::RakNetPacketChannel
    {
    };
}
#else
#error "PacketChannel class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
