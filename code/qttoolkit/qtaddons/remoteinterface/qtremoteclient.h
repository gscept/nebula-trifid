#pragma once
//------------------------------------------------------------------------------
/**
    @class QtToolkitUtil::QtRemoteClient
    
    A remote client acts as a sender of messages to a QtRemoteServer.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <QObject>
#include "core/refcounted.h"
#include "net/socket/socket.h"
#include "messaging/message.h"
#include "util/queue.h"
namespace QtRemoteInterfaceAddon
{
class QtRemoteClient : 
	public QObject,
	public Core::RefCounted
{
	Q_OBJECT
	__DeclareClass(QtRemoteClient);	
public:
	/// constructor
	QtRemoteClient();
	/// destructor
	virtual ~QtRemoteClient();

	/// opens client, set port first
	bool Open();
	/// closes client
	void Close();
	/// returns true if client is connected
	const bool IsConnected() const;
	/// returns true if client is open
	const bool IsOpen() const;

	/// registers in global database
	void Register(const Util::String& name);
	/// unregisters from global database
	void Unregister(const Util::String& name);
	/// sets the port
	void SetPort(const ushort port);
	/// get the port
	const ushort GetPort() const;

	/// sends a message to the socket, assumes the socket is open
	void Send(const Ptr<Messaging::Message>& message);

	/// updates client, sends queued messages
	void OnFrame();

	/// access to named client
	static const Ptr<QtRemoteClient>& GetClient(const Util::String& name);

signals:
	/// called whenever the remote client gets disconnected
	void OnDisconnected();
private:
	Util::Queue<Ptr<Messaging::Message> > messageQueue;
	Ptr<Net::Socket> socket;
	ushort port;
	bool isOpen;

	static Util::Dictionary<Util::String, Ptr<QtRemoteClient> > clients;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const bool 
QtRemoteClient::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
QtRemoteClient::SetPort( const ushort port )
{
	this->port = port;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<QtRemoteClient>& 
QtRemoteClient::GetClient( const Util::String & name)
{
	return clients[name];
}
//------------------------------------------------------------------------------
/**
*/
inline const ushort 
QtRemoteClient::GetPort() const
{
	return this->port;
}

} // namespace QtToolkitUtil
//------------------------------------------------------------------------------