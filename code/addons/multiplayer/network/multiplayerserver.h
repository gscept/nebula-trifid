#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::MultiplayerServer
          
    (C) 2015 Individual contributors, see AUTHORS file
*/    
#if __WIN32__ || __LINUX__
#include "network/raknet/raknetinternalmultiplayerserver.h"
namespace Multiplayer
{
    class MultiplayerServer : public RakNet::RakNetMultiplayerServer
    {
        __DeclareClass(MultiplayerServer);
        __DeclareSingleton(MultiplayerServer);
    public:
        /// constructor
        MultiplayerServer();
        /// destructor
        virtual ~MultiplayerServer();
    };
}
#error "MultiplayerServer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
