//------------------------------------------------------------------------------
//  NetworkPageHandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/debug/networkpagehandler.h"

namespace Debug
{
#if __WIN32__ || __LINUX__
    __ImplementClass(Debug::NetworkPageHandler, 'DNPH', RakNet::RakNetNetworkPageHandler);    
#else
#error "NetworkPageHandler class not implemented on this platform!"
#endif

//------------------------------------------------------------------------------
/**
*/
NetworkPageHandler::NetworkPageHandler()
{
}

//------------------------------------------------------------------------------
/**
*/
NetworkPageHandler::~NetworkPageHandler()
{
}

} // namespace Debug
