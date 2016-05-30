//------------------------------------------------------------------------------
//  networkgame.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "networkgame.h"
#include "RakNetTypes.h"
#include "syncpoint.h"
#include "networkserver.h"
#include "FullyConnectedMesh2.h"
#include "io/uri.h"
#include "http/httpclient.h"
#include "http/httpclientregistry.h"
#include "io/memorystream.h"
#include "RakPeerInterface.h"
#include "io/binarywriter.h"
#include "multiplayerattrs.h"
#include "appgame/gameapplication.h"
#include "statehandlers/gamestatehandler.h"
#include "multiplayerfeatureunit.h"
#include "GetTime.h"


using namespace RakNet;
using namespace Multiplayer;

namespace MultiplayerFeature
{
	__ImplementClass(MultiplayerFeature::NetworkGame, 'MNGA', Core::RefCounted);
	__ImplementInterfaceSingleton(MultiplayerFeature::NetworkGame);

//------------------------------------------------------------------------------
/**
*/
NetworkGame::NetworkGame() :
		gameName("DefaultGame"),
		canJoin(true),
		inLobby(false),		
		nextMasterServerUpdate(),
		maxPlayers(8),
		currentPlayers(1),
		creator(false)
{
	__ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/

NetworkGame::~NetworkGame()
{
	__DestructInterfaceSingleton;
}


//------------------------------------------------------------------------------
/**
*/
void 
NetworkGame::OnUserReplicaPreSerializeTick(void)
{
    this->currentMessages = this->queuedMessages;
    this->queuedMessages.Clear();
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3ConstructionState
NetworkGame::QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3)
{
	if (!NetworkServer::Instance()->HasHost())
	{
		return RM3CS_NO_ACTION;
	}
	if (this->IsCreator())
	{
		return QueryConstruction_PeerToPeer(destinationConnection, R3P2PM_STATIC_OBJECT_CURRENTLY_AUTHORITATIVE);
	}		
	else
	{
		return QueryConstruction_PeerToPeer(destinationConnection, R3P2PM_STATIC_OBJECT_NOT_CURRENTLY_AUTHORITATIVE);
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::SerializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	constructionBitstream->Write(this->gameName.AsCharPtr());
	constructionBitstream->Write(this->canJoin);
	constructionBitstream->Write(this->inLobby);	
	constructionBitstream->Write(this->maxPlayers);
	Ptr<BitWriter> writer = BitWriter::Create();
	writer->SetStream(constructionBitstream);
	this->SerializeConstruction(writer);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::DeserializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	RakString name;
	constructionBitstream->Read(name);
	this->gameName = name.C_String();
	constructionBitstream->Read(this->canJoin);
	constructionBitstream->Read(this->inLobby);	
	constructionBitstream->Read(this->maxPlayers);
	Ptr<BitReader> reader = BitReader::Create();
	reader->SetStream(constructionBitstream);
	this->DeserializeConstruction(reader);	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::DeallocReplica(RakNet::Connection_RM3 *sourceConnection)
{
	// just dereference, we dont want the system to remove this singleton ever
	ReplicationManager::Instance()->Dereference(this);
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3QuerySerializationResult
NetworkGame::QuerySerialization(RakNet::Connection_RM3 *destinationConnection)
{
	if (this->currentMessages.Size() > 0)
	{
		return RM3QSR_CALL_SERIALIZE;
	}
	if (this->IsCreator())
	{
		return QuerySerialization_PeerToPeer(destinationConnection, R3P2PM_STATIC_OBJECT_CURRENTLY_AUTHORITATIVE);
	}
	else
	{
		return QuerySerialization_PeerToPeer(destinationConnection, R3P2PM_STATIC_OBJECT_NOT_CURRENTLY_AUTHORITATIVE);
	}
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3SerializationResult
NetworkGame::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	
	if (this->IsCreator())
	{
		serializeParameters->outputBitstream[0].Write(true);
		serializeParameters->outputBitstream[0].Write(this->gameName.AsCharPtr());
		serializeParameters->outputBitstream[0].Write(this->canJoin);
		serializeParameters->outputBitstream[0].Write(this->inLobby);		
	}
	else
	{
		serializeParameters->outputBitstream[0].Write(false);
	}
	Ptr<BitWriter> writer = BitWriter::Create();
	writer->SetStream(&(serializeParameters->outputBitstream[0]));
	int count = this->currentMessages.Size();
	n_assert(count < 256);
	writer->WriteUChar(count);

	Ptr<IO::BinaryWriter> bwriter = IO::BinaryWriter::Create();
	bwriter->SetStream(writer.cast<IO::Stream>());
	bwriter->Open();
	
	for (int i = 0; i < count; i++)
	{
		const Ptr<Messaging::Message> & msg = this->currentMessages[i];
		writer->WriteUInt(msg->GetClassFourCC().AsUInt());
		msg->Encode(bwriter);
	}	
	this->Serialize(writer);

	if (count > 0)
	{ 
		return RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
	}
	else
	{
		return RM3SR_BROADCAST_IDENTICALLY;
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	if (deserializeParameters->bitstreamWrittenTo[0])
	{
		bool full;
		deserializeParameters->serializationBitstream[0].Read(full);
		if (full)
		{
			RakString name;
			deserializeParameters->serializationBitstream[0].Read(name);
			this->gameName = name.C_String();
			bool b;
			deserializeParameters->serializationBitstream[0].Read(b);
			if (b != this->canJoin)
			{
				this->canJoin = b;
				//FIXME: tell system that its unlocked now
			}
			deserializeParameters->serializationBitstream[0].Read(b);
			if (b != this->inLobby)
			{
				this->inLobby = b;
				//FIXME tell system about being back in lobby (or game)
			}
		}
		Ptr<BitReader> reader = BitReader::Create();
		reader->SetStream(&deserializeParameters->serializationBitstream[0]);
		
		// deserialize messages
		int count = reader->ReadUChar();
		Ptr<IO::BinaryReader> breader = IO::BinaryReader::Create();
		breader->SetStream(reader.cast<IO::Stream>());
		breader->Open();
		for (int i = 0; i < count; i++)
		{
			Util::FourCC fcc = reader->ReadUInt();
			Ptr<Core::RefCounted> cmsg = Core::Factory::Instance()->Create(fcc);
			Ptr<Messaging::Message> msg = cmsg.cast<Messaging::Message>();
			msg->SetDistribute(false);
			msg->Decode(breader);
			this->HandleMessage(msg);
		}
		this->Deserialize(reader);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkGame::QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkGame::DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkGame::DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3ActionOnPopConnection
NetworkGame::QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const
{
	// FIXME: connection was dropped, do something?
	return RakNet::RM3AOPC_DO_NOTHING;
}



//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::UpdateRoomList()
{
	NetworkServer::Instance()->SearchForGames();
}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkGame::AddPlayer(const Ptr<MultiplayerFeature::NetworkPlayer> & player)
{
	n_assert(!this->players.Contains(player->GetUniqueId().GetRaknetGuid().g));
	this->players.Add(player->GetUniqueId().GetRaknetGuid().g, player);
	SyncPoint::AddToTracking("_READY", player->GetUniqueId());
	SyncPoint::AddToTracking("_LOADED", player->GetUniqueId());
	this->OnPlayerJoined(player);
	n_printf("\nADDING PLAYER");
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::RemovePlayer(const Ptr<MultiplayerFeature::NetworkPlayer> & player)
{
	n_assert(this->players.Contains(player->GetUniqueId().GetRaknetGuid().g));
	SyncPoint::RemoveFromTracking("_READY", player->GetUniqueId());
	SyncPoint::RemoveFromTracking("_LOADED", player->GetUniqueId());
	this->players.Erase(player->GetUniqueId().GetRaknetGuid().g);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::ReceiveMasterList(Ptr<Attr::AttributeTable> & masterList)
{
	this->serverList = masterList;
	this->OnReceiverMasterList();
}



//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::StartInGame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::CreateRoom()
{
	this->creator = true;
	NetworkServer::Instance()->CreateRoom();	
	__SetSyncEventCallback(MultiplayerFeature::NetworkGame, OnReadyChanged, this, "_READY");
	__SetSyncEventAllCallback(MultiplayerFeature::NetworkGame, OnAllReady, this, "_READY");
	__SetSyncEventAllCallback(MultiplayerFeature::NetworkGame, OnLoaded, this, "_LOADED");
	SyncPoint::AddToTracking("_READY", MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetPlayer()->GetUniqueId());
	SyncPoint::AddToTracking("_LOADED", MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetPlayer()->GetUniqueId());
	SyncPoint::SetReady("_READY", false);
	SyncPoint::SetReady("_LOADED", false);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::JoinRoom(const Util::String & guid, bool isIp)
{
	this->creator = false;
    if (isIp)
    {
        RakNet::SystemAddress adr(guid.AsCharPtr());
        NetworkServer::Instance()->ConnectDirect(adr);
    }
    else
    {
        RakNet::RakNetGUID rguid;
        rguid.FromString(guid.AsCharPtr());
        NetworkServer::Instance()->Connect(rguid);
    }	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::CancelRoom()
{
	this->queuedMessages.Clear();
	this->currentMessages.Clear();
	for (int i = 0 ; i<this->players.Size();i++)
	{
		this->RemovePlayer(this->players.ValueAtIndex(0));
	}
	NetworkServer::Instance()->CancelRoom();
}


//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::StartGame()
{
	NetworkServer::Instance()->StartGame();	
	this->OnGameStarted();
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::Close()
{

}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::HandleMessage(const Ptr<Messaging::Message> &msg)
{
	if (msg->CheckId(MultiplayerFeature::StartLevel::Id))
	{
		Ptr<MultiplayerFeature::StartLevel> smsg = msg.cast<MultiplayerFeature::StartLevel>();
		const Ptr<BaseGameFeature::GameStateHandler>& state = App::GameApplication::Instance()->FindStateHandlerByName(smsg->GetState()).cast<BaseGameFeature::GameStateHandler>();
		state->SetLevelName(smsg->GetLevel());
		state->SetSetupMode(BaseGameFeature::GameStateHandler::LoadNetworkedLevel);
		App::GameApplication::Instance()->RequestState(smsg->GetState());		
	}
	else
	{
		this->OnHandleMessage(msg);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::Send(const Ptr<Messaging::Message> & message)
{
	this->queuedMessages.Append(message);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::PostDeserializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{		
	__SetSyncEventCallback(MultiplayerFeature::NetworkGame, OnReadyChanged, this, "_READY");
	__SetSyncEventAllCallback(MultiplayerFeature::NetworkGame, OnAllReady, this, "_READY");
	__SetSyncEventAllCallback(MultiplayerFeature::NetworkGame, OnLoaded, this, "_LOADED");
	SyncPoint::SetReady("_READY", false);
	SyncPoint::SetReady("_LOADED", false);
	this->OnJoinedRoom();
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::OnReadyChanged(const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>& change)
{	
	if (change.Key() != MultiplayerFeatureUnit::Instance()->GetUniqueId())
	{
		this->players[change.Key().GetRaknetGuid().g]->OnReadyChanged(change.Value());
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::OnAllReady(bool)
{
	n_printf("All ready\n");
}

//------------------------------------------------------------------------------
/**
*/
Ptr<MultiplayerFeature::NetworkPlayer> &
NetworkGame::GetPlayer(const Multiplayer::UniquePlayerId & id)
{
	n_assert(this->players.Contains(id.GetRaknetGuid().g));
	return this->players[id.GetRaknetGuid().g];
}

//------------------------------------------------------------------------------
/**
*/
int NetworkGame::GetCurrentAmountOfPlayers()
{
	//Added 1 for the own client because it's not added to the list which contains other clients
	return this->players.Size() + 1;
}

//------------------------------------------------------------------------------
/**
*/
bool NetworkGame::CanJoinInGame()
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<MultiplayerFeature::NetworkPlayer> &
NetworkGame::GetPlayerByIndex(IndexT idx)
{
	n_assert(idx < this->players.Size());
	return this->players.ValueAtIndex(idx);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::OnFrame()
{
// 	if (this->GetMasterServerUpdate())
// 	{
// 		if (NetworkServer::Instance()->IsHost() && (this->IsPublished() || this->delayedMaster))
// 		{
// 			RakNet::Time interval = this->delayedMaster ? 5000 : 20000;
// 			if ((RakNet::GetTimeMS() - this->nextMasterServerUpdate) > interval)
// 			{
// 				this->PublishToMaster();
// 				this->nextMasterServerUpdate = RakNet::GetTimeMS();
// 			}
// 		}
// 	}
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::OnLoaded(bool dummy)
{	
	SyncPoint::SetReady("_LOADED", false);
	App::GameApplication::Instance()->GetCurrentStateHandler().cast<BaseGameFeature::GameStateHandler>()->OnNetworkStarted();
}

}