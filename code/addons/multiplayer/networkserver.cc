//------------------------------------------------------------------------------
//  networkserver.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "networkserver.h"
#include "threading/thread.h"
#include <time.h>
#include "NetworkIDManager.h"
#include "RakPeerInterface.h"
#include "TCPInterface.h"
#include "RPC4Plugin.h"
#include "ReadyEvent.h"
#include "networkgame.h"
#include "MessageIdentifiers.h"
#include "networkplayer.h"
#include "multiplayerfeatureunit.h"
#include "GetTime.h"
#include "app/application.h"
#include "http/httpclient.h"
#include "io/memorystream.h"
#include "attr/attributetable.h"
#include "multiplayerattrs.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "syncpoint.h"
#include "networkentity.h"
#include "bitreader.h"
#include "src/jansson.h"

namespace MultiplayerFeature
{
__ImplementAbstractClass(MultiplayerFeature::NetworkServer, 'MNNS', Core::RefCounted);

using namespace Util;
using namespace Timing;
using namespace Debug;
using namespace Math;
using namespace RakNet;


//------------------------------------------------------------------------------
/**
*/
static void
DispatchNetworkMessage(RakNet::BitStream *bitStream, RakNet::Packet *packet)
{	
	n_assert(MultiplayerFeatureUnit::HasInstance());
	Ptr<NetworkServer> server = MultiplayerFeatureUnit::Instance()->GetServer();
	n_assert(server.isvalid());
	server->DispatchMessageStream(bitStream, packet);
}

RPC4GlobalRegistration __NebulaMessageDispatch("NebulaMessage", DispatchNetworkMessage, 0);

//------------------------------------------------------------------------------
/**
*/
NetworkServer::NetworkServer() :
    state(IDLE),
	rakPeer(NULL),
	networkIDManager(NULL),		
	rpc(NULL),	
	readyEvent(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NetworkServer::~NetworkServer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::Open()
{
	// init raknet
	this->rakPeer = RakPeerInterface::GetInstance();	
	this->networkIDManager = NetworkIDManager::GetInstance();	
	this->rpc = RPC4::GetInstance();
	this->readyEvent = Multiplayer::SyncPoint::GetInstance();
	this->replicationManager = ReplicationManager::Create();

	Multiplayer::SyncPoint::SetupSyncPoint(this->readyEvent);
	// attach raknet plugins 		
	this->rakPeer->AttachPlugin(this->rpc);
	this->rakPeer->AttachPlugin(this->readyEvent);
	this->rakPeer->AttachPlugin(this->replicationManager);			
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::Close()
{
	this->rakPeer->Shutdown(100, 0);
	RakNet::RakPeerInterface::DestroyInstance(this->rakPeer);
	delete this->networkIDManager;
	delete this->rpc;
	delete this->readyEvent;
	this->replicationManager = 0;	
}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkServer::OnFrame()
{        
	if (this->state != IDLE)
	{	
		if (this->deferredMessages.Size() > 0)
		{
			for (int i = this->deferredMessages.Size() -1  ; i >=0 ; i--)
			{
				RakNet::NetworkID id = this->deferredMessages.KeyAtIndex(i);
				RakNet::Replica3* replica = this->LookupReplica(id);				
				MultiplayerFeature::NetworkEntity * entity = dynamic_cast<MultiplayerFeature::NetworkEntity*>(replica);
				if (entity && entity->IsActive())
				{
					Util::Array<Ptr<Messaging::Message>> & msgs = this->deferredMessages.ValueAtIndex(i);
					for (int j = 0; j < msgs.Size(); j++)
					{
						entity->SendSync(msgs[j]);
					}
					this->deferredMessages.EraseAtIndex(i);
				}
			}
		}
		Packet *packet;
		for (packet = this->rakPeer->Receive(); packet; this->rakPeer->DeallocatePacket(packet), packet = this->rakPeer->Receive())
		{
			this->HandlePacket(packet);
		}		
	}	
	NetworkGame::Instance()->OnFrame();	
}


//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::SearchForGames()
{	
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::CreateRoom()
{
	this->state = IN_LOBBY_WAITING_FOR_HOST_DETERMINATION;
}


//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::CancelRoom()
{
	this->state = NETWORK_STARTED;
}


//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::StartGame()
{	
	this->state = IN_GAME;	
}

//------------------------------------------------------------------------------
/**
*/
RakNet::Replica3 *
NetworkServer::LookupReplica(RakNet::NetworkID replicaId)
{
	RakNet::Replica3 * replica = this->networkIDManager->GET_OBJECT_FROM_ID<RakNet::Replica3*>(replicaId);	
	return replica;
}


//------------------------------------------------------------------------------
/**
Returns a user defined flag when IN_GAME so that the user can decide if it's allowed to join or not.
*/
bool NetworkServer::IsInGameJoinUnLocked()
{
	if (this->state == IN_GAME)
	{
		return NetworkGame::Instance()->CanJoinInGame();
	}
	else
	{
		return true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::DispatchMessageStream(RakNet::BitStream * msgStream, Packet *packet)
{
	RakNet::NetworkID entityId;
	msgStream->Read(entityId);
	RakNet::Replica3 * replica = this->LookupReplica(entityId);

	MultiplayerFeature::NetworkEntity * entity = dynamic_cast<MultiplayerFeature::NetworkEntity*>(replica);

	Ptr<Multiplayer::BitReader> reader = Multiplayer::BitReader::Create();
	reader->SetStream(msgStream);

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
		if (entity)
		{
			entity->SendSync(msg);
		}
		else
		{
			this->AddDeferredMessage(entityId, msg);
		}
		
	}
	breader->Close();

}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::SendMessageStream(RakNet::BitStream* msgStream)
{
	for (IndexT i = 0; i < this->participants.Size(); i++)
	{
			this->rpc->Signal("NebulaMessage", msgStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, this->participants[i], false, false);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::AddDeferredMessage(RakNet::NetworkID entityId, const Ptr<Messaging::Message> &msg)
{
	if (!this->deferredMessages.Contains(entityId))
	{
		this->deferredMessages.Add(entityId, Util::Array<Ptr<Messaging::Message>>());
	}
	this->deferredMessages[entityId].Append(msg);
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkServer::SetupLowlevelNetworking()
{
	n_assert2(NetworkGame::HasInstance(), "No NetworkGame or subclass instance exists, cant continue\n");

	this->replicationManager->SetNetworkIDManager(this->networkIDManager);
	this->replicationManager->SetAutoManageConnections(false, true);

	Ptr<NetworkGame> game = NetworkGame::Instance();
	game->SetNetworkIDManager(this->networkIDManager);
	game->SetNetworkID(0);
	this->replicationManager->Reference(game);

	Ptr<NetworkPlayer> user = MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetPlayer();
	user->SetNetworkIDManager(this->networkIDManager);
	user->SetUniqueId(this->rakPeer->GetMyGUID());
	n_printf("my id: %s\n", this->rakPeer->GetMyGUID().ToString());
	return true;
}


//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::ShutdownLowlevelNetworking()
{
	this->replicationManager->Clear();
	this->rakPeer->Shutdown(100, 0);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::ParseServerResponse(const Util::String & response)
{
	n_printf("got master server results:\n%s", response.AsCharPtr());
	json_error_t error;
	json_t * root = json_loads(response.AsCharPtr(), JSON_REJECT_DUPLICATES, &error);
	if (NULL == root)
	{
		n_warning("error parsing json from master server: %s\n", error.text);
		// FIXME which state now
		return;
	}

	Ptr<Attr::AttributeTable> table;
	if (!this->masterResult.isvalid())
	{
		table = Attr::AttributeTable::Create();
		table->BeginAddColumns(false);
		table->AddColumn(Attr::GameRow, false);
		table->AddColumn(Attr::RoomName, false);
		table->AddColumn(Attr::GameID, false);
		table->AddColumn(Attr::GameAge, false);
		table->AddColumn(Attr::GameServerAddress, false);
		table->AddColumn(Attr::MaxPlayers, false);
		table->AddColumn(Attr::CurrentPlayers, false);
		table->AddColumn(Attr::Id, false);
		table->EndAddColumns();
		this->masterResult = table;
	}
	else
	{
		table = this->masterResult;
	}

	void *iter = json_object_iter(root);
	while (iter)
	{
		String firstKey = json_object_iter_key(iter);


		if (firstKey == "GET")
		{
			json_t* jsonArray = json_object_iter_value(iter);
			size_t arraySize = json_array_size(jsonArray);
			for (unsigned int i = 0; i < arraySize; i++)
			{
				json_t* object = json_array_get(jsonArray, i);

				json_t * val = json_object_get(object, "guid");
				Util::String eguid = json_string_value(val);
				if (InvalidIndex != table->FindRowIndexByAttr(Attr::Attribute(Attr::Id, eguid)))
				{
					continue;
				}

				IndexT row = table->AddRow();				
				val = json_object_get(object, "roomName");
				n_assert(val->type == JSON_STRING);
				Util::String room = json_string_value(val);
				room.Append("\r");
				Util::String decRoom = Util::String::FromBase64(room);
				table->SetString(Attr::RoomName, row, decRoom);

				val = json_object_get(object, "__gameId");
				n_assert(val->type == JSON_STRING);
				Util::String gameId = json_string_value(val);
				table->SetString(Attr::GameID, row, gameId);

				val = json_object_get(object, "__timeoutSec");
				n_assert(val->type == JSON_STRING);
				Util::String strval = json_string_value(val);
				int timeout = strval.AsInt();
				table->SetInt(Attr::GameAge, row, timeout);

				val = json_object_get(object, "__addr");
				n_assert(val->type == JSON_STRING);
				Util::String ipAddr = json_string_value(val);
				table->SetString(Attr::GameServerAddress, row, ipAddr);

				val = json_object_get(object, "guid");
				n_assert(val->type == JSON_STRING);
				Util::String guid = json_string_value(val);
				table->SetString(Attr::Id, row, guid);

				val = json_object_get(object, "__rowId");
				n_assert(val->type == JSON_INTEGER);
				int gameRow = (int)json_integer_value(val);
				table->SetInt(Attr::GameRow, row, gameRow);

				val = json_object_get(object, "currentPlayers");
				n_assert(val->type == JSON_INTEGER);
				int current = (int)json_integer_value(val);
				table->SetInt(Attr::CurrentPlayers, row, current);

				val = json_object_get(object, "maxPlayers");
				n_assert(val->type == JSON_INTEGER);
				int maxplayers = (int)json_integer_value(val);
				table->SetInt(Attr::MaxPlayers, row, maxplayers);
				n_printf("Room name: %s, Server: %s, players: %d/%d\n", room.AsCharPtr(), ipAddr.AsCharPtr(), current, maxplayers);
			}
			iter = json_object_iter_next(root, iter);
		}
		else
		{
			iter = json_object_iter_next(root, iter);
		}
	}
	json_decref(root);	
}

//------------------------------------------------------------------------------
/**
*/
MultiplayerFeature::NetworkServer*
NetworkServer::Instance()
{
	return MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetServer();
}
} // namespace MultiplayerFeature
