//------------------------------------------------------------------------------
//  remotemessagehandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remotemessagehandler.h"

namespace Remote
{
__ImplementClass(Remote::RemoteMessageHandler, 'REMH', Interface::InterfaceHandlerBase);

using namespace Interface;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
RemoteMessageHandler::RemoteMessageHandler() :
    portNum(2102)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
RemoteMessageHandler::~RemoteMessageHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteMessageHandler::Open()
{   
    n_assert(!this->IsOpen());
    InterfaceHandlerBase::Open();

    // setup core runtime and central remote server
    this->remoteControl = RemoteControl::Create();
    this->remoteControl->SetPortNumber(this->portNum);
    this->remoteControl->Open();
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteMessageHandler::Close()
{
    n_assert(this->IsOpen());

    this->remoteControl->Close();
    this->remoteControl = 0;

    InterfaceHandlerBase::Close();
}

//------------------------------------------------------------------------------
/**
    Triggers the remote server from time to time.
*/
void
RemoteMessageHandler::DoWork()
{
    n_assert(this->IsOpen());

    // only process remote requests once in a while
    this->remoteControl->OnFrame();
    n_sleep(0.1f);
}

//------------------------------------------------------------------------------
/**
*/
bool
RemoteMessageHandler::HandleMessage(const Ptr<Message>& msg)
{
    n_assert(msg.isvalid());

    if (msg->CheckId(AttachRequestHandler::Id))
    {
        this->OnAttachRequestHandler(msg.cast<AttachRequestHandler>());
    }
    else if (msg->CheckId(RemoveRequestHandler::Id))
    {
        this->OnRemoveRequestHandler(msg.cast<RemoveRequestHandler>());
    }
    else if (msg->CheckId(AttachCommandHandler::Id))
    {
        this->OnAttachCommandHandler(msg.cast<AttachCommandHandler>());
    }
    else if (msg->CheckId(RemoveCommandHandler::Id))
    {
        this->OnRemoveCommandHandler(msg.cast<RemoveCommandHandler>());
    }
	else if (msg->CheckId(SendToQt::Id))
	{
		this->OnSendToQt(msg.cast<SendToQt>());
	}
    else
    {
        // unknown message
        return false;
    }
    // fallthrough: message was handled
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteMessageHandler::OnAttachRequestHandler(const Ptr<AttachRequestHandler>& msg)
{
    n_printf("RemoteMessageHandler::OnAttachRequestHandler() called!\n");
    this->remoteControl->AttachRequestHandler(msg->GetRequestHandler());
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteMessageHandler::OnRemoveRequestHandler(const Ptr<RemoveRequestHandler>& msg)
{
    n_printf("RemoteMessageHandler::OnRemoveRequestHandler() called!\n");
    this->remoteControl->RemoveRequestHandler(msg->GetRequestHandler());
}

//------------------------------------------------------------------------------
/**
*/
void 
RemoteMessageHandler::OnAttachCommandHandler(const Ptr<AttachCommandHandler>& msg)
{
    n_printf("RemoteMessageHandler::OnAttachCommandHandler() called!\n");
    this->remoteControl->AttachCommandHandler(msg->GetCommandHandler());
}

//------------------------------------------------------------------------------
/**
*/
void 
RemoteMessageHandler::OnRemoveCommandHandler(const Ptr<RemoveCommandHandler>& msg)
{   
    n_printf("RemoteMessageHandler::OnRemoveCommandHandler() called!\n");
    this->remoteControl->RemoveCommandHandler(msg->GetCommandHandler());
}

//------------------------------------------------------------------------------
/**
*/
void 
RemoteMessageHandler::OnSendToQt( const Ptr<SendToQt>& msg )
{
	n_printf("RemoteMessageHandler::OnSendToQt() called!\n");
	this->remoteControl->SendXMLMessageToQt(msg->GetSendStream());
}
} // namespace Remote
