//------------------------------------------------------------------------------
//  multiplayerserver.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/multiplayerserver.h"

namespace Multiplayer
{
#if __WIN32__ || __LINUX__
    __ImplementClass(Multiplayer::MultiplayerServer, 'MPSV', RakNet::RakNetMultiplayerServer);
    __ImplementSingleton(Multiplayer::MultiplayerServer);
#endif
#else
#error "MultiplayerServer class not implemented on this platform!"
#endif

//------------------------------------------------------------------------------
/**
*/
MultiplayerServer::MultiplayerServer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MultiplayerServer::~MultiplayerServer()
{
    __DestructSingleton;
}

} // namespace Multiplayer
