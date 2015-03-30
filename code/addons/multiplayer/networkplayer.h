#pragma once
//------------------------------------------------------------------------------
/**
    @class MultiplayerFeature::NetworkPlayer
            
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "uniqueplayerid.h"
#include "util/stringatom.h"
#include "NatTypeDetectionCommon.h"
#include "ReplicaManager3.h"
#include "bitwriter.h"
#include "bitreader.h"

//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class NetworkPlayer : public RakNet::Replica3, public Core::RefCounted
{
	__DeclareClass(NetworkPlayer);
public:
    enum SignInState
    {
        NotSignedIn = 0,
        SignedInLocally,
        SignedInOnline,

        Unkown
    };
    /// constructor
	NetworkPlayer();
    /// destructor
	virtual ~NetworkPlayer();
    /// get SignInState	
    SignInState GetSignInState() const;
    /// set SignInState
    void SetSignInState(SignInState val);
    /// get PlayerName	
    const Util::String& GetPlayerName() const;
    /// set PlayerName
    void SetPlayerName(const Util::String& val);

    /// set player ready, overwrite in subclass
    void SetReady(bool val);
    /// is player ready, overwrite in subclass
    bool IsReady() const;

    /// get Id	
    const Multiplayer::UniquePlayerId& GetUniqueId() const;
    /// set Id
    void SetUniqueId(const Multiplayer::UniquePlayerId& val);

    /// get IsLocal	
    bool IsLocal() const;
    /// set IsLocal
    void SetLocal(bool val);
    /// get IsHost	
    bool IsHost() const;
    /// set IsHost
    void SetHost(bool val);
    
	/// overload to add own variables to the serialization stream when creating game object
	virtual void SerializeConstruction(const Ptr<Multiplayer::BitWriter> & writer){}
	/// overload to add own variables to the deserialization stream when creating game object
	virtual void DeserializeConstruction(const Ptr<Multiplayer::BitReader> & reader){}

	/// overload to add own variables to the serialization stream
	virtual void Serialize(const Ptr<Multiplayer::BitWriter> & writer){}
	/// overload to add own variables to the deserialization stream
	virtual void Deserialize(const Ptr<Multiplayer::BitReader> & reader){}

	/// ready flag changed
	virtual void OnReadyChanged(bool ready){}

	/// send message to peers
//	void Send(const Ptr<Messaging::Message> & message);

protected:

	friend class NetworkServer;

	/// raknet sync stuff below many of these dont need to be overloaded in this case but are pure virtual
	///
	virtual void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const;
	///
	virtual RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3);
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
	///
	virtual void PostDeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection);

    
   
    SignInState signInState;    
    bool isLocal;
    bool isHost;
    Util::String playerName;
	Multiplayer::UniquePlayerId id;	

	RakNet::NATTypeDetectionResult natType;	
	RakNet::SystemAddress playerAddress;
};

//------------------------------------------------------------------------------
/**
*/
inline NetworkPlayer::SignInState 
NetworkPlayer::GetSignInState() const
{
    return this->signInState;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
NetworkPlayer::SetSignInState(NetworkPlayer::SignInState val)
{
    this->signInState = val;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
NetworkPlayer::GetPlayerName() const
{
    return this->playerName;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
NetworkPlayer::SetPlayerName(const Util::String& val)
{
    this->playerName = val;
}

//------------------------------------------------------------------------------
/**
*/
inline const Multiplayer::UniquePlayerId& 
NetworkPlayer::GetUniqueId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
NetworkPlayer::SetUniqueId(const Multiplayer::UniquePlayerId& val)
{
    this->id = val;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
NetworkPlayer::IsLocal() const
{
    return this->isLocal;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
NetworkPlayer::SetLocal(bool val)
{
    this->isLocal = val;
}

} // namespace MultiplayerFeature
//------------------------------------------------------------------------------
