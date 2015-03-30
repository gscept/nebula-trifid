//------------------------------------------------------------------------------
//  netstream.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/netstream.h"

namespace Multiplayer
{
#if __WIN32__ || __LINUX__
__ImplementClass(Multiplayer::NetStream, 'NEST', RakNet::RakNetStream);
#error "NetStream class not implemented on this platform!"
#endif
}