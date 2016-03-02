#pragma once
//------------------------------------------------------------------------------
/**
@class MultiplayerFeature::NetworkGame

	Base class for networked games, gets synced over the net.
	Contains game names, network state, lobby etc.
	If you derive your own class make sure to implement the serialization
	functions and add the variables you want to get synced.

(C) 2015 Individual contributors, see AUTHORS file
*/

#include "util/dictionary.h"
#include "timing/time.h"
#include "core/refcounted.h"
#include "core/singleton.h"
#include "ReplicaManager3.h"
#include "NetworkIDManager.h"
#include "util/string.h"
#include "attr/attributecontainer.h"
#include "util/array.h"
#include "bitwriter.h"
#include "bitreader.h"
#include "networkplayer.h"
#include "attr/attributetable.h"
#include "messaging/message.h"

namespace RakNet
{
	struct Packet;
	struct RakNetGUID;
	struct SystemAddress;
	class RakString;
}

//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class NetworkGame : public RakNet::Replica3, public Core::RefCounted
{
	__DeclareClass(NetworkGame);
	__DeclareInterfaceSingleton(NetworkGame);
public:

	/// constructor
	NetworkGame();
	/// destructor
	~NetworkGame();

	/// 
	void Open();
	///
	void Close();

	///
	void OnFrame();

	/// set game name
	void SetGameName(const Util::String & name);
	/// get game name
	const Util::String & GetGameName() const;
	/// we are the initial creator of the session, unrelated to host status
	const bool IsCreator() const;
	/// set game ID (registered game type at masterserver)
	void SetGameID(const Util::String & name);
	/// get game ID
	const Util::String & GetGameID() const;
	/// Submit to master server
	void SetMasterServerUpdate(bool enable);
	///
	bool GetMasterServerUpdate();
	/// trigger refresh of available rooms on master
	virtual void UpdateRoomList();
	/// join a server returned by the master server
    void JoinRoom(const Util::String & guid, bool isIp=false);
	/// create lobby and publish to master server (if enabled)
	void CreateRoom();
	/// unpublishes and resets network
	void CancelRoom();
	/// get master server list
	const Ptr<Attr::AttributeTable>& GetMasterList() const;
	
	/// start game (if host)
	void StartGame();
		

	/// receives the master servers room list
	virtual void OnReceiverMasterList(){}
	/// called when successfully joined a room
	virtual void OnJoinedRoom(){}
	/// called when failed to join a room
	virtual void OnJoinFailed(const Util::String & reason){ n_printf("\n%s",reason.AsCharPtr());};
	/// called for already connected clients when another client has disconnected
	virtual void OnPlayerDisconnect(const RakNet::RakNetGUID& guid){}

	/// we received a message
	virtual void OnHandleMessage(const Ptr<Messaging::Message> &msg){}
	/// called when player joins room
	virtual void OnPlayerJoined(const Ptr<MultiplayerFeature::NetworkPlayer>& player){}

	/// game started
	virtual void OnGameStarted(){}

	/// overload to add own variables to the serialization stream when creating game object
	virtual void SerializeConstruction(const Ptr<Multiplayer::BitWriter> & writer){}
	/// overload to add own variables to the deserialization stream when creating game object
	virtual void DeserializeConstruction(const Ptr<Multiplayer::BitReader> & reader){}

	/// overload to add own variables to the serialization stream
	virtual void Serialize(const Ptr<Multiplayer::BitWriter> & writer){}
	/// overload to add own variables to the deserialization stream
	virtual void Deserialize(const Ptr<Multiplayer::BitReader> & reader){}

	/// send message to peers
	void Send(const Ptr<Messaging::Message> & message);

	/// set max players
	void SetMaxPlayers(ubyte val);
	/// get max players
	const ubyte GetMaxPlayers() const;

	/// get the amount of clients connected
	int GetCurrentAmountOfPlayers();

	/// get a player
	Ptr<MultiplayerFeature::NetworkPlayer> & GetPlayer(const Multiplayer::UniquePlayerId & id);
	/// get a player by index
	Ptr<MultiplayerFeature::NetworkPlayer> & GetPlayerByIndex(IndexT idx);

	/// whenever joining a room this is called if the game is started
	virtual bool CanJoinInGame();

	/// set the master server list
	void ReceiveMasterList(Ptr<Attr::AttributeTable> & masterList);

protected:
	/// 
	virtual void OnAllReady(bool dummy);
private:

	///
	void OnReadyChanged(const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>& change);
	

	/// raknet sync stuff below many of these dont need to be overloaded in this case but are pure virtual
	///
	virtual void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const;
	///
	virtual RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3);
    ///
    virtual void OnUserReplicaPreSerializeTick(void);
	///
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection);
	///
	virtual void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection);
	///
	virtual void SerializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection);
	///
	virtual bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection);
	///
	virtual void DeserializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection);
	///
	virtual void PostDeserializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection);
	///
	virtual void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection);
	///
	virtual bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection);
	///
	virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const;
	///
	virtual void DeallocReplica(RakNet::Connection_RM3 *sourceConnection);
	///
	virtual RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection);
	///
	virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters);
	///
	virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters);




	friend class NetworkServer;
	friend class NetworkPlayer;

	/// add player instance to current game
	void AddPlayer(const Ptr<MultiplayerFeature::NetworkPlayer> & player);
	/// remove player
	void RemovePlayer(const Ptr<MultiplayerFeature::NetworkPlayer> & player);

	/// deal with a packet
	bool HandlePacket(RakNet::Packet * packet);	

	/// handle message
	virtual void HandleMessage(const Ptr<Messaging::Message> &msg);

	/// get in game
	void StartInGame();

	/// these are synced per default	
	Util::String gameName;
	bool canJoin;
	bool inLobby;	

	bool updateMaster;	
	int currentPlayers;
	int maxPlayers;


	/// these are local
	bool creator;	
	RakNet::Time nextMasterServerUpdate;	
	Util::Dictionary<uint64_t, Ptr<MultiplayerFeature::NetworkPlayer>> players;
	Ptr<Attr::AttributeTable> serverList;
	Util::String gameID;
	Util::Array<Ptr<Messaging::Message>> queuedMessages;
    Util::Array<Ptr<Messaging::Message>> currentMessages;
};

//------------------------------------------------------------------------------
/**
*/
inline 
const bool
NetworkGame::IsCreator() const
{
	return this->creator;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String &
NetworkGame::GetGameName() const
{
	return this->gameName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String &
NetworkGame::GetGameID() const
{
	return this->gameID;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
NetworkGame::SetMasterServerUpdate(bool enable)
{
	this->updateMaster = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
NetworkGame::GetMasterServerUpdate()
{
	return this->updateMaster;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
NetworkGame::SetGameName(const Util::String & name) 
{
	this->gameName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
NetworkGame::SetGameID(const Util::String & name)
{
	this->gameID = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Ptr<Attr::AttributeTable>& 
NetworkGame::GetMasterList() const
{
	return this->serverList;
}

//------------------------------------------------------------------------------
/**
*/
inline const ubyte
NetworkGame::GetMaxPlayers() const
{
	return this->maxPlayers;
}

//------------------------------------------------------------------------------
/**
*/
inline void
NetworkGame::SetMaxPlayers(ubyte val)
{
	n_printf("\nset max players to %d",val);
	this->maxPlayers = val;
	n_printf("\nmax players is set to %d", maxPlayers);
}

}// namespace MultiplayerFeature
//------------------------------------------------------------------------------
