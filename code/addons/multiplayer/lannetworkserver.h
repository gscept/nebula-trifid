#pragma once
//------------------------------------------------------------------------------
/**
    @class MultiplayerFeature::LanNetworkServer       
                
    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "util/dictionary.h"
#include "timing/time.h"
#include "threading/thread.h"
#include "threading/safeflag.h"
#include "attr/attributetable.h"
#include "messaging/message.h"
#include "networkserver.h"

namespace RakNet
{
	class RakPeerInterface;
	class NetworkIDManager;
	class TCPInterface;
	class RPC4;	
	class HTTPConnection2;
	class ReadyEvent;
}
//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class LanNetworkServer : public MultiplayerFeature::NetworkServer
{
	__DeclareClass(LanNetworkServer);
	__DeclareInterfaceSingleton(LanNetworkServer);
public:
	
    /// constructor
	LanNetworkServer();
    /// destructor
	virtual ~LanNetworkServer();

    /// open the RakNetServer 
    virtual void Open();
    /// close the RakNetServer
	virtual void Close();

    /// perform client-side per-frame updates
	virtual void OnFrame();

	/// connect to server using 
	void Connect(const RakNet::RakNetGUID &guid);
    ///
    void ConnectDirect(const RakNet::SystemAddress &addr);

	/// setup low level network handling, NAT punch, UPNP
	virtual bool SetupLowlevelNetworking();
	    
	/// shut down low level networking
	virtual void ShutdownLowlevelNetworking();

    /// are we the host of the session
	virtual bool IsHost() const;

	/// is the host determined yet
	virtual bool HasHost() const;

	/// start searching for games
	virtual void SearchForGames();

private:


	
	/// start the game
	virtual void StartGame();
	/// create room
	virtual void CreateRoom();
	/// cancel room
	virtual void CancelRoom();
	/// deal with a packet
	virtual bool HandlePacket(RakNet::Packet * packet);
	
	/// update the json string with server data used for broadcasts
	void UpdateServerInfo();
	
	/// hmm, lets have this for the time being
	friend class NetworkGame;
	RakNet::Time lastBroadcast;
	bool host;
	bool serverStarted;	
	Util::String serverInfoString;
};



//------------------------------------------------------------------------------
/**
*/
inline
bool
LanNetworkServer::IsHost() const
{	
	return this->host;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
NetworkServer::HasHost() const
{
	return this->state > IN_LOBBY_WAITING_FOR_HOST_DETERMINATION;
}


} // namespace MultiplayerFeature
//------------------------------------------------------------------------------
