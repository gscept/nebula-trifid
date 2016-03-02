//------------------------------------------------------------------------------
//  natnetworkserver.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "natnetworkserver.h"
#include "threading/thread.h"
#include <time.h>
#include "FullyConnectedMesh2.h"
#include "NatPunchthroughClient.h"
#include "networkgame.h"
#include "MessageIdentifiers.h"
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
#include "attr/attributetable.h"
#include "multiplayerattrs.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "syncpoint.h"
#include "networkentity.h"
#include "bitreader.h"
#include "networkserver.h"
#include "http/httpclientregistry.h"

namespace MultiplayerFeature
{
__ImplementClass(MultiplayerFeature::NatNetworkServer, 'NNNS', MultiplayerFeature::NetworkServer);
__ImplementClass(MultiplayerFeature::NatNetworkServer::MasterHelperThread, 'msht', Threading::Thread);


__ImplementInterfaceSingleton(MultiplayerFeature::NatNetworkServer);

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
NatNetworkServer::NatNetworkServer() :
    natPunchthroughClient(NULL),
	fullyConnectedMesh(NULL),
	natServer(DEFAULT_SERVER_ADDRESS),
	connectedToNatPunchThrough(false),
	masterServerRow(-1)
{
	__ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
NatNetworkServer::~NatNetworkServer()
{
	__DestructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::Open()
{
	NetworkServer::Open();
	
	this->fullyConnectedMesh = FullyConnectedMesh2::GetInstance();
	this->natPunchthroughClient = NatPunchthroughClient::GetInstance();
	
	// attach raknet plugins 
	this->rakPeer->AttachPlugin(this->fullyConnectedMesh);
	this->rakPeer->AttachPlugin(this->natPunchthroughClient);	
}

//------------------------------------------------------------------------------
/**
*/
bool
NatNetworkServer::SetupLowlevelNetworking()
{
	n_assert2(NetworkGame::HasInstance(), "No NetworkGame or subclass instance exists, cant continue\n");

	this->fullyConnectedMesh->SetAutoparticipateConnections(false);
	this->fullyConnectedMesh->SetConnectOnNewRemoteConnection(false, "");
	this->replicationManager->SetNetworkIDManager(this->networkIDManager);
	this->replicationManager->SetAutoManageConnections(false, true);

	NetworkServer::SetupLowlevelNetworking();

	RakNet::SocketDescriptor sd;
	sd.socketFamily = AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
	sd.port = 0;
	StartupResult sr = rakPeer->Startup(8, &sd, 1);
	RakAssert(sr == RAKNET_STARTED);
	this->rakPeer->SetMaximumIncomingConnections(8);
	this->rakPeer->SetTimeoutTime(CONNECTION_TIMEOUT, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	n_printf("Our guid is %s\n", this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	n_printf("Started on %s\n", this->rakPeer->GetMyBoundAddress().ToString(true));

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
NatNetworkServer::ShutdownLowlevelNetworking()
{
	this->replicationManager->Clear();
	this->fullyConnectedMesh->Clear();
	this->rakPeer->Shutdown(100, 0);	
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::Close()
{
	NetworkServer::Close();
	delete this->fullyConnectedMesh;
	delete this->natPunchthroughClient;	
}

//------------------------------------------------------------------------------
/**
*/
void 
NatNetworkServer::OnFrame()
{        	
	if (this->doneFlag.TestAndClearIfSet())
	{
		NetworkGame::Instance()->ReceiveMasterList(this->masterResult);
		this->masterResult = 0;
	}
	NetworkServer::OnFrame();	
}

//------------------------------------------------------------------------------
/**
*/
bool
NatNetworkServer::HandlePacket(RakNet::Packet * packet)
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
		NetworkGame::Instance()->OnPlayerDisconnect(packet->guid);
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
		NetworkGame::Instance()->OnJoinFailed("Connection failed");
	}
	break;
	case ID_FCM2_VERIFIED_JOIN_REJECTED:	
	{
		RakNet::BitStream bs(packet->data, packet->length, false);
		Ptr<Multiplayer::BitReader> br = Multiplayer::BitReader::Create();
		br->SetStream(&bs);
		br->ReadChar();
		Util::String answer = br->ReadString();

		n_printf("Failed to join game session: %s", answer.AsCharPtr());
		NetworkGame::Instance()->OnJoinFailed(answer);
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
		for (unsigned int i = 0;i < meshclients.Size();i++)
		{
			this->participants.Append(meshclients[i]);
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
			if (car != ALREADY_CONNECTED_TO_ENDPOINT || car != RakNet::CONNECTION_ATTEMPT_STARTED)
			{
				n_warning("Nat punchthrough failed\n");
			}
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
*/
void
NatNetworkServer::SearchForGames()
{	
	this->UpdateRoomList();
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::MasterServerResult(Util::String response)
{	
	this->masterResult = 0;
	this->ParseServerResponse(response);
	NatNetworkServer::Instance()->doneFlag.Set();
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::UpdateRoomList()
{
	this->masterThread = MasterHelperThread::Create();
	this->masterThread->gameId = NetworkGame::Instance()->GetGameID();	
	this->masterThread->Start();
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::Connect(const RakNet::RakNetGUID &guid)
{
	this->natPunchthroughClient->OpenNAT(guid, this->natPunchServerAddress);
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::CreateRoom()
{
	NetworkServer::CreateRoom();
	this->PublishToMaster();
}


//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::CancelRoom()
{
	NetworkServer::CancelRoom();
	
}


//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::StartGame()
{	
	NetworkServer::StartGame();
}


//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::PublishToMaster()
{
#if 0
	this->delayedMaster = false;
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
	Util::String gamename = NetworkGame::Instance()->GetGameName().AsBase64();
	// we dont want linefeeds
	gamename.Strip("\r");
	req.Format("{'__gameId':'%s','__clientReqId': '0','__timeoutSec': '30','roomName':'%s','guid':'%s','currentPlayers':%d,'maxPlayers':%d %s}",
		NetworkGame::Instance()->GetGameID().AsCharPtr(), gamename.AsCharPtr(), NetworkServer::Instance()->GetRakPeerInterface()->GetMyGUID().ToString(),
		NetworkGame::Instance()->GetCurrentAmountOfPlayers(), NetworkGame::Instance()->GetMaxPlayers(), rowStr.AsCharPtr());
	n_printf("%s\n", req.AsCharPtr());
	Http::HttpStatus::Code res = client->SendRequest(Http::HttpMethod::Post, requri, req, stream.cast<IO::Stream>());
	n_printf("Publish: %s\n", Http::HttpStatus::ToHumanReadableString(res).AsCharPtr());
	if (res == Http::HttpStatus::OK)
	{
		Util::String buf;
		buf.Set((const char*)stream->GetRawPointer(), stream->GetSize());
		json_error_t error;
		json_t * root = json_loads(buf.AsCharPtr(), JSON_REJECT_DUPLICATES, &error);
		if (NULL == root)
		{
			n_warning("error parsing json from master server\n");
			Http::HttpClientRegistry::Instance()->ReleaseConnection(serverUri);
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
	else if (res == Http::HttpStatus::ServiceUnavailable)
	{
		// sometimes master server times out, try again later
		this->delayedMaster = true;
	}
	Http::HttpClientRegistry::Instance()->ReleaseConnection(serverUri);
#endif
}


//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::UnpublishFromMaster()
{
	Util::String req;
	req.Format("http://" MASTER_SERVER_ADDRESS "/testServer");
	IO::URI serverUri = req;
	Ptr<Http::HttpClient> client = Http::HttpClientRegistry::Instance()->ObtainConnection(serverUri);
	IO::URI requri = req;
	req.Format("testServer?__gameId=%s&__rowId=%d", NetworkGame::Instance()->GetGameID().AsCharPtr(), this->masterServerRow);
	requri.SetLocalPath(req);
	Ptr<IO::MemoryStream> stream = IO::MemoryStream::Create();
	stream->SetAccessMode(IO::Stream::ReadWriteAccess);
	stream->Open();
	Http::HttpStatus::Code res = client->SendRequest(Http::HttpMethod::Delete, requri, stream.cast<IO::Stream>());
	n_printf("Unpublish: %s\n", Http::HttpStatus::ToHumanReadableString(res).AsCharPtr());
	Http::HttpClientRegistry::Instance()->ReleaseConnection(serverUri);
	this->masterServerRow = -1;
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::ConnectDirect(const RakNet::SystemAddress &addr)
{
    n_error("not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void
NatNetworkServer::MasterHelperThread::DoWork()
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
	NatNetworkServer::Instance()->MasterServerResult(response);
}
} // namespace MultiplayerFeature
