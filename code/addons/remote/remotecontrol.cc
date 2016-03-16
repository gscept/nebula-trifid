//------------------------------------------------------------------------------
//  remotecontrol.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remotecontrol.h"
#include "remote/remoterequestreader.h"
#include "io/memorystream.h"
#include "io/xmlreader.h"
#include "net/messageclientconnection.h"
#include "util/commandlineargs.h"

namespace Remote
{
__ImplementClass(Remote::RemoteControl, 'RECO', Core::RefCounted);
__ImplementSingleton(Remote::RemoteControl);

using namespace Util;
using namespace Net;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
RemoteControl::RemoteControl() :
    isOpen(false),
    portNum(2102)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
RemoteControl::~RemoteControl()
{
    n_assert(!this->IsOpen());
    __DestructSingleton;        
}

//------------------------------------------------------------------------------
/**
*/
bool
RemoteControl::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    
    // setup a new TcpServer object
    this->tcpServer = TcpServer::Create();
    this->tcpServer->SetAddress(IpAddress("any", this->portNum));    
    this->tcpServer->SetClientConnectionClass(MessageClientConnection::RTTI);
    bool success = this->tcpServer->Open();

    // create default request handler
    this->defaultRequestHandler = DefaultRemoteRequestHandler::Create();

    return success;
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteControl::Close()
{
    n_assert(this->isOpen);

    // clear pending requests
    this->pendingRequests.Clear();

    // destroy the default remote request handler
    this->defaultRequestHandler = 0;

    // remove request handlers
    this->requestHandlers.Clear();

    // shutdown TcpServer
    this->tcpServer->Close();
    this->tcpServer = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteControl::AttachRequestHandler(const Ptr<RemoteRequestHandler>& requestHandler)
{
    n_assert(requestHandler.isvalid());
    n_assert(this->isOpen);
    n_assert(!this->requestHandlers.Contains(requestHandler->GetControllerName()));
    this->requestHandlers.Add(requestHandler->GetControllerName(), requestHandler);
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteControl::RemoveRequestHandler(const Ptr<RemoteRequestHandler>& requestHandler)
{
    n_assert(requestHandler.isvalid());
    n_assert(this->isOpen);
    this->requestHandlers.Erase(requestHandler->GetControllerName());
}

//------------------------------------------------------------------------------
/**
*/
void
RemoteControl::OnFrame()
{
    n_assert(this->isOpen);

    // check for incoming messages
    Array<Ptr<TcpClientConnection> > recvClients;
    do 
    {
        recvClients = this->tcpServer->Recv();
        IndexT clientIndex;
        for (clientIndex = 0; clientIndex < recvClients.Size(); clientIndex++)
        {
            // check if this is an XML or a text message
            bool isXmlMessage = false;
            String result;

            const Ptr<Stream>& recvStream = recvClients[clientIndex]->GetRecvStream();
            recvStream->SetAccessMode(Stream::ReadAccess);
            if (recvStream->GetSize() > 5)
            {
                if (recvStream->Open())
                {
                    uchar* rawBytes = (uchar*) recvStream->Map();
                    if ((rawBytes[0] == '<') && 
                        (rawBytes[1] == '?') &&
                        (rawBytes[2] == 'x') && 
                        (rawBytes[3] == 'm') && 
                        (rawBytes[4] == 'l'))
                    {
                        isXmlMessage = true;
                    }
                    recvStream->Unmap();
                    recvStream->Close();
                }
            }

            // handle message either as XML command or as raw text command
            if (isXmlMessage)
            {
                if (!this->HandleRemoteRequest(recvClients[clientIndex]))                
                {   
                    recvClients[clientIndex]->Shutdown();
                }
            }
            else
            {
                // non xml cmds
                Ptr<TextReader> textReader = TextReader::Create();
                textReader->SetStream(recvStream);
                if (textReader->Open())
                {
                    String cmdStr = textReader->ReadAll();
                    n_printf("RemoteControl: received '%s'\n", cmdStr.AsCharPtr());
                    textReader->Close();
                    CommandLineArgs cmdLineArgs(cmdStr.AsCharPtr());
                    IndexT i;
                    for (i = 0; i < this->commandHandlers.Size(); i++)
                    {
                        this->commandHandlers[i]->PutCommand(cmdLineArgs);
                    }
                }
                result = "ok";
            }

            if (result.IsValid())
            {
                // send back result
                const Ptr<Stream>& sendStream = recvClients[clientIndex]->GetSendStream();
                Ptr<TextWriter> textWriter = TextWriter::Create();
                textWriter->SetStream(sendStream);
                if (textWriter->Open())
                {
                    textWriter->WriteString(result);
                    textWriter->Close();
                }
                recvClients[clientIndex]->Send();
            }
        }
    }
    while (!recvClients.IsEmpty());

    // handle processed remote requests
    IndexT i;
    for (i = 0; i < this->pendingRequests.Size();)
    {
        const Ptr<RemoteRequest>& remoteRequest = this->pendingRequests[i].remoteRequest;
        if (remoteRequest->Handled())
        {
            const Ptr<TcpClientConnection>& conn = this->pendingRequests[i].clientConnection;
            if (this->BuildRemoteResponse(conn, remoteRequest))
            {
                conn->Send();
            }
            this->pendingRequests.EraseIndex(i);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
RemoteControl::HandleRemoteRequest(const Ptr<TcpClientConnection>& clientConnection)  
{
    // decode the request
    Ptr<RemoteRequestReader> remoteRequestReader = RemoteRequestReader::Create();
    remoteRequestReader->SetStream(clientConnection->GetRecvStream());
    if (remoteRequestReader->Open())
    {
        remoteRequestReader->ReadRequest();
        remoteRequestReader->Close();
    }
    if (remoteRequestReader->IsValidRemoteRequest())
    {
        // create a content stream for the response
        Ptr<MemoryStream> responseContentStream = MemoryStream::Create();
        
        // build a RemoteRequest object
        Ptr<RemoteRequest> remoteRequest = RemoteRequest::Create();

        remoteRequest->SetXmlReader(remoteRequestReader->GetXmlReader());
        remoteRequest->SetResponseContentStream(responseContentStream.upcast<Stream>());
        remoteRequest->SetStatus(RemoteStatus::NotFound);

        // find a request handler which accepts the request
        Ptr<RemoteRequestHandler> requestHandler;
        Util::String controllerName = remoteRequestReader->GetControllerName();
        if (this->requestHandlers.Contains(controllerName))
        {
            requestHandler = this->requestHandlers[controllerName];
        }
        if (requestHandler.isvalid())
        {
            // asynchronously handle the request
            requestHandler->PutRequest(remoteRequest);
        }
        else
        {
            // no request handler accepts the request, let the default
            // request handler handle the request
            this->defaultRequestHandler->HandleRequest(remoteRequest);
            remoteRequest->SetHandled(true);
        }

        // append request to pending queue
        PendingRequest pendingRequest;
        pendingRequest.clientConnection = clientConnection;
        pendingRequest.remoteRequest = remoteRequest;
        this->pendingRequests.Append(pendingRequest);

        return true;
    }
    else
    {
        // the received data was not a valid Remote request
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
RemoteControl::BuildRemoteResponse(const Ptr<TcpClientConnection>& conn, const Ptr<RemoteRequest>& remoteRequest)
{
    Ptr<RemoteResponseWriter> responseWriter = RemoteResponseWriter::Create();
    responseWriter->SetStream(conn->GetSendStream());
    responseWriter->SetStatusCode(remoteRequest->GetStatus());
    if (RemoteStatus::OK != remoteRequest->GetStatus())
    {
        // an error occured, need to write an error message to the response stream
        Ptr<TextWriter> textWriter = TextWriter::Create();
        textWriter->SetStream(remoteRequest->GetResponseContentStream());
        textWriter->Open();
        textWriter->WriteFormatted("%s %s", RemoteStatus::ToString(remoteRequest->GetStatus()).AsCharPtr(), RemoteStatus::ToHumanReadableString(remoteRequest->GetStatus()).AsCharPtr());
        textWriter->Close();
        remoteRequest->GetResponseContentStream()->SetMediaType(MediaType("text/plain"));
    }
    if (remoteRequest->GetResponseContentStream()->GetSize() > 0)
    {
        remoteRequest->GetResponseContentStream()->GetMediaType().IsValid();
        responseWriter->SetContent(remoteRequest->GetResponseContentStream());
    }
    responseWriter->Open();
    responseWriter->WriteResponse();
    responseWriter->Close();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
RemoteControl::AttachCommandHandler(const Ptr<RemoteCommandHandler>& h)
{               
    n_assert(h.isvalid());
    n_assert(this->isOpen);
    this->commandHandlers.Append(h);    
}

//------------------------------------------------------------------------------
/**
*/
void 
RemoteControl::RemoveCommandHandler(const Ptr<RemoteCommandHandler>& h)
{   
    n_assert(h.isvalid());
    n_assert(this->isOpen);
    IndexT cmdHandlerIndex = this->commandHandlers.FindIndex(h);
    n_assert(InvalidIndex != cmdHandlerIndex);
    this->commandHandlers.EraseIndex(cmdHandlerIndex);
}

void RemoteControl::SendXMLMessageToQt(const Ptr<IO::Stream>& _sendStream)
{
	this->tcpServer->Broadcast(_sendStream);
}		
} // namespace Remote

