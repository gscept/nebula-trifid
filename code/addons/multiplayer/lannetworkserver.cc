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
	serverStarted(false)
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
}

//------------------------------------------------------------------------------
/**
*/
bool
LanNetworkServer::SetupLowlevelNetworking()
{
	NetworkServer::SetupLowlevelNetworking();
	this->replicationManager->SetNetworkIDManager(this->networkIDManager);
	this->replicationManager->SetAutoManageConnections(true, true);
		
	
	this->state = IDLE;
	
	
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::ShutdownLowlevelNetworking()
{
	NetworkServer::ShutdownLowlevelNetworking();
	
}

//------------------------------------------------------------------------------
/**
*/
void
LanNetworkServer::Close()
{
	NetworkServer::Close();	
}

//------------------------------------------------------------------------------
/**
*/
void 
LanNetworkServer::OnFrame()
{
	if (this->state == SERVER_LOBBY)
	{
		RakNet::Time now = RakNet::GetTime();
		if (now - this->lastBroadcast > 4000)
		{
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
		IndexT idx = this->participants.FindIndex(packet->guid);
		if (idx != InvalidIndex)
		{
			this->participants.EraseIndex(idx);
		}
	}
	break;
	case ID_CONNECTION_LOST:
	{
		n_printf("Connection to %s lost\n", targetName.AsCharPtr());				
		IndexT idx = this->participants.FindIndex(packet->guid);
		if (idx != InvalidIndex)
		{
			this->participants.EraseIndex(idx);
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
		n_printf("ID_NEW_INCOMING_CONNECTION\n");
		this->participants.Append(packet->guid);
	}
	break;

	case ID_CONNECTION_REQUEST_ACCEPTED:
	{
		n_printf("Connection request to %s accepted\n", targetName.AsCharPtr());
		n_printf("connection guid: %s\n", packet->guid.ToString());	
		this->participants.Append(packet->guid);
		
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
		sd.port = CLIENT_PORT;

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

//------------------------------------------------------------------------------
/**
*/
bool
LanNetworkServer::HasHost() const
{
	return this->state > IDLE;
}

} // namespace MultiplayerFeature
