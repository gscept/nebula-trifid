#pragma once
#ifndef REMOTE_REMOTEREQUESTHANDLER_H
#define REMOTE_REMOTEREQUESTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteRequestHandler
    
    RemoteRequestHandlers are attached to the RemoteServer and process
    incoming XML requests. When an Remote request comes in, the
    RemoteServer asks every attached RemoteRequestHandler until the first one
    accepts the request. If the RemoteRequestHandler accepts the request
    its HandleRequest() method will be called with a pointer to a content
    stream. The request handler is expected to write the response to the
    content stream.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "remote/remoterequest.h"
#include "io/uri.h"
#include "io/stream.h"
#include "util/string.h"
#include "util/stringatom.h"
#include "threading/safequeue.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteRequestHandler : public Core::RefCounted
{
    __DeclareClass(RemoteRequestHandler);
public:
    /// constructor
    RemoteRequestHandler();
    /// destructor
    virtual ~RemoteRequestHandler();

    /// get a human readable name of the request handler
    const Util::String& GetName() const;
    /// get a human readable description of the request handler
    const Util::String& GetDesc() const;
    /// get the controller component name of this handler
    const Util::StringAtom& GetControllerName() const;

protected:
    friend class RemoteControl;
    friend class RemoteControlProxy;

    /// handle a remote request, overwrite this method in you subclass
    virtual void HandleRequest(const Ptr<RemoteRequest>& request);
    /// handle all pending requests, called by local-thread's RemoteServerProxy
    void HandlePendingRequests();
    /// put a request to the pending queue, called by RemoteServer thread
    void PutRequest(const Ptr<RemoteRequest>& remoteRequest);
    /// set human readable name of the request handler
    void SetName(const Util::String& n);
    /// set human readable description
    void SetDesc(const Util::String& d);
    /// set the controller component name of this handler
    void SetControllerName(const Util::StringAtom& l);

    Util::String name;
    Util::String desc;
    // used as identifier of handler
    Util::StringAtom controllerName;
    Threading::SafeQueue<Ptr<RemoteRequest> > pendingRequests;       
    Util::Array<Ptr<RemoteRequest> > curWorkRequests;
};

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteRequestHandler::SetName(const Util::String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
RemoteRequestHandler::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteRequestHandler::SetDesc(const Util::String& d)
{
    this->desc = d;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
RemoteRequestHandler::GetDesc() const
{
    return this->desc;
}

//------------------------------------------------------------------------------
/**
*/
inline void
RemoteRequestHandler::SetControllerName(const Util::StringAtom& l)
{
    this->controllerName = l;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
RemoteRequestHandler::GetControllerName() const
{
    return this->controllerName;
}

} // namespace Remote
//------------------------------------------------------------------------------
#endif

