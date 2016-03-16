#pragma once
//------------------------------------------------------------------------------
/**
    @class MultiplayerFeature::NatNetworkServer       
                
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "util/dictionary.h"
#include "timing/time.h"
#include "replicationmanager.h"
#include "threading/thread.h"
#include "FullyConnectedMesh2.h"
#include "threading/safeflag.h"
#include "attr/attributetable.h"
#include "syncpoint.h"
#include "messaging/message.h"
#include "networkserver.h"

//FIXME these should be replaced by own implementations, leave them for the time being
#define DEFAULT_SERVER_PORT 61111
#define DEFAULT_SERVER_ADDRESS "natpunch.jenkinssoftware.com"
#define MASTER_SERVER_ADDRESS "masterserver2.raknet.com"
#define MASTER_SERVER_PORT "80"

namespace RakNet
{
	class RakPeerInterface;
	class NetworkIDManager;
	class TCPInterface;
	class NatPunchthroughClient;
	class RPC4;	
	class HTTPConnection2;
	class ReadyEvent;
	class CloudClient;
}
//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class NatNetworkServer : public MultiplayerFeature::NetworkServer
{
	__DeclareClass(NatNetworkServer);
	__DeclareInterfaceSingleton(NatNetworkServer);
public:
	
    /// constructor
	NatNetworkServer();
    /// destructor
	virtual ~NatNetworkServer();

    /// open the RakNetServer 
    virtual void Open();
    /// close the RakNetServer
    virtual void Close();

    /// perform client-side per-frame updates
    virtual void OnFrame();

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


	class MasterHelperThread : public Threading::Thread
	{
		__DeclareClass(MasterHelperThread)			
	public:
		Util::String gameId;
	protected:
		/// implements the actual listener method
		virtual void DoWork();
	};
	
	friend class MasterHelperThread;

	/// callback function for masterserver worker
	void MasterServerResult(Util::String response);

	/// connect to server using nat punchthrough
	void Connect(const RakNet::RakNetGUID &guid);
    ///
    void ConnectDirect(const RakNet::SystemAddress &addr);


	/// start the game
	virtual void StartGame();
	/// create room
	virtual void CreateRoom();
	/// cancel room
	virtual void CancelRoom();
	/// trigger refresh of available rooms on master
	void UpdateRoomList();
	/// deal with a packet
	virtual bool HandlePacket(RakNet::Packet * packet);
	///
	void PublishToMaster();
	///
	void UnpublishFromMaster();
	/// hmm, lets have this for the time being
	friend class NetworkGame;

	int masterServerRow;
	RakNet::NatPunchthroughClient *natPunchthroughClient;	
	RakNet::FullyConnectedMesh2 *fullyConnectedMesh;	
	RakNet::RakNetGUID natPunchServerGuid;
	RakNet::SystemAddress natPunchServerAddress;		
	Ptr<MasterHelperThread> masterThread;
	Util::String natServer;
	bool connectedToNatPunchThrough;			
	Threading::SafeFlag doneFlag;		
	bool delayedMaster;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
NatNetworkServer::IsHost() const
{	
	return this->fullyConnectedMesh->IsHostSystem();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
NatNetworkServer::HasHost() const
{
	return this->state > IN_LOBBY_WAITING_FOR_HOST_DETERMINATION;
}


} // namespace MultiplayerFeature
//------------------------------------------------------------------------------
