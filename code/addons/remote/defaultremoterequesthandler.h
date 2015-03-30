#pragma once
#ifndef REMOTE_DEFAULTREMOTEREQUESTHANDLER_H
#define REMOTE_DEFAULTREMOTEREQUESTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Remote::DefaultRemoteRequestHandler
    
    This implements the default response to remote requests. It will
    answer all Remote requests which are not handled by a custom
    RemoteRequestHandler.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "remote/remoterequesthandler.h"

//------------------------------------------------------------------------------
namespace Remote
{
class DefaultRemoteRequestHandler : public RemoteRequestHandler
{
    __DeclareClass(DefaultRemoteRequestHandler);
public:
    /// handle a remote request, the handler is expected to fill the content stream with response data
    virtual void HandleRequest(const Ptr<RemoteRequest>& request);
};

} // namespace RemoteRequestHandler
//------------------------------------------------------------------------------
#endif
    