#pragma once
//------------------------------------------------------------------------------
/**
    @class MultiplayerFeature::NetworkServer       
                
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

//FIXME these should be replaced by own implementations, leave them for the time being
#define DEFAULT_SERVER_PORT 61111
#define DEFAULT_SERVER_ADDRESS "natpunch.jenkinssoftware.com"
#define MASTER_SERVER_ADDRESS "masterserver2.raknet.com"
#define MASTER_SERVER_PORT "80"

#define IS_HOST (MultiplayerFeature::NetworkServer::Instance()->IsHost())

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
class NetworkServer : public Core::RefCounted
{
	__DeclareClass(NetworkServer);
	__DeclareInterfaceSingleton(NetworkServer);
public:

	enum NetworkServerState
	{
		IDLE,
		NETWORK_STARTED,
		IN_LOBBY_WAITING_FOR_HOST_DETERMINATION,
		IN_LOBBY,
		IN_GAME,
	};
    /// constructor
	NetworkServer();
    /// destructor
	virtual ~NetworkServer();

    /// open the RakNetServer 
    void Open();
    /// close the RakNetServer
    void Close();

    /// perform client-side per-frame updates
    void OnFrame();

	/// setup low level network handling, NAT punch, UPNP
	bool SetupLowlevelNetworking();	
	    
	/// shut down low level networking
	void ShutdownLowlevelNetworking();

    /// get rakpeer interface
	RakNet::RakPeerInterface* GetRakPeerInterface() const;

	/// are we the host of the session
	bool IsHost() const;

	/// is the host determined yet
	bool HasHost();	

	/// internal
	void DispatchMessageStream(RakNet::BitStream * msgStream, RakNet::Packet *packet);
	/// internal
	void SendMessageStream(RakNet::BitStream* msgStream);
	/// Allow/Disallow clients to join server while in-game
	void LockInGameJoin(bool flag);
private:
	/// returns status if allowed to join a game
	bool IsInGameJoinUnLocked();

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
	void NatConnect(const RakNet::RakNetGUID &guid);
	/// start searching for games
	void SearchForGames();

	/// start the game
	void StartGame();
	/// create room
	void CreateRoom();
	/// cancel room
	void CancelRoom();
	/// trigger refresh of available rooms on master
	void UpdateRoomList();
	/// deal with a packet
	bool HandlePacket(RakNet::Packet * packet);
	

	/// get replica via network id
 	RakNet::Replica3 * LookupReplica(RakNet::NetworkID replicaId);
	///
	void AddDeferredMessage(RakNet::NetworkID entityId, const Ptr<Messaging::Message> &msg);
	/// hmm, lets have this for the time being
	friend class NetworkGame;

	NetworkServerState state;
	RakNet::RakPeerInterface *rakPeer;	
	Ptr<MultiplayerFeature::ReplicationManager> replicationManager;
	RakNet::NetworkIDManager *networkIDManager;
	RakNet::NatPunchthroughClient *natPunchthroughClient;
	RakNet::RPC4 *rpc;
	RakNet::FullyConnectedMesh2 *fullyConnectedMesh;
	Multiplayer::SyncPoint *readyEvent;	
	RakNet::RakNetGUID natPunchServerGuid;
	RakNet::SystemAddress natPunchServerAddress;		
	Ptr<MasterHelperThread> masterThread;
	Util::String natServer;
	bool connectedToNatPunchThrough;		
	Ptr<Attr::AttributeTable> masterResult;
	Threading::SafeFlag doneFlag;	
	Util::Dictionary<RakNet::NetworkID, Util::Array<Ptr<Messaging::Message>>> deferredMessages;
};

//------------------------------------------------------------------------------
/**
*/
inline
RakNet::RakPeerInterface*
NetworkServer::GetRakPeerInterface() const
{
	return this->rakPeer;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
NetworkServer::IsHost() const
{	
	return this->fullyConnectedMesh->IsHostSystem();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
NetworkServer::HasHost()
{
	return this->state > IN_LOBBY_WAITING_FOR_HOST_DETERMINATION;
}


} // namespace MultiplayerFeature
//------------------------------------------------------------------------------
