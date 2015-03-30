#pragma once
//------------------------------------------------------------------------------
/**
*/
#if __WIN32__ ||  __LINUX__
#include "network/raknet/raknetpacketid.h"
namespace Multiplayer
{
    typedef RakNet::RakNetPackedId PacketId;
}
#else
n_error("PacketId not implemented for this platform!");
#endif