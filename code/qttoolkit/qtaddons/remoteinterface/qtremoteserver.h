#pragma once
//------------------------------------------------------------------------------
/**
    @class QtToolkitUtil::QtRemoteServer
    
    Implements a remote interface which should be used between Qt Nebula applications to send messages.
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "net/tcpserver.h"
#include "messaging/message.h"

namespace QtRemoteInterfaceAddon
{
class QtRemoteServer : public Core::RefCounted
{
	__DeclareClass(QtRemoteServer);
	__DeclareSingleton(QtRemoteServer);	
public:
	/// constructor
	QtRemoteServer();
	/// destructor
	virtual ~QtRemoteServer();
	/// open the QtRemoteServer
	bool Open();
	/// close the QtRemoteServer
	void Close();
	/// return if QtRemoteServer is open
	bool IsOpen() const;

	/// updates remote interface, basically checks incoming sockets for packages
	void OnFrame();

	/// sets the address of the listener, this must be set before calling open
	void SetPort(const ushort port);
	/// returns the address of the remote interface
	const ushort GetPort() const;

	
private:
	/// handles a message
	void HandleMessage(const Ptr<Messaging::Message>& msg);

	bool isOpen;
	Ptr<Net::TcpServer> server;
	ushort port;
}; 

//------------------------------------------------------------------------------
/**
*/
inline bool
QtRemoteServer::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
QtRemoteServer::SetPort( const ushort port )
{
	this->port = port;
}

//------------------------------------------------------------------------------
/**
*/
inline const ushort 
QtRemoteServer::GetPort() const
{
	return this->port;
}


} // namespace QtToolkitUtil
//------------------------------------------------------------------------------