//------------------------------------------------------------------------------
//  remoterequesthandler.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remoterequesthandler.h"

namespace Remote
{
__ImplementClass(Remote::RemoteRequestHandler, 'RRHD', Core::RefCounted);

using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
RemoteRequestHandler::RemoteRequestHandler()
{
    this->pendingRequests.SetSignalOnEnqueueEnabled(false);
}

//------------------------------------------------------------------------------
/**
*/
RemoteRequestHandler::~RemoteRequestHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Put a remote request into the request handlers message queue. This method
    is meant to be called from another thread.
*/
void
RemoteRequestHandler::PutRequest(const Ptr<RemoteRequest>& remoteRequest)
{
    this->pendingRequests.Enqueue(remoteRequest);
}

//------------------------------------------------------------------------------
/**
    Handle all pending remote requests in the pending queue. This method
    must be called frequently from the thread which created this
    request handler.
*/
void
RemoteRequestHandler::HandlePendingRequests()
{
    this->pendingRequests.DequeueAll(this->curWorkRequests);
    IndexT i;
    for (i = 0; i < this->curWorkRequests.Size(); i++)
    {
        this->HandleRequest(this->curWorkRequests[i]);
        this->curWorkRequests[i]->SetHandled(true);
    }
}

//------------------------------------------------------------------------------
/**
    Overwrite this method in your subclass. This method will be called by the
    RemoteServer if AcceptsRequest() returned true. The request handler should
    properly process the request by filling the responseContentStream with
    data (for instance a HTML page), set the MediaType on the 
    responseContentStream (for instance "text/html") and return with a
    RemoteStatus code (usually RemoteStatus::OK).
*/
void
RemoteRequestHandler::HandleRequest(const Ptr<RemoteRequest>& request)
{
    request->SetStatus(RemoteStatus::NotFound);
}

} // namespace Remote
