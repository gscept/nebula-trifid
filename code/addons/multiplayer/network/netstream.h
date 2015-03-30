#pragma once
/**
    @class Multiplayer::NetStream
                    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#if __WIN32__ || __LINUX__
#include "network/raknet/raknetstream.h"
namespace Multiplayer
{
    class NetStream : public RakNet::RakNetStream
    {
        __DeclareClass(NetStream);
    };
}
#else
n_error("NetStream not implemented for this platform!");
#endif