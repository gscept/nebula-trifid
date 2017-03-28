#pragma once
//------------------------------------------------------------------------------
/**
    @class MultiplayerFeature::NetworkServer       
                
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include "util/dictionary.h"
#include "timing/time.h"
#include "replicationmanager.h"
#include "threading/thread.h"
#include "threading/safeflag.h"
#include "attr/attributetable.h"
#include "syncpoint.h"
#include "messaging/message.h"
#include "RakPeerInterface.h"
#include "game/entity.h"
#include "networkentity.h"

//FIXME
#define IS_HOST (MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetServer()->IsHost())

namespace RakNet
{
	class RakPeerInterface;
	class NetworkIDManager;
	class TCPInterface;	
	class RPC4;		
	class ReadyEvent;	
}
//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class NetworkServer : public Core::RefCounted
{
	__DeclareClass(NetworkServer);	
public:

	enum NetworkServerState
	{
		IDLE,
		NETWORK_STARTED,
		IN_LOBBY_WAITING_FOR_HOST_DETERMINATION,
		SERVER_LOBBY,
		IN_LOBBY,
		IN_GAME,
	};
    /// constructor
	NetworkServer();
    /// destructor
	virtual ~NetworkServer();

    /// open the RakNetServer 
    virtual void Open();
    /// close the RakNetServer
    virtual void Close();

    /// perform client-side per-frame updates
    virtual void OnFrame();

	/// setup low level network handling
	virtual bool SetupLowlevelNetworking();
	    
	/// shut down low level networking
	virtual void ShutdownLowlevelNetworking();

    /// get rakpeer interface
	RakNet::RakPeerInterface* GetRakPeerInterface() const;

	/// are we the host of the session
	virtual bool IsHost() const = 0;

	/// is the host determined yet
	virtual bool HasHost() const = 0;	
	/// start searching for games
	virtual void SearchForGames() = 0;
	///
	virtual void Connect(const RakNet::RakNetGUID &guid) = 0;
    ///
    virtual void ConnectDirect(const RakNet::SystemAddress &addr) = 0;

	/// internal
	void DispatchMessageStream(RakNet::BitStream * msgStream, RakNet::Packet *packet);
	/// internal
	void SendMessageStream(RakNet::BitStream* msgStream);
	/// Allow/Disallow clients to join server while in-game
	void LockInGameJoin(bool flag);

	///
	static NetworkServer* Instance();

	///
	virtual NetworkServerState GetState() const;

	/// get entity by a network id
	Ptr<Game::Entity> GetEntityByNetworkID(const MultiplayerFeature::NetworkID &id);
	
protected:
	/// returns status if allowed to join a game
	bool IsInGameJoinUnLocked();

	

	/// start the game
	virtual void StartGame();
	/// create room
	virtual void CreateRoom();
	/// cancel room
	virtual void CancelRoom();
	
	/// deal with a packet
	virtual bool HandlePacket(RakNet::Packet * packet) = 0;
	
	/// parse a json response
	void ParseServerResponse(const Util::String & response);

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
	RakNet::RPC4 *rpc;	
	Multiplayer::SyncPoint *readyEvent;				
	Util::Dictionary<RakNet::NetworkID, Util::Array<Ptr<Messaging::Message>>> deferredMessages;
	Util::Array<RakNet::RakNetGUID> participants;
	Ptr<Attr::AttributeTable> masterResult;
};

//------------------------------------------------------------------------------
/**
*/
inline
NetworkServer::NetworkServerState
NetworkServer::GetState() const
{
	return this->state;
}

//------------------------------------------------------------------------------
/**
*/
inline
RakNet::RakPeerInterface*
NetworkServer::GetRakPeerInterface() const
{
	return this->rakPeer;
}

} // namespace MultiplayerFeature
//------------------------------------------------------------------------------
