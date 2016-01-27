#pragma once
#ifndef REMOTE_REMOTECONTROL_H
#define REMOTE_REMOTECONTROL_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteControl
    
    Implements an Remote server with attached RemoteRequestHandlers. This is 
    used to remote control the application over a TCP/IP connection.
    Its target application is to control nebula games from external
    tools like animation or cut scene editors.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "net/tcpserver.h"
#include "net/socket/ipaddress.h"
#include "io/textreader.h"
#include "io/textwriter.h"
#include "remote/remoteresponsewriter.h"
#include "remote/remoterequesthandler.h"
#include "remote/remotecommandhandler.h"
#include "remote/defaultremoterequesthandler.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteControl : public Core::RefCounted
{
    __DeclareClass(RemoteControl);
    __DeclareSingleton(RemoteControl);
public:
    /// constructor
    RemoteControl();
    /// destructor
    virtual ~RemoteControl();
    
    /// open the remote server
    bool Open();
    /// close the remote server
    void Close();
    /// return true if server is open
    bool IsOpen() const;
    
    /// Set the used portnumber for connections
    void SetPortNumber(ushort portNumber);
    /// Get the used portnumber for connections
    ushort GetPortNumber() const;

    /// attach a request handler to the server
    void AttachRequestHandler(const Ptr<RemoteRequestHandler>& h);
    /// remove a request handler from the server
    void RemoveRequestHandler(const Ptr<RemoteRequestHandler>& h);
    /// get registered request handlers
    Util::Array<Ptr<RemoteRequestHandler> > GetRequestHandlers() const;

    /// attach command handler
    void AttachCommandHandler(const Ptr<RemoteCommandHandler>& h);
    /// remove command handler
    void RemoveCommandHandler(const Ptr<RemoteCommandHandler>& h);
    /// get registered command handlers
    Util::Array<Ptr<RemoteCommandHandler> > GetCommandHandlers() const;

    /// call this method frequently to serve http connections
    void OnFrame();

	/// sends a stream containing a xml message to the level editor gui
	void SendXMLMessageToQt(const Ptr<IO::Stream>& _sendStream);

private:
    /// handle an RemoteRequest
    bool HandleRemoteRequest(const Ptr<Net::TcpClientConnection>& clientConnection);
    /// build an RemoteResponse for a handled remote request
    bool BuildRemoteResponse(const Ptr<Net::TcpClientConnection>& clientConnection, const Ptr<RemoteRequest>& remoteRequest);

    struct PendingRequest
    {
        Ptr<Net::TcpClientConnection> clientConnection;
        Ptr<RemoteRequest> remoteRequest;
    };

    Util::Dictionary<Util::StringAtom, Ptr<RemoteRequestHandler> > requestHandlers;
    Util::Array<Ptr<RemoteCommandHandler> > commandHandlers;
    Ptr<DefaultRemoteRequestHandler> defaultRequestHandler;
    Ptr<Net::TcpServer> tcpServer;
    Util::Array<PendingRequest> pendingRequests;
    bool isOpen;
    ushort portNum;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
RemoteControl::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**	
*/
inline void
RemoteControl::SetPortNumber(ushort portNumber)
{
    n_assert(!this->isOpen);
    this->portNum = portNumber;
}

//------------------------------------------------------------------------------
/**	
*/
inline ushort
RemoteControl::GetPortNumber() const
{
    return this->portNum;
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Ptr<RemoteRequestHandler> >
RemoteControl::GetRequestHandlers() const
{
    return this->requestHandlers.ValuesAsArray();
}

//------------------------------------------------------------------------------
/**
*/
inline Util::Array<Ptr<RemoteCommandHandler> >
RemoteControl::GetCommandHandlers() const
{
    return this->commandHandlers;
}

} // namespace Remote
//------------------------------------------------------------------------------
#endif
    