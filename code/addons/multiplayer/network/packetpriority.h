#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::PacketPriority

	(C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#if __WIN32__ || __LINUX__
#include "network/raknet/raknetpacketpriority.h"
namespace Multiplayer
{
    class PacketPriority : public RakNet::RakNetPacketPriority
    {
    };
}
#else
#error "PacketPriority class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
