//------------------------------------------------------------------------------
//  qtremoteclient.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "qtremoteclient.h"
#include "io/memorystream.h"
#include "io/binarywriter.h"
#include "net/socket/ipaddress.h"
#include "remoteinterface/qtremoteprotocol.h"

using namespace Net;
using namespace Util;
using namespace IO;

Util::Dictionary<Util::String, Ptr<QtRemoteInterfaceAddon::QtRemoteClient>> QtRemoteInterfaceAddon::QtRemoteClient::clients;


namespace QtRemoteInterfaceAddon
{
__ImplementClass(QtRemoteInterfaceAddon::QtRemoteClient, 'QTRC', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
QtRemoteClient::QtRemoteClient() :
	isOpen(false)
{	
	// empty
}

//------------------------------------------------------------------------------
/**
*/
QtRemoteClient::~QtRemoteClient()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
QtRemoteClient::Open()
{
	n_assert(!this->IsOpen());

	// setup socket
	this->socket = Socket::Create();
	this->socket->SetAddress(IpAddress("localhost", this->port));
	bool opened = this->socket->Open(Socket::TCP);
	if (opened)
	{
		Socket::Result res = this->socket->Connect();
		if (res == Socket::Success)
		{
			this->isOpen = true;
			return true;
		}
	}
	
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
QtRemoteClient::Register(const Util::String & name)
{
	n_assert(!clients.Contains(name));
	clients.Add(name, this);
}

//------------------------------------------------------------------------------
/**
*/
void 
QtRemoteClient::Unregister( const Util::String& name )
{
	n_assert(clients.Contains(name));
	clients.Erase(name);
}

//------------------------------------------------------------------------------
/**
*/
void 
QtRemoteClient::Close()
{
	n_assert(this->IsOpen()); 
	this->isOpen = false;
	this->messageQueue.Clear();
	this->socket->Close();
	this->socket = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
QtRemoteClient::Send( const Ptr<Messaging::Message>& message )
{
	// add it to message queue, we can add messages to the queue even if the connection isn't open
	// every message will be treated in the OnFrame function
	this->messageQueue.Enqueue(message);
}

//------------------------------------------------------------------------------
/**
*/
void 
QtRemoteClient::OnFrame()
{
	// we constantly ask the socket if it's connected, and if it isn't
	if (this->IsOpen())
	{
		if (this->socket->IsConnected())
		{
			// always send keep-alive message
			Ptr<KeepAlive> msg = KeepAlive::Create();
			this->messageQueue.Enqueue(msg.upcast<Messaging::Message>());

			// create stream
			Ptr<MemoryStream> stream = MemoryStream::Create();
			stream->SetAccessMode(Stream::ReadWriteAccess);

			// create binary writer
			Ptr<BinaryWriter> writer = BinaryWriter::Create();
			writer->SetStream(stream.upcast<Stream>());

			while (!this->messageQueue.IsEmpty())
			{
				// open writer
				writer->Open();

				// take message
				const Ptr<Messaging::Message>& message = this->messageQueue.Dequeue();

				// first write down four-cc of message
				writer->WriteUInt(message->GetClassFourCC().AsUInt());

				// encode message
				message->Encode(writer);

				// map stream
				void* data = stream->Map();
				SizeT size = stream->GetSize();

				// send message
				SizeT sent = 0;
				Socket::Result res = this->socket->Send(data, size, sent);

				// test-send, if it fails, we close down the client
				if (res == Socket::Error)
				{
					this->Close();
					emit this->OnDisconnected();
				}

				// close stream
				writer->Close();
			}
		}
	}
}


} // namespace QtToolkitUtil