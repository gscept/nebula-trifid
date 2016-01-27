#pragma once
#ifndef REMOTE_REMOTEMESSAGEHANDLER_H
#define REMOTE_REMOTEMESSAGEHANDLER_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteMessageHandler
    
    Runs the RemoteServer thread, and owns the central remote server. Processes
    messages sent to the RemoteInterface from other threads.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"
#include "io/console.h"
#include "remote/remotecontrol.h"
#include "remote/remoteprotocol.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteMessageHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(RemoteMessageHandler);
public:
    /// constructor
    RemoteMessageHandler();
    /// destructor
    virtual ~RemoteMessageHandler();

    /// open the handler
    virtual void Open();
    /// close the handler
    virtual void Close();
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);
    /// do per-frame work
    virtual void DoWork();
    /// set port number
    virtual void SetPortNumber(ushort portNumber);
    /// get port number
    virtual ushort GetPortNumber() const;

private:
    /// handle AttachRequestHandler message
    void OnAttachRequestHandler(const Ptr<AttachRequestHandler>& msg);
    /// handle RemoveRequestHandler message
    void OnRemoveRequestHandler(const Ptr<RemoveRequestHandler>& msg);
    /// handle AttachCommandHandler message
    void OnAttachCommandHandler(const Ptr<AttachCommandHandler>& msg);
    /// handle RemoveCommandHandler message
    void OnRemoveCommandHandler(const Ptr<RemoveCommandHandler>& msg);
	/// handle SendToQt message
	void OnSendToQt(const Ptr<SendToQt>& msg);

    Ptr<RemoteControl> remoteControl;
    ushort portNum;
};

//------------------------------------------------------------------------------
/**	
*/
inline void
RemoteMessageHandler::SetPortNumber(ushort portNumber)
{
    n_assert(!this->isOpen);
    this->portNum = portNumber;
}

//------------------------------------------------------------------------------
/**	
*/
inline ushort
RemoteMessageHandler::GetPortNumber() const
{
    return this->portNum;
}

} // namespace Remote
//------------------------------------------------------------------------------
#endif
