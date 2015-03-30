#pragma once
//------------------------------------------------------------------------------
/**
@class Multiplayer::TimeStamp

    A TimeStamp will be same on each system.
    Therefore its platform depended implementation must synchronize its time value.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#if __WIN32__ || __LINUX__
#include "multiplayer/raknet/raknettimestamp.h"
namespace Multiplayer
{
    typedef RakNet::RakNetTimeStamp TimeStamp;
}
#else
#error "TimeStamp class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
