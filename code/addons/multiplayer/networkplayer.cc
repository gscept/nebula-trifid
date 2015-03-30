//------------------------------------------------------------------------------
//  networkplayer.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "networkplayer.h"
#include "bitwriter.h"
#include "bitreader.h"
#include "networkgame.h"
#include "networkserver.h"

namespace MultiplayerFeature
{
	__ImplementClass(MultiplayerFeature::NetworkPlayer, 'MPNP', Core::RefCounted);

using namespace Util;
using namespace Multiplayer;
using namespace RakNet;

//------------------------------------------------------------------------------
/**
*/
NetworkPlayer::NetworkPlayer() :
    signInState(Unkown),    
    isLocal(true),
    isHost(false),
	natType(NAT_TYPE_UNKNOWN)
{
	//
}

//------------------------------------------------------------------------------
/**
*/
NetworkPlayer::~NetworkPlayer()
{
	//	
}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkPlayer::SetReady(bool val)
{ 
	Multiplayer::SyncPoint::SetReady("_READY", val);	
}

//------------------------------------------------------------------------------
/**
*/
bool 
NetworkPlayer::IsReady() const
{  
	return Multiplayer::SyncPoint::GetReady("_READY");
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{	
	allocationIdBitstream->Write(this->GetRtti()->GetFourCC().AsUInt());
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3ConstructionState
NetworkPlayer::QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3)
{
	// Whoever created the user replicates it.
	return QueryConstruction_PeerToPeer(destinationConnection);
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkPlayer::QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	Ptr<BitWriter> writer = BitWriter::Create();
	writer->SetStream(constructionBitstream);
	writer->WriteString(this->playerName);
	constructionBitstream->WriteCasted<unsigned char>(this->natType);
	writer->WritePlayerID(this->id);
	constructionBitstream->Write(this->playerAddress);	
	this->SerializeConstruction(writer);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::SerializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkPlayer::DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	Ptr<BitReader> reader = BitReader::Create();
	reader->SetStream(constructionBitstream);
	this->playerName = reader->ReadString();
	constructionBitstream->ReadCasted<unsigned char>(this->natType);
	this->id = reader->ReadPlayerID();
	constructionBitstream->Read(this->playerAddress);
	this->DeserializeConstruction(reader);	
	this->isLocal = false;
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::DeserializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkPlayer::DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3ActionOnPopConnection
NetworkPlayer::QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const
{
	return QueryActionOnPopConnection_PeerToPeer(droppedConnection);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::DeallocReplica(RakNet::Connection_RM3 *sourceConnection)
{
	NetworkGame::Instance()->RemovePlayer(Ptr<NetworkPlayer>(this));	
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3QuerySerializationResult
NetworkPlayer::QuerySerialization(RakNet::Connection_RM3 *destinationConnection)
{
	return QuerySerialization_PeerToPeer(destinationConnection);
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3SerializationResult
NetworkPlayer::Serialize(RakNet::SerializeParameters *serializeParameters)
{		
	Ptr<BitWriter> writer = BitWriter::Create();
	writer->SetStream(&serializeParameters->outputBitstream[0]);
	this->Serialize(writer);
	return RM3SR_BROADCAST_IDENTICALLY;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	Ptr<BitReader> reader = BitReader::Create();
	reader->SetStream(&deserializeParameters->serializationBitstream[0]);
	this->Deserialize(reader);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkPlayer::PostDeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	NetworkGame::Instance()->AddPlayer(Ptr<NetworkPlayer>(this));
}

} // namespace MultiplayerFeature
