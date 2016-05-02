//------------------------------------------------------------------------------
//  lannetworkserver.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "lannetworkserver.h"
#include "threading/thread.h"
#include <time.h>
#include "NetworkIDManager.h"
#include "RakPeerInterface.h"
#include "TCPInterface.h"
#include "RPC4Plugin.h"
#include "ReadyEvent.h"
#include "FullyConnectedMesh2.h"
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
#include "cJSON.h"

namespace MultiplayerFeature
{
__ImplementClass(MultiplayerFeature::LanNetworkServer, 'LNNS', MultiplayerFeature::NetworkServer);


__ImplementInterfaceSingleton(MultiplayerFeature::LanNetworkServer);

using namespace Util;
using namespace Timing;
using namespace Math;
using namespace RakNet;

#define CLIENT_PORT 61112
#define SERVER_PORT 61111


#if NEBULA3_DEBUG
#define CONNECTION_TIMEOUT 3000000 // ms 3000s
#else
#define CONNECTION_TIMEOUT 10000 // ms 10s
#endif

//------------------------------------------------------------------------------
/**
*/
LanNetworkServer::LanNetworkServer() :	
	host(false),
	serverStarted(false),
	fullyConnectedMesh(NULL)
{
	__ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
LanNetworkServer::~LanNetworkServer()
{
	__DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::Open()
{
	NetworkServer::Open();
	this->fullyConnectedMesh = FullyConnectedMesh2::GetInstance();
	this->rakPeer->AttachPlugin(this->fullyConnectedMesh);

}

//------------------------------------------------------------------------------
/**
*/
bool
LanNetworkServer::SetupLowlevelNetworking()
{
	NetworkServer::SetupLowlevelNetworking();
	this->fullyConnectedMesh->SetAutoparticipateConnections(false);
	this->fullyConnectedMesh->SetConnectOnNewRemoteConnection(false, "");
	this->replicationManager->SetNetworkIDManager(this->networkIDManager);
	this->replicationManager->SetAutoManageConnections(false, true);
		
	
	this->state = IDLE;
	
	
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::ShutdownLowlevelNetworking()
{
	this->fullyConnectedMesh->Clear();
	NetworkServer::ShutdownLowlevelNetworking();
	
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::Close()
{
	delete this->fullyConnectedMesh;
	NetworkServer::Close();	
}

//------------------------------------------------------------------------------
/**
*/
void 
LanNetworkServer::OnFrame()
{
	if (this->state == SERVER_LOBBY || (this->state == IN_LOBBY && this->IsHost()))
	{
		RakNet::Time now = RakNet::GetTime();
		if (now - this->lastBroadcast > 4000)
		{
			this->UpdateServerInfo();
			rakPeer->AdvertiseSystem("255.255.255.255", CLIENT_PORT, this->serverInfoString.AsCharPtr(), this->serverInfoString.Length(), 0);
			this->lastBroadcast = now;
		}
	}
	NetworkServer::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
bool
LanNetworkServer::HandlePacket(RakNet::Packet * packet)
{
	Util::String targetName = packet->systemAddress.ToString(true);
	switch (packet->data[0])
	{
	case ID_IP_RECENTLY_CONNECTED:
	{
		n_printf("This IP address recently connected from %s\n", targetName.AsCharPtr());
				
	}
	break;
	case ID_INCOMPATIBLE_PROTOCOL_VERSION:
	{
		n_printf("ID_INCOMPATIBLE_PROTOCOL_VERSION from %s\n", targetName.AsCharPtr());
	}
	break;
	case ID_DISCONNECTION_NOTIFICATION:
	{
		n_printf("ID_DISCONNECTION_NOTIFICATION from %s\n", targetName.AsCharPtr());
		NetworkGame::Instance()->OnPlayerDisconnect(packet->guid);
		IndexT idx = this->participants.FindIndex(packet->guid);
		if (idx != InvalidIndex)
		{
			//this->participants.EraseIndex(idx);
		}
	}
	break;
	case ID_CONNECTION_LOST:
	{
		n_printf("Connection to %s lost\n", targetName.AsCharPtr());				
		IndexT idx = this->participants.FindIndex(packet->guid);
		if (idx != InvalidIndex)
		{
			//this->participants.EraseIndex(idx);
		}		
	}
	break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
	{
		n_printf("No free incoming connections to %s\n", targetName.AsCharPtr());				
	}
	break;
	case ID_NEW_INCOMING_CONNECTION:
	{
		if (this->fullyConnectedMesh->IsHostSystem())
		{
			n_printf("Sending player list to new connection: %s\n", packet->guid.ToString());
			this->fullyConnectedMesh->StartVerifiedJoin(packet->guid);
		}
		n_printf("ID_NEW_INCOMING_CONNECTION\n");
		//this->participants.Append(packet->guid);
	}
	break;
	case ID_FCM2_VERIFIED_JOIN_START:
	{
		DataStructures::List<RakNet::SystemAddress> addresses;
		DataStructures::List<RakNet::RakNetGUID> guids;
		DataStructures::List<RakNet::BitStream*> streams;
		this->fullyConnectedMesh->GetVerifiedJoinRequiredProcessingList(packet->guid, addresses, guids, streams);
		for (unsigned int i = 0; i < guids.Size(); i++)
		{
			ConnectionAttemptResult car = this->rakPeer->Connect(addresses[i].ToString(false), addresses[i].GetPort(), 0, 0);			
		}
	}
	break;
	case ID_FCM2_VERIFIED_JOIN_CAPABLE:
	{
		//If server not full and you're in lobby or allowed to join while game has started
		if (this->fullyConnectedMesh->GetParticipantCount() + 1 < NetworkGame::Instance()->GetMaxPlayers() && IsInGameJoinUnLocked())
		{
			this->fullyConnectedMesh->RespondOnVerifiedJoinCapable(packet, true, 0);
		}
		else
		{
			RakNet::BitStream answer;
			answer.Write("Server Full\n");
			this->fullyConnectedMesh->RespondOnVerifiedJoinCapable(packet, false, &answer);
		}
	}
		break;
	case ID_FCM2_VERIFIED_JOIN_ACCEPTED:
	{
		DataStructures::List<RakNet::RakNetGUID> systemsAccepted;
		bool thisSystemAccepted;
		this->fullyConnectedMesh->GetVerifiedJoinAcceptedAdditionalData(packet, &thisSystemAccepted, systemsAccepted, 0);
		if (thisSystemAccepted)
		{
			n_printf("Game join request accepted\n");
		}
		else
		{
			n_printf("System %s joined the mesh\n", systemsAccepted[0].ToString());
		}

		for (unsigned int i = 0; i < systemsAccepted.Size(); i++)
		{
			this->replicationManager->PushConnection(this->replicationManager->AllocConnection(rakPeer->GetSystemAddressFromGuid(systemsAccepted[i]), systemsAccepted[i]));
		}
		this->participants.Clear();
		DataStructures::List<RakNetGUID> meshclients;
		this->fullyConnectedMesh->GetParticipantList(meshclients);
		for (unsigned int i = 0; i < meshclients.Size(); i++)
		{
			this->participants.Append(meshclients[i]);
		}
	}
		break;
	case ID_CONNECTION_REQUEST_ACCEPTED:
	{
		n_printf("Connection request to %s accepted\n", targetName.AsCharPtr());
		n_printf("connection guid: %s\n", packet->guid.ToString());	
		//this->participants.Append(packet->guid);
		
	}
	break;
	case ID_FCM2_NEW_HOST:
	{
		if (packet->guid == rakPeer->GetMyGUID())
		{
			n_printf("we are the host\n");
			this->state = IN_LOBBY;
			// Original host dropped. I am the new session host. Upload to the cloud so new players join this system.
			//RakNet::CloudKey cloudKey(NetworkGame::Instance()->GetGameID().AsCharPtr(), 0);
			//cloudClient->Post(&cloudKey, 0, 0, rakPeer->GetGuidFromSystemAddress(this->natPunchServerAddress));			
		}
		else
		{
			this->state = IN_LOBBY;
			n_printf("the new host is %s\n", packet->guid.ToString());
		}
	}
		break;
	case ID_CONNECTION_ATTEMPT_FAILED:
	{
		n_printf("Connection attempt to %s failed\n", targetName.AsCharPtr());
			
	}
	break;
	

	case ID_ADVERTISE_SYSTEM:		
		if (rakPeer->GetSystemAddressFromGuid(packet->guid) == RakNet::UNASSIGNED_SYSTEM_ADDRESS &&
			rakPeer->GetMyGUID() != packet->guid)
		{
			Util::String resp;
			if (packet->length > 0)
			{
				resp.Set((const char*)&packet->data[1], packet->length-1);
				this->ParseServerResponse(resp);
				NetworkGame::Instance()->ReceiveMasterList(this->masterResult);
			}
			
			//printf("Connecting to %s\n", packet->systemAddress.ToString(true));
			//rakPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
		}		
		break;
	case ID_READY_EVENT_ALL_SET:			
		break;

	case ID_READY_EVENT_SET:		
		break;

	case ID_READY_EVENT_UNSET:		
		break;
	}
	return true;
}
	
//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::SearchForGames()
{	
	if (this->state == NetworkServer::IDLE)
	{
		RakNet::SocketDescriptor sd;
		sd.socketFamily = AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
#if 0
		// use this to enable multiple clients
		sd.port = CLIENT_PORT + Math::n_irand(1,10);
#else
		sd.port = CLIENT_PORT;
#endif
		StartupResult sr = rakPeer->Startup(32, &sd, 1);
		RakAssert(sr == RAKNET_STARTED);
		this->rakPeer->SetMaximumIncomingConnections(32);
		this->rakPeer->SetTimeoutTime(CONNECTION_TIMEOUT, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		n_printf("Our guid is %s\n", this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
		n_printf("Started on %s\n", this->rakPeer->GetMyBoundAddress().ToString(true));
		this->state = NetworkServer::NETWORK_STARTED;
	}
	this->host = false;
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::CreateRoom()
{
	this->state = NetworkServer::SERVER_LOBBY;
	//this->rakPeer->Shutdown(100);

	RakNet::SocketDescriptor sd;
	sd.socketFamily = AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
	sd.port = SERVER_PORT;
    
	StartupResult sr = rakPeer->Startup(32, &sd, 1);
	RakAssert(sr == RAKNET_STARTED);
	this->rakPeer->SetMaximumIncomingConnections(32);
	this->rakPeer->SetTimeoutTime(CONNECTION_TIMEOUT, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	n_printf("Our guid is %s\n", this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	n_printf("Started on %s\n", this->rakPeer->GetMyBoundAddress().ToString(true));
	this->UpdateServerInfo();
	this->host = true;
}


//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::CancelRoom()
{
	if (this->state != NetworkServer::IDLE)
	{
		this->rakPeer->Shutdown(100);
	}	
	this->host = false;
	this->state = IDLE;
}


//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::StartGame()
{	
	this->state = IN_GAME;	
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::UpdateServerInfo()
{
	Util::String name = NetworkGame::Instance()->GetGameName().AsBase64();

    cJSON * entry = cJSON_CreateObject();
    cJSON_AddStringToObject(entry, "__gameId", NetworkGame::Instance()->GetGameID().AsCharPtr());
    cJSON_AddStringToObject(entry, "roomName", name.AsCharPtr());
    cJSON_AddStringToObject(entry, "guid", this->rakPeer->GetMyGUID().ToString());
    cJSON_AddStringToObject(entry, "__timeoutSec", "1.0");
    cJSON_AddStringToObject(entry, "__addr", this->rakPeer->GetInternalID().ToString());
    cJSON_AddNumberToObject(entry, "__rowId", 1);
    cJSON_AddNumberToObject(entry, "currentPlayers", NetworkGame::Instance()->GetCurrentAmountOfPlayers());
    cJSON_AddNumberToObject(entry, "maxPlayers", NetworkGame::Instance()->GetMaxPlayers());

	
    cJSON * root = cJSON_CreateObject();
    cJSON * arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, entry);
    cJSON_AddItemReferenceToObject(root, "GET", arr);	
    char* json_string = cJSON_Print(root);
    this->serverInfoString = json_string;
    free(json_string);	
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::Connect(const RakNet::RakNetGUID &guid)
{
	Ptr<Attr::AttributeTable> table = NetworkGame::Instance()->GetMasterList();
	IndexT row = table->FindRowIndexByAttr(Attr::Attribute(Attr::Id, guid.ToString()));
	n_assert(row != InvalidIndex);
	RakNet::SystemAddress client(table->GetString(Attr::GameServerAddress, row).AsCharPtr());

	this->rakPeer->Connect(client.ToString(false),client.GetPort(),0,0);
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::ConnectDirect(const RakNet::SystemAddress &client)
{
    this->rakPeer->Connect(client.ToString(false), client.GetPort(), 0, 0);
}

} // namespace MultiplayerFeature
