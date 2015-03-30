//------------------------------------------------------------------------------
//  Player.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "multiplayer/player.h"

namespace Multiplayer
{
#if __WIN32__ || __LINUX__
__ImplementClass(Multiplayer::Player, 'MPPL', RakNet::RakNetPlayer);
#else
#error "Player class not implemented on this platform!"
#endif

//------------------------------------------------------------------------------
/**
*/
Player::Player()
{
}

//------------------------------------------------------------------------------
/**
*/
Player::~Player()
{
}

} // namespace Multiplayer
