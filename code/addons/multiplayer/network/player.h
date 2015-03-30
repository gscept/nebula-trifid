#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::Player
          
    (C) 2015 Individual contributors, see AUTHORS file
*/    
#if __WIN32__ || __LINUX__
#include "multiplayer/raknet/raknetplayer.h"
namespace Multiplayer
{
    class Player : public RakNet::RakNetPlayer
    {
        __DeclareClass(Player);
    public:
        /// constructor
        Player();
        /// destructor
        virtual ~Player();
    };
}
#else
#error "Player class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
