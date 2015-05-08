//------------------------------------------------------------------------------
//  networkserver.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "networkserver.h"
#include "debug/debugserver.h"            
#include "http/html/htmlpagewriter.h"
#include "http/svg/svglinechartwriter.h"
#include "threading/thread.h"
#include <time.h>
#include "FullyConnectedMesh2.h"
#include "NetworkIDManager.h"
#include "RakPeerInterface.h"
#include "TCPInterface.h"
#include "NatPunchthroughClient.h"
#include "RPC4Plugin.h"
#include "ReadyEvent.h"
#include "HTTPConnection2.h"
#include "networkgame.h"
#include "MessageIdentifiers.h"
#include "PacketLogger.h"
#include "networkplayer.h"
#include "multiplayerfeatureunit.h"
//FIXME i dont feel like adding another global define just for this
#define MINIUPNP_STATICLIB
#include "miniupnp/miniupnpc/upnpcommands.h"
#include "GetTime.h"
#include "miniupnp/miniupnpc/miniupnpc.h"
#include "miniupnp/miniupnpc/upnperrors.h"
#include "app/application.h"
#include "http/httpclient.h"
#include "io/memorystream.h"
#include "jansson/src/jansson.h"
#include "attr/attributetable.h"
#include "multiplayerattrs.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "CloudClient.h"
#include "syncpoint.h"

namespace MultiplayerFeature
{
__ImplementClass(MultiplayerFeature::NetworkServer, 'MNNS', Core::RefCounted);
__ImplementClass(MultiplayerFeature::NetworkServer::MasterHelperThread, 'msht', Threading::Thread);


__ImplementInterfaceSingleton(MultiplayerFeature::NetworkServer);

using namespace Util;

using namespace Timing;
using namespace Http;
using namespace Debug;
using namespace Math;
using namespace RakNet;


#if NEBULA3_DEBUG
#define CONNECTION_TIMEOUT 3000000 // ms 3000s
#else
#define CONNECTION_TIMEOUT 10000 // ms 10s
#endif

//------------------------------------------------------------------------------
/**
*/
NetworkServer::NetworkServer() :
    state(IDLE),
	rakPeer(NULL),
	networkIDManager(NULL),	
	natPunchthroughClient(NULL),
	rpc(NULL),
	fullyConnectedMesh(NULL),
	readyEvent(NULL),
	natServer(DEFAULT_SERVER_ADDRESS),
	connectedToNatPunchThrough(false)
{
	__ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
NetworkServer::~NetworkServer()
{
	__DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::Open()
{
	// init raknet
	this->rakPeer = RakPeerInterface::GetInstance();
	this->fullyConnectedMesh = FullyConnectedMesh2::GetInstance();
	this->networkIDManager = NetworkIDManager::GetInstance();
	this->natPunchthroughClient = NatPunchthroughClient::GetInstance();
	this->rpc = RPC4::GetInstance();
	this->readyEvent = Multiplayer::SyncPoint::GetInstance();
	this->replicationManager = ReplicationManager::Create();
	this->cloudClient = new CloudClient;

	Multiplayer::SyncPoint::SetupSyncPoint(this->readyEvent);
	// attach raknet plugins 
	this->rakPeer->AttachPlugin(this->fullyConnectedMesh);
	this->rakPeer->AttachPlugin(this->natPunchthroughClient);
	this->rakPeer->AttachPlugin(this->rpc);
	this->rakPeer->AttachPlugin(this->readyEvent);
	this->rakPeer->AttachPlugin(this->replicationManager);
	this->rakPeer->AttachPlugin(this->cloudClient);
	this->fullyConnectedMesh->SetAutoparticipateConnections(false);
	this->fullyConnectedMesh->SetConnectOnNewRemoteConnection(false, "");
	this->replicationManager->SetNetworkIDManager(this->networkIDManager);
	this->replicationManager->SetAutoManageConnections(false,true);   
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkServer::SetupLowlevelNetworking()
{
	n_assert2(NetworkGame::HasInstance(), "No NetworkGame or subclass instance exists, cant continue\n");

	Ptr<NetworkGame> game = NetworkGame::Instance();
	game->SetNetworkIDManager(this->networkIDManager);
	game->SetNetworkID(0);
	this->replicationManager->Reference(game);

	Ptr<NetworkPlayer> user = MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetPlayer();
	user->SetNetworkIDManager(this->networkIDManager);
	user->SetUniqueId(this->rakPeer->GetMyGUID());
	n_printf("my id: %s\n", this->rakPeer->GetMyGUID().ToString());

	RakNet::SocketDescriptor sd;
	sd.socketFamily = AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
	sd.port = 0;
	StartupResult sr = rakPeer->Startup(8, &sd, 1);
	RakAssert(sr == RAKNET_STARTED);
	this->rakPeer->SetMaximumIncomingConnections(8);
	this->rakPeer->SetTimeoutTime(CONNECTION_TIMEOUT, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	n_printf("Our guid is %s\n", this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	n_printf("Started on %s\n", this->rakPeer->GetMyBoundAddress().ToString(true));

	
	// Start TCPInterface and begin connecting to the NAT punchthrough server
//	this->tcp->Start(0, 0, 1);

	this->natPunchServerAddress.FromStringExplicitPort(this->natServer.AsCharPtr(), DEFAULT_SERVER_PORT);

	this->state = NETWORK_STARTED;
	ConnectionAttemptResult car = this->rakPeer->Connect(this->natServer.AsCharPtr(), DEFAULT_SERVER_PORT, 0, 0);
	if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		this->state = IDLE;
		return false;
	}
	
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::Close()
{
	this->rakPeer->Shutdown(100, 0);
	RakNet::RakPeerInterface::DestroyInstance(this->rakPeer);
	delete this->fullyConnectedMesh;
	delete this->networkIDManager;
	delete this->natPunchthroughClient;
	delete this->rpc;
	delete this->readyEvent;
	this->replicationManager = 0;
	delete this->cloudClient;

}

//------------------------------------------------------------------------------
/**
*/
void 
NetworkServer::OnFrame()
{        
	if (this->state != IDLE)
	{	
		Packet *packet;
		for (packet = this->rakPeer->Receive(); packet; this->rakPeer->DeallocatePacket(packet), packet = this->rakPeer->Receive())
		{
			this->HandlePacket(packet);
		}		
	}
	if (this->doneFlag.TestAndClearIfSet())
	{
		NetworkGame::Instance()->ReceiveMasterList(this->masterResult);
		this->masterResult = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkServer::HandlePacket(RakNet::Packet * packet)
{
	Util::String targetName = packet->systemAddress.ToString(true);
	switch (packet->data[0])
	{
	case ID_IP_RECENTLY_CONNECTED:
	{
		n_printf("This IP address recently connected from %s\n", targetName.AsCharPtr());
		if (packet->systemAddress == this->natPunchServerAddress)
		{
			n_printf("Multiplayer will not work without the NAT punchthrough server!");
		}			
	}
	break;
	case ID_INCOMPATIBLE_PROTOCOL_VERSION:
	{
		n_printf("Incompatible protocol version from %s\n", targetName.AsCharPtr());
		if (packet->systemAddress == this->natPunchServerAddress)
		{
			n_printf("Multiplayer will not work without the NAT punchthrough server!");
		}
	}
	break;
	case ID_DISCONNECTION_NOTIFICATION:
	{
		n_printf("Disconnected from %s\n", targetName.AsCharPtr());
		if (packet->systemAddress == this->natPunchServerAddress)
		{
			this->connectedToNatPunchThrough = false;
		}			
	}
	break;
	case ID_CONNECTION_LOST:
	{
		n_printf("Connection to %s lost\n", targetName.AsCharPtr());
		if (packet->systemAddress == this->natPunchServerAddress)
		{
			this->connectedToNatPunchThrough = false;
		}			
	}
	break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
	{
		n_printf("No free incoming connections to %s\n", targetName.AsCharPtr());
		if (packet->systemAddress == this->natPunchServerAddress)
		{
			n_printf("Multiplayer will not work without the NAT punchthrough server!");
		}			
	}
	break;
	case ID_NEW_INCOMING_CONNECTION:
	{
		if (this->fullyConnectedMesh->IsHostSystem())
		{
			n_printf("Sending player list to new connection: %s\n",packet->guid.ToString());
			this->fullyConnectedMesh->StartVerifiedJoin(packet->guid);
		}
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
			natPunchthroughClient->OpenNAT(guids[i], this->natPunchServerAddress);
		}			
	}
	break;
	case ID_FCM2_VERIFIED_JOIN_FAILED:
	{
		n_printf("Failed to join game session");
	}
	break;
	case ID_FCM2_VERIFIED_JOIN_CAPABLE:
	{
		if (this->fullyConnectedMesh->GetParticipantCount() < NetworkGame::Instance()->GetMaxPlayers())
		{
			this->fullyConnectedMesh->RespondOnVerifiedJoinCapable(packet, true, 0);
		}
		else
		{
			this->fullyConnectedMesh->RespondOnVerifiedJoinCapable(packet, false, 0);
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
	}
	break;
	case ID_CONNECTION_REQUEST_ACCEPTED:
	{
		n_printf("Connection request to %s accepted\n", targetName.AsCharPtr());
		n_printf("connection guid: %s\n", packet->guid.ToString());		
		if (packet->systemAddress == this->natPunchServerAddress)
		{
			this->connectedToNatPunchThrough = true;

			// Open UPNP.
			struct UPNPDev * devlist = 0;
			devlist = upnpDiscover(1000, 0, 0, 0,0,0);
			if (devlist)
			{
				char lanaddr[64];	/* my ip address on the LAN */
				struct UPNPUrls urls;
				struct IGDdatas data;
				if (UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr)) == 1)
				{
					// External port is the port people will be connecting to us on. This is our port as seen by the directory server
					// Internal port is the port RakNet was internally started on
					char eport[32], iport[32];
					natPunchthroughClient->GetUPNPPortMappings(eport, iport, this->natPunchServerAddress);

					int r = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
						eport, iport, lanaddr, 0, "UDP", 0,"0");

					if (r == UPNPCOMMAND_SUCCESS)
					{
						// UPNP done
					}

				}
			}
		}
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
	case ID_CLOUD_GET_RESPONSE:
	{
		RakNet::CloudQueryResult cloudQueryResult;
		cloudClient->OnGetReponse(&cloudQueryResult, packet);
		if (cloudQueryResult.rowsReturned.Size() > 0)
		{
			n_printf("NAT punch to existing game instance");
			natPunchthroughClient->OpenNAT(cloudQueryResult.rowsReturned[0]->clientGUID, this->natPunchServerAddress);
		}
		else
		{
			n_printf("Publishing new game instance");

			// Start as a new game instance because no other games are running
			this->UpdateRoomList();
		}

		cloudClient->DeallocateWithDefaultAllocator(&cloudQueryResult);
	}
	break;
	case ID_CONNECTION_ATTEMPT_FAILED:
	{
		n_printf("Connection attempt to %s failed\n", targetName.AsCharPtr());
		if (packet->systemAddress == this->natPunchServerAddress)
		{
			n_printf("Multiplayer will not work without the NAT punchthrough server!");
		}			
	}
	break;
	case ID_NAT_TARGET_NOT_CONNECTED:
	{
		RakNet::RakNetGUID recipientGuid;
		RakNet::BitStream bs(packet->data, packet->length, false);
		bs.IgnoreBytes(sizeof(RakNet::MessageID));
		bs.Read(recipientGuid);
		targetName = recipientGuid.ToString();
		n_printf("NAT target %s not connected\n", targetName.AsCharPtr());		
	}
	break;
	case ID_NAT_TARGET_UNRESPONSIVE:
	{
		RakNet::RakNetGUID recipientGuid;
		RakNet::BitStream bs(packet->data, packet->length, false);
		bs.IgnoreBytes(sizeof(RakNet::MessageID));
		bs.Read(recipientGuid);
		targetName = recipientGuid.ToString();
		n_printf("NAT target %s unresponsive\n", targetName.AsCharPtr());
	}
	break;
	case ID_NAT_CONNECTION_TO_TARGET_LOST:
	{
		RakNet::RakNetGUID recipientGuid;
		RakNet::BitStream bs(packet->data, packet->length, false);
		bs.IgnoreBytes(sizeof(RakNet::MessageID));
		bs.Read(recipientGuid);
		targetName = recipientGuid.ToString();
		n_printf("NAT target connection to %s lost\n", targetName.AsCharPtr());		
	}
	break;
	case ID_NAT_ALREADY_IN_PROGRESS:
	{
		RakNet::RakNetGUID recipientGuid;
		RakNet::BitStream bs(packet->data, packet->length, false);
		bs.IgnoreBytes(sizeof(RakNet::MessageID));
		bs.Read(recipientGuid);
		targetName = recipientGuid.ToString();
		n_printf("NAT target connection to %s already in progress, skipping\n", targetName.AsCharPtr());
	}
	break;

	case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
	{
		if (packet->data[1] == 1)
		{
			n_printf("Connecting to existing game instance");
			RakNet::ConnectionAttemptResult car = rakPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
			RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);
		}
	}
	break;

	case ID_ADVERTISE_SYSTEM:
		if (packet->guid != rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS))
		{
			char hostIP[32];
			packet->systemAddress.ToString(false, hostIP);
			RakNet::ConnectionAttemptResult car = rakPeer->Connect(hostIP, packet->systemAddress.GetPort(), 0, 0);
			RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);
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

bool
NetworkServer::HandlePacket(RakNet::Packet * packet)
{
	switch (packet->data[0])
	{
		case ID_NEW_INCOMING_CONNECTION:
		{
			n_printf("ID_NEW_INCOMING_CONNECTION from %s. guid=%s.\n", packet->systemAddress.ToString(true), packet->guid.ToString());
		}
		break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			n_printf("ID_CONNECTION_REQUEST_ACCEPTED from %s,guid=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
			this->natPunchServerAddress = packet->systemAddress;
			this->natPunchServerGuid = packet->guid;

			if (this->state == CONNECTING_TO_SERVER)
			{			
				this->SetupUPNP();	
			}
			else if (this->state == CONNECTING_TO_GAME_HOST)
			{
				n_printf("Asking host to join session...\n");

				// So time in single player does not count towards which system has been running multiplayer the longest
				this->fullyConnectedMesh->ResetHostCalculation();

				// Custom message to ask to join the game
				// We first connect to the game host, and the game host is responsible for calling StartVerifiedJoin() for us to join the session
				// FIXME
				BitStream bsOut;
				bsOut.Write((MessageID)ID_USER_PACKET_ENUM);
				this->rakPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->guid, false);
			}
		}
		break;
		case ID_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
			if (this->state == NAT_PUNCH_TO_GAME_HOST)
			{
				n_printf("Lost connection during NAT punch to game host. Reason %s.\n", PacketLogger::BaseIDTOString(packet->data[0]));
				this->state = SEARCH_FOR_GAMES;
			}
			else
			{
				if (packet->guid == this->natPunchServerGuid)
				{
					n_printf("Server connection lost. Reason %s.\nGame session is no longer searchable.\n", PacketLogger::BaseIDTOString(packet->data[0]));
				}
				else
				{
					n_printf("Peer connection lost. Reason %s.\n", PacketLogger::BaseIDTOString(packet->data[0]));
				}
			}
			break;

		case ID_ALREADY_CONNECTED:
			n_printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", packet->guid);
			break;

		case ID_INVALID_PASSWORD:
		case ID_NO_FREE_INCOMING_CONNECTIONS:
		case ID_CONNECTION_ATTEMPT_FAILED:
		case ID_CONNECTION_BANNED:
		case ID_IP_RECENTLY_CONNECTED:
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			// Note: Failing to connect to another system does not automatically mean we cannot join a session, since that system may be disconnecting from the host simultaneously
			// FullyConnectedMesh2::StartVerifiedJoin() internally handles success or failure and notifies the client through ID_FCM2_VERIFIED_JOIN_FAILED if needed.
			n_printf("Failed to connect to %s. Reason %s\n", packet->systemAddress.ToString(true), PacketLogger::BaseIDTOString(packet->data[0]));

			if (this->state == CONNECTING_TO_SERVER)
			{
				this->state = IDLE;
			}				
			break;

		case ID_FCM2_NEW_HOST:
		{
			n_printf("ID_FCM2_NEW_HOST\n");
			RakNet::BitStream bs(packet->data, packet->length, false);
			bs.IgnoreBytes(1);
			RakNetGUID oldHost;
			bs.Read(oldHost);

			if (packet->guid == rakPeer->GetMyGUID())
			{
				if (oldHost != UNASSIGNED_RAKNET_GUID)
				{
					if (this->state == IN_LOBBY_WAITING_FOR_HOST)
					{
						this->state = IN_LOBBY_WITH_HOST;
					}						
					NetworkGame::Instance()->PublishToMaster();
					n_printf("ID_FCM2_NEW_HOST: Taking over as host from the old host.\n");
				}
				else
				{
					// Room not hosted if we become host the first time since this was done in CreateRoom() already
					n_printf("ID_FCM2_NEW_HOST: We have become host for the first time.\n");
				}

				//printf("(L)ock and unlock game\n");
			}
			else
			{
				if (oldHost != UNASSIGNED_RAKNET_GUID)
					n_printf("ID_FCM2_NEW_HOST: A new system %s has become host, GUID=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				else
					n_printf("ID_FCM2_NEW_HOST: System %s is host, GUID=%s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
			}

			if (oldHost == UNASSIGNED_RAKNET_GUID)
			{
				// First time calculated host. Add existing connections to ReplicaManager3
				DataStructures::List<RakNetGUID> participantList;
				this->fullyConnectedMesh->GetParticipantList(participantList);
				for (unsigned int i = 0; i < participantList.Size(); i++)
				{
					this->readyEvent->AddToWaitList(0, participantList[i]);					
				}
				// Reference the user we created (host or not)	
				this->replicationManager->Reference(MultiplayerFeature::MultiplayerFeatureUnit::Instance()->GetPlayer());				
			}
		}
		break;				
		case ID_NAT_TARGET_NOT_CONNECTED:
		case ID_NAT_TARGET_UNRESPONSIVE:
		case ID_NAT_CONNECTION_TO_TARGET_LOST:
		case ID_NAT_PUNCHTHROUGH_FAILED:
		{
			// As with connection failed, this does not automatically mean we cannot join the session
			// We only fail on ID_FCM2_VERIFIED_JOIN_FAILED
			n_printf("NAT punch to %s failed. Reason %s\n", packet->guid.ToString(), PacketLogger::BaseIDTOString(packet->data[0]));

			if (this->state == NAT_PUNCH_TO_GAME_HOST)
			{
				this->SearchForGames();
			}				
		}

		case ID_NAT_ALREADY_IN_PROGRESS:
			// Can ignore this
			break;

		case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
		{
			n_printf("ID_NAT_PUNCHTHROUGH_SUCCEEDED\n");
			if (this->state == NAT_PUNCH_TO_GAME_HOST || this->state == VERIFIED_JOIN)
			{
				// Connect to the session host
				ConnectionAttemptResult car = this->rakPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
				if (car != RakNet::CONNECTION_ATTEMPT_STARTED)
				{
					n_printf("Failed connect call to %s. Code=%i\n", packet->systemAddress.ToString(false), car);
					this->SearchForGames();
				}
				else
				{
					if (this->state == NAT_PUNCH_TO_GAME_HOST)
					{
						n_printf("NAT punch completed. Connecting to %s (game host)...\n", packet->systemAddress.ToString(true));
						this->state = CONNECTING_TO_GAME_HOST;
					}
					else
					{
						n_printf("NAT punch completed. Connecting to %s (game client)...\n", packet->systemAddress.ToString(true));
					}
				}
			}
		}
		break;

		case ID_READY_EVENT_ALL_SET:
			n_printf("Got ID_READY_EVENT_ALL_SET from %s\n", packet->systemAddress.ToString(true));
// 			printf("All users ready.\n");
// 			if (fullyConnectedMesh2->IsConnectedHost())
// 				printf("New options:\n(B)egin gameplay\n");
			break;

		case ID_READY_EVENT_SET:
			n_printf("Got ID_READY_EVENT_SET from %s\n", packet->systemAddress.ToString(true));
			break;

		case ID_READY_EVENT_UNSET:
			n_printf("Got ID_READY_EVENT_UNSET from %s\n", packet->systemAddress.ToString(true));
			break;

			// ID_USER_PACKET_ENUM is used by this sample as a custom message to ask to join a game
			//FIXME
		case ID_USER_PACKET_ENUM:
			if (this->state >  SEARCH_FOR_GAMES)
			{
				n_printf("Got request from client to join session.\nExecuting StartVerifiedJoin()\n");
				this->fullyConnectedMesh->StartVerifiedJoin(packet->guid);
			}
			else
			{
				BitStream bsOut;
				bsOut.Write((MessageID)(ID_USER_PACKET_ENUM + 1));
				this->rakPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->guid, false);
			}
			break;		
		case ID_FCM2_VERIFIED_JOIN_START:
		{
			this->state = VERIFIED_JOIN;

			// This message means the session host sent us a list of systems in the session
			// Once we connect to, or fail to connect to, each of these systems we will get ID_FCM2_VERIFIED_JOIN_FAILED, ID_FCM2_VERIFIED_JOIN_ACCEPTED, or ID_FCM2_VERIFIED_JOIN_REJECTED
			n_printf("Host sent us system list. Doing NAT punch to each system...\n");
			DataStructures::List<SystemAddress> addresses;
			DataStructures::List<RakNetGUID> guids;
			DataStructures::List<BitStream*> ud;
			this->fullyConnectedMesh->GetVerifiedJoinRequiredProcessingList(packet->guid, addresses, guids, ud);
			for (unsigned int i = 0; i < guids.Size(); i++)
			{
				this->natPunchthroughClient->OpenNAT(guids[i], this->natPunchServerAddress);
			}				
		}
		break;

		case ID_FCM2_VERIFIED_JOIN_CAPABLE:
			n_printf("Client is capable of joining FullyConnectedMesh2.\n");
// 			if (game->lockGame)
// 			{
// 				RakNet::BitStream bsOut;
// 				bsOut.Write("Game is locked");
// 				fullyConnectedMesh2->RespondOnVerifiedJoinCapable(packet, false, &bsOut);
// 			}
// 			else

			this->fullyConnectedMesh->RespondOnVerifiedJoinCapable(packet, true, 0);
			break;

		case ID_FCM2_VERIFIED_JOIN_ACCEPTED:
		{
			DataStructures::List<RakNetGUID> systemsAccepted;
			bool thisSystemAccepted;
			this->fullyConnectedMesh->GetVerifiedJoinAcceptedAdditionalData(packet, &thisSystemAccepted, systemsAccepted, 0);
			if (thisSystemAccepted)
				n_printf("Game join request accepted\n");
			else
				n_printf("System %s joined the mesh\n", systemsAccepted[0].ToString());

			// Add the new participant to the game if we already know who the host is. Otherwise do this
			// once ID_FCM2_NEW_HOST arrives
			if (this->fullyConnectedMesh->GetConnectedHost() != UNASSIGNED_RAKNET_GUID)
			{
				// FullyConnectedMesh2 already called AddParticipant() for each accepted system
				// Still need to add those systems to the other plugins though
				for (unsigned int i = 0; i < systemsAccepted.Size(); i++)
				{
					this->readyEvent->AddToWaitList(0, systemsAccepted[i]);					
				}					

				if (thisSystemAccepted)
				{
					this->state = IN_LOBBY_WITH_HOST;
				}					
			}
			else
			{
				if (thisSystemAccepted)
				{
					this->state = IN_LOBBY_WAITING_FOR_HOST;
				}					
			}			
		}
		break;

		case ID_FCM2_VERIFIED_JOIN_REJECTED:
		{
			BitStream additionalData;
			this->fullyConnectedMesh->GetVerifiedJoinRejectedAdditionalData(packet, &additionalData);
			RakString reason;
			additionalData.Read(reason);
			n_printf("Join rejected. Reason=%s\n", reason.C_String());
			this->rakPeer->CloseConnection(packet->guid, true);
			this->SearchForGames();
			break;
		}

		case ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE:
		{
			if (this->replicationManager->GetAllConnectionDownloadsCompleted() == true)
			{
				n_printf("Completed all remote downloads\n");

				if (NetworkGame::Instance()->InLobby())
				{
					this->state = IN_LOBBY_WITH_HOST;
				}
				else
				{
					this->state = IN_GAME;
					NetworkGame::Instance()->StartInGame();
				}					
			}

			break;
		}
	}
	return true;
}


*/


//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::SearchForGames()
{	
	this->UpdateRoomList();
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::MasterServerResult(Util::String response)
{	
	n_printf("got master server results:\n%s", response.AsCharPtr());
	json_error_t error;
	json_t * root = json_loads(response.AsCharPtr(), JSON_REJECT_DUPLICATES, &error);
	if (NULL == root)
	{
		n_warning("error parsing json from master server\n");
		// FIXME which state now
		return;
	}

	Ptr<Attr::AttributeTable> table = Attr::AttributeTable::Create();
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
				IndexT row = table->AddRow();
				json_t* object = json_array_get(jsonArray, i);

				json_t* val = json_object_get(object, "roomName");
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
	NetworkServer::Instance()->masterResult = table;
	NetworkServer::Instance()->doneFlag.Set();	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::UpdateRoomList()
{
#ifdef USE_CLOUD_MASTER
	RakNet::CloudQuery cloudQuery;
	cloudQuery.keys.Push(RakNet::CloudKey(NetworkGame::Instance()->GetGameID().AsCharPtr(), 0), _FILE_AND_LINE_);
	cloudClient->Get(&cloudQuery, rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS));
#else
	this->masterThread = MasterHelperThread::Create();
	this->masterThread->gameId = NetworkGame::Instance()->GetGameID();	
	this->masterThread->Start();
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::NatConnect(const RakNet::RakNetGUID &guid)
{
	this->natPunchthroughClient->OpenNAT(guid, this->natPunchServerAddress);
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
NetworkServer::StartGame()
{	
	this->state = IN_GAME;	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkServer::MasterHelperThread::DoWork()
{
	Ptr<Http::HttpClient> client = Http::HttpClient::Create();
	Util::String serverString = "http://" MASTER_SERVER_ADDRESS;
	Util::String requestString;
	requestString.Format("testServer?__gameId=%s", this->gameId.AsCharPtr());
	IO::URI serverUri(serverString);
	IO::URI requestUri(serverString);
	requestUri.SetLocalPath(requestString);
	
	n_printf("%s\n", requestUri.LocalPath().AsCharPtr());
	Ptr<IO::MemoryStream> stream = IO::MemoryStream::Create();
	stream->SetAccessMode(IO::Stream::ReadWriteAccess);
	stream->Open();
	client->Connect(serverUri);
	Http::HttpStatus::Code res = client->SendRequest(Http::HttpMethod::Get, requestUri, stream.cast<IO::Stream>());	
	
	if (res != HttpStatus::OK)
	{
		n_warning("Failed to do http request to masterserver\n");
		//FIXME what status should we go to
		return;
	}
	Util::String response;
 	response.Set((const char*)stream->GetRawPointer(), stream->GetSize());
	NetworkServer::Instance()->MasterServerResult(response);
}
} // namespace RakNet
