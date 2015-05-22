//------------------------------------------------------------------------------
//  networkgame.cc
//  (C) 2015 Individual contributors, see AUTHORS file
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
#include "jansson/src/jansson.h"
#include "multiplayerfeatureunit.h"


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
		updateMaster(true),
		masterServerRow(-1),
		nextMasterServerUpdate(0.0),
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
	constructionBitstream->Write(this->updateMaster);
	constructionBitstream->Write(this->masterServerRow);
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
	constructionBitstream->Read(this->updateMaster);
	constructionBitstream->Read(this->masterServerRow);
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
		serializeParameters->outputBitstream[0].Write(this->updateMaster);
		serializeParameters->outputBitstream[0].Write(this->masterServerRow);
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
			deserializeParameters->serializationBitstream[0].Read(this->updateMaster);
			deserializeParameters->serializationBitstream[0].Read(masterServerRow);
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
	NetworkServer::Instance()->UpdateRoomList();
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
NetworkGame::PublishToMaster()
{
	Util::String req;
	req.Format("http://" MASTER_SERVER_ADDRESS "/testServer");
	IO::URI serverUri = req;
	Ptr<Http::HttpClient> client = Http::HttpClientRegistry::Instance()->ObtainConnection(serverUri);
	
	IO::URI requri = req;	
	Util::String rowStr = "";
	if (this->masterServerRow != -1)
	{
		rowStr.Format(",'__rowId':%d", this->masterServerRow);
	}
	Ptr<IO::MemoryStream> stream = IO::MemoryStream::Create();
	stream->SetAccessMode(IO::Stream::ReadWriteAccess);
	stream->Open();
	// encode gamename in base64 to avoid any encoding issues
	Util::String gamename = this->GetGameName().AsBase64();
	// we dont want linefeeds
	gamename.Strip("\r");	
	req.Format("{'__gameId':'%s','__clientReqId': '0','__timeoutSec': '30','roomName':'%s','guid':'%s','currentPlayers':%d,'maxPlayers':%d %s}", 
		this->gameID.AsCharPtr(), gamename.AsCharPtr(), NetworkServer::Instance()->GetRakPeerInterface()->GetMyGUID().ToString(),
		this->GetCurrentAmountOfPlayers(), this->maxPlayers, rowStr.AsCharPtr());
	n_printf("%s\n", req.AsCharPtr());
	Http::HttpStatus::Code res = client->SendRequest(Http::HttpMethod::Post, requri, req, stream.cast<IO::Stream>());
	if(res == Http::HttpStatus::OK)
	{	
		Util::String buf;
		buf.Set((const char*)stream->GetRawPointer(), stream->GetSize());
		json_error_t error;
		json_t * root = json_loads(buf.AsCharPtr(), JSON_REJECT_DUPLICATES, &error);
		if (NULL == root)
		{
			n_warning("error parsing json from master server\n");	
			return;
		}

		void *iter = json_object_iter(root);
		if (iter)
		{
			Util::String firstKey = json_object_iter_key(iter);


			if (firstKey == "POST")
			{
				json_t* object = json_object_iter_value(iter);			
				json_t* val = json_object_get(object, "__rowId");
				n_assert(val->type == JSON_INTEGER);
				this->masterServerRow = (int)json_integer_value(val);
			}
		}
	}
	Http::HttpClientRegistry::Instance()->ReleaseConnection(serverUri);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::UnpublishFromMaster()
{
	Util::String req;
	req.Format("http://" MASTER_SERVER_ADDRESS "/testServer");
	IO::URI serverUri = req;
	Ptr<Http::HttpClient> client = Http::HttpClientRegistry::Instance()->ObtainConnection(serverUri);	
	IO::URI requri = req;
	req.Format("testServer?__gameId=%s&__rowId=%d", this->gameID.AsCharPtr(),this->masterServerRow);
	requri.SetLocalPath(req);
	Ptr<IO::MemoryStream> stream = IO::MemoryStream::Create();
	stream->SetAccessMode(IO::Stream::ReadWriteAccess);
	stream->Open();	
	Http::HttpStatus::Code res = client->SendRequest(Http::HttpMethod::Delete, requri, stream.cast<IO::Stream>());
	Http::HttpClientRegistry::Instance()->ReleaseConnection(serverUri);
	this->masterServerRow = -1;
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
	PublishToMaster();	
	__SetSyncEventCallback(MultiplayerFeature::NetworkGame, OnReadyChanged, this, "_READY");
	__SetSyncEventAllCallback(MultiplayerFeature::NetworkGame, OnAllReady, this, "_READY");
	SyncPoint::AddToTracking("_READY", MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetPlayer()->GetUniqueId());

}

//------------------------------------------------------------------------------
/**
*/
void
NetworkGame::JoinRoom(const Util::String & guid)
{
	this->creator = false;
	RakNet::RakNetGUID rguid;
	rguid.FromString(guid.AsCharPtr());
	NetworkServer::Instance()->NatConnect(rguid);
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

}