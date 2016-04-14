//------------------------------------------------------------------------------
//  QtRemoteServer.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "qtremoteserver.h"
#include "net/messageclientconnection.h"
#include "net/socket/ipaddress.h"
#include "net/tcpserver.h"
#include "messaging/staticmessagehandler.h"
#include "core/factory.h"

using namespace Net;
using namespace IO;
using namespace Util;
namespace QtRemoteInterfaceAddon
{

__ImplementClass(QtRemoteInterfaceAddon::QtRemoteServer, 'QRMS', Core::RefCounted);
__ImplementSingleton(QtRemoteInterfaceAddon::QtRemoteServer);

//------------------------------------------------------------------------------
/**
*/
QtRemoteServer::QtRemoteServer() :
	isOpen(false),
	server(0),
	port(2102)
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
QtRemoteServer::~QtRemoteServer()
{
	if (this->IsOpen())
	{
		this->Close();
	}
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
QtRemoteServer::Open()
{
	n_assert(!this->IsOpen());

	// create tcp server and open it
	this->server = TcpServer::Create();
	this->server->SetAddress(IpAddress("any", this->port));
	this->server->SetClientConnectionClass(TcpClientConnection::RTTI);
	bool opened = this->server->Open();

	this->isOpen = opened;
	return opened;
}

//------------------------------------------------------------------------------
/**
*/
void
QtRemoteServer::Close()
{
	n_assert(this->IsOpen());
	this->server->Close();
	this->server = 0;
	this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
QtRemoteServer::OnFrame()
{
	n_assert(this->IsOpen());

	Array<Ptr<TcpClientConnection> > clients;
	do 
	{
		// get clients
		clients = this->server->Recv();

		// iterate over them
		IndexT clientIndex;
		for (clientIndex = 0; clientIndex < clients.Size(); clientIndex++)
		{
			// get stream
			const Ptr<Stream>& recvStream = clients[clientIndex]->GetRecvStream();
			recvStream->SetAccessMode(Stream::ReadAccess);
			
			// create binary reader
			Ptr<BinaryReader> reader = BinaryReader::Create();
			reader->SetStream(recvStream);
			reader->Open();
			
			while (!reader->Eof())
			{
				// first read fourCC
				FourCC msgFourCC(reader->ReadUInt());
				if (Core::Factory::Instance()->ClassExists(msgFourCC))
				{
					// now we quite simply just decode it
					Ptr<Messaging::Message> msg = (Messaging::Message*)Core::Factory::Instance()->Create(msgFourCC);

					// decode and handle message
					msg->Decode(reader);
					this->HandleMessage(msg);
				}				
			}
			
			// close reader
			reader->Close();			
			
		}
	} 
	while (!clients.IsEmpty());
	
}

//------------------------------------------------------------------------------
/**
*/
void 
QtRemoteServer::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	__Dispatch(QtRemoteServer, this, msg);
}

}
