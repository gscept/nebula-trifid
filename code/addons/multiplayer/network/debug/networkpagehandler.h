#pragma once
//------------------------------------------------------------------------------
/**
@class Multiplayer::NetworkPageHandler

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#if __WIN32__ || __LINUX__
#include "network/debug/raknet/raknetnetworkpagehandler.h"
namespace Debug
{
class NetworkPageHandler : public RakNet::RakNetNetworkPageHandler
{
	__DeclareClass(NetworkPageHandler);
public:
	/// constructor
	NetworkPageHandler();
	/// destructor
	virtual ~NetworkPageHandler();
};
}
#else
#error "NetworkPageHandler class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
