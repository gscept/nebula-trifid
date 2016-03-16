#pragma once
#ifndef REMOTE_REMOTECONTROLPROXY_H
#define REMOTE_REMOTECONTROLPROXY_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteControlProxy
    
    Client-side proxy of the RemoteControl. Client threads create and
    attach RemoteRequestHandlers to their RemoteControlProxy. The RemoteControlProxy 
    receives incoming remote requests from the remote thread, and lets
    its RemoteRequestHandlers process the request in the client thread's
    context, then sends the result back to the remote thread.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "remote/remoterequesthandler.h"
#include "remote/remotecommandhandler.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteControlProxy : public Core::RefCounted
{
    __DeclareClass(RemoteControlProxy);
    __DeclareSingleton(RemoteControlProxy);
public:
    /// constructor
    RemoteControlProxy();
    /// destructor
    virtual ~RemoteControlProxy();

    /// open the server proxy
    void Open();
    /// close the server proxy
    void Close();
    /// return true if open
    bool IsOpen() const;

    /// attach a request handler to the server
    void AttachRequestHandler(const Ptr<Remote::RemoteRequestHandler>& h);
    /// remove a request handler from the server
    void RemoveRequestHandler(const Ptr<Remote::RemoteRequestHandler>& h);
    /// handle pending remote requests, call this method frequently
    void HandlePendingRequests();

    /// attach command handler for non-xml commands
    void AttachCommandHandler(const Ptr<Remote::RemoteCommandHandler>& h);
    /// remove command handler for non-xml commands
    void RemoveCommandHandler(const Ptr<Remote::RemoteCommandHandler>& h);
    /// handle pending remote commands, call this method frequently
    void HandlePendingCommands();

private:
    Util::Array<Ptr<RemoteRequestHandler> > requestHandlers;
    Util::Array<Ptr<RemoteCommandHandler> > commandHandlers;
    bool isOpen;
};        

//------------------------------------------------------------------------------
/**
*/
inline bool
RemoteControlProxy::IsOpen() const
{
    return this->isOpen;
}

} // namespace Remote
//------------------------------------------------------------------------------
#endif
    