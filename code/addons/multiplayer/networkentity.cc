//------------------------------------------------------------------------------
//  networkentity.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "networkentity.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "replicationmanager.h"
#include "networkserver.h"
#include "RakPeerInterface.h"
#include "managers/entitymanager.h"
#include "bitwriter.h"
#include "bitreader.h"
#include "multiplayerattrs.h"
#include "io/binarywriter.h"
#include "basegamefeature/basegameprotocol.h"

using namespace RakNet;
using namespace Multiplayer;

namespace MultiplayerFeature
{
	__ImplementClass(MultiplayerFeature::NetworkEntity, 'NWGA', Game::Entity);

	

//------------------------------------------------------------------------------
/**
*/
NetworkEntity::NetworkEntity():
	levelEntity(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NetworkEntity::~NetworkEntity()
{
	// empty
}



//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(this->GetRtti()->GetFourCC().AsUInt());
	Ptr<BitWriter> writer = BitWriter::Create();
	writer->SetStream(allocationIdBitstream);
	writer->WriteString(this->GetCategory());
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3ConstructionState
NetworkEntity::QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3)
{
	RakNet::Replica3P2PMode mode = R3P2PM_SINGLE_OWNER;	
	if (this->levelEntity)
	{
		if (NetworkServer::Instance()->IsHost())
		{
			mode = R3P2PM_STATIC_OBJECT_CURRENTLY_AUTHORITATIVE;
		}
		else
		{
			mode = R3P2PM_STATIC_OBJECT_NOT_CURRENTLY_AUTHORITATIVE;
		}
	}
	return QueryConstruction_PeerToPeer(destinationConnection, mode);
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkEntity::QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	const Ptr<Db::ValueTable> & table = this->GetAttrTable();
	int count = table->GetNumColumns();
	IndexT row = this->GetAttrTableRowIndex();
	Ptr<BitWriter> writer = BitWriter::Create();
	writer->SetStream(constructionBitstream);

	// always ignore master attribute	
	writer->WriteInt(count-1);
	for (int i = 0; i < count; i++)
	{
		Attr::AttrId at = table->GetColumnId(i);
		if (at == Attr::IsMaster)
		{
			continue;
		}
		switch (at.GetValueType())
		{
		case Attr::IntType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteInt(table->GetInt(at, row));
		}
		break;
		case Attr::StringType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteString(table->GetString(at, row));
		}
		break;
		case Attr::FloatType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteFloat(table->GetFloat(at, row));
		}
		break;
		case Attr::BoolType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteBool(table->GetBool(at, row));
		}
		break;
		case Attr::Float4Type:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteFloat4(table->GetFloat4(at, row));
		}
		break;
		case Attr::Matrix44Type:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteMatrix44(table->GetMatrix44(at, row));
		}
		break;
		case Attr::GuidType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteGuid(table->GetGuid(at, row));
		}
		break;
		default:
			n_error("unhandled type\n");
			break;
		}
	}

}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkEntity::DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{	
	Ptr<BitReader> reader = BitReader::Create();
	reader->SetStream(constructionBitstream);
	int count = reader->ReadInt();
	for (int i = 0; i < count; i++)
	{
		Util::FourCC fc(reader->ReadInt());
		Attr::AttrId at(fc);
		switch (at.GetValueType())
		{
		case Attr::IntType:
		{			
			int val = reader->ReadInt();
			this->SetInt(at, val);
		}
		break;
		case Attr::StringType:
		{
			Util::String val = reader->ReadString();
			this->SetString(at, val);
		}
		break;
		case Attr::FloatType:
		{
			float val = reader->ReadFloat();
			this->SetFloat(at, val);
		}
		break;
		case Attr::BoolType:
		{
			bool val = reader->ReadBool();
			this->SetBool(at, val);
		}
		break;
		case Attr::Float4Type:
		{
			Math::float4 val = reader->ReadFloat4();
			this->SetFloat4(at, val);
		}
		break;
		case Attr::Matrix44Type:
		{
			Math::matrix44 val = reader->ReadMatrix44();
			this->SetMatrix44(at, val);
		}
		break;
		case Attr::GuidType:
		{
			Util::Guid val = reader->ReadGuid();
			this->SetGuid(at, val);
		}
		break;
		default:
			break;
		}		
	}
	return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::SerializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::DeserializeConstructionExisting(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkEntity::DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3ActionOnPopConnection
NetworkEntity::QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const
{
	if (this->HasAttr(Attr::_LevelEntity) && this->GetBool(Attr::_LevelEntity))
	{
		return RM3AOPC_DO_NOTHING;
	}
	else
	{
		return QueryActionOnPopConnection_PeerToPeer(droppedConnection);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::DeallocReplica(RakNet::Connection_RM3 *sourceConnection)
{
	// only remove if we are still active
	if (this->IsActive())
	{
		BaseGameFeature::EntityManager::Instance()->DeleteEntity(this);
	}	
	this->Release();
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3QuerySerializationResult
NetworkEntity::QuerySerialization(RakNet::Connection_RM3 *destinationConnection)
{
	if (!this->IsActive())
	{
		return RM3QSR_DO_NOT_CALL_SERIALIZE;
	}
	else
	{
		return this->QuerySerialize(destinationConnection);
	}
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3SerializationResult
NetworkEntity::Serialize(RakNet::SerializeParameters *serializeParameters)
{		
	Ptr<BitWriter> writer = BitWriter::Create();
	writer->SetStream(&serializeParameters->outputBitstream[0]);

	const Util::Array<Attr::AttrId> & syncedAttrs = BaseGameFeature::CategoryManager::Instance()->GetSyncedAttributes(this->GetCategory());
		
	int count = syncedAttrs.Size();
	// 255 attributes should be fine		
	n_assert(count < 256);
	writer->WriteUChar(count);
	for (int i = 0; i < count; i++)
	{
		Attr::AttrId at = syncedAttrs[i];
		switch (at.GetValueType())
		{
		case Attr::IntType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteInt(this->GetInt(at));
		}
		break;
		case Attr::StringType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteString(this->GetString(at));
		}
		break;
		case Attr::FloatType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteFloat(this->GetFloat(at));
		}
		break;
		case Attr::BoolType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteBool(this->GetBool(at));
		}
		break;
		case Attr::Float4Type:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteFloat4(this->GetFloat4(at));
		}
		break;
		case Attr::Matrix44Type:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteMatrix44(this->GetMatrix44(at));
		}
		break;
		case Attr::GuidType:
		{
			writer->WriteUInt(at.GetFourCC().AsUInt());
			writer->WriteGuid(this->GetGuid(at));
		}
		break;
		default:
			n_error("unhandled type\n");
			break;
		}
	}		

	return RakNet::RM3SR_BROADCAST_IDENTICALLY;	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	if (!this->IsActive())
	{
		return;
	}
	Ptr<BitReader> reader = BitReader::Create();
	reader->SetStream(&deserializeParameters->serializationBitstream[0]);
	bool hasTransform = false;
	int count = reader->ReadUChar();
	for (int i = 0; i < count; i++)
	{
		Util::FourCC fc(reader->ReadInt());
		Attr::AttrId at(fc);
		switch (at.GetValueType())
		{
		case Attr::IntType:
		{
			int val = reader->ReadInt();
			this->SetInt(at, val);
		}
		break;
		case Attr::StringType:
		{
			Util::String val = reader->ReadString();
			this->SetString(at, val);
		}
		break;
		case Attr::FloatType:
		{
			float val = reader->ReadFloat();
			this->SetFloat(at, val);
		}
		break;
		case Attr::BoolType:
		{
			bool val = reader->ReadBool();
			this->SetBool(at, val);
		}
		break;
		case Attr::Float4Type:
		{
			Math::float4 val = reader->ReadFloat4();
			this->SetFloat4(at, val);
		}
		break;
		case Attr::Matrix44Type:
		{
			if (at == Attr::Transform)
			{
				hasTransform = true;
			}
			Math::matrix44 val = reader->ReadMatrix44();
			this->SetMatrix44(at, val);
		}
		break;
		case Attr::GuidType:
		{
			Util::Guid val = reader->ReadGuid();
			this->SetGuid(at, val);
		}
		break;
		default:
			break;
		}
	}	
	if (hasTransform)
	{
		// we got a transform attribute update, send a settransform to update entity
		Ptr<BaseGameFeature::SetTransform> msg = BaseGameFeature::SetTransform::Create();
		msg->SetMatrix(this->GetMatrix44(Attr::Transform));
		this->SendSync(msg.cast<Messaging::Message>());
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::OnActivate()
{		
	IndexT row = this->GetAttrTableRowIndex();
	const Ptr<Db::ValueTable>& table = this->GetAttrTable();
	if (!table->HasColumn(Attr::IsMaster))
	{
		table->AddColumn(Attr::IsMaster);
	}
	
	// check if we have created this object and make us master
	if (!this->WasReferenced())
	{
		this->SetBool(Attr::IsMaster, true);
	}	
	else
	{
		this->SetBool(Attr::IsMaster, false);
	}

	Entity::OnActivate();

	if (!this->WasReferenced())
	{
		if (this->HasAttr(Attr::_LevelEntity))
		{
			this->levelEntity = this->GetBool(Attr::_LevelEntity);
			if (this->levelEntity)
			{
				this->SetNetworkIDManager(ReplicationManager::Instance()->GetNetworkIDManager());
				Util::String key = this->GetString(Attr::_ID);
				int64_t hash = key.HashCode();
				this->SetNetworkID(hash);
				this->SetBool(Attr::IsMaster, false);
				ReplicationManager::Instance()->Reference(this);
			}
			else
			{			
				ReplicationManager::Instance()->Reference(this);
			}
		}		
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::OnDeactivate()
{
	if (this->IsCreator())
	{
		this->BroadcastDestruction();
	}	
	Game::Entity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::PostDeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection)
{
	BaseGameFeature::EntityManager::Instance()->AttachEntity(dynamic_cast<Game::Entity*>(this));
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkEntity::IsCreator()
{
	if (this->HasAttr(Attr::_LevelEntity) && this->GetBool(Attr::_LevelEntity))
	{
		// never deal with levelentities
		return false;
	}
	else
	{
		return (this->replicaManager && this->replicaManager->GetRakPeerInterface() && (this->creatingSystemGUID == this->replicaManager->GetRakPeerInterface()->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS)));
	}
}

//------------------------------------------------------------------------------
/**
*/
RakNet::RM3QuerySerializationResult
NetworkEntity::QuerySerialize(RakNet::Connection_RM3 *destinationConnection)
{
	RakNet::Replica3P2PMode mode = R3P2PM_SINGLE_OWNER;
	if (this->levelEntity)
	{
		if (NetworkServer::Instance()->IsHost())
		{
			mode = R3P2PM_STATIC_OBJECT_CURRENTLY_AUTHORITATIVE;
		}
		else
		{
			mode = R3P2PM_STATIC_OBJECT_NOT_CURRENTLY_AUTHORITATIVE;
		}
	}
	return QuerySerialization_PeerToPeer(destinationConnection, mode);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::SendNetwork(const Ptr<Messaging::Message> & message)
{
	this->queuedMessages.Append(message);
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkEntity::OnUserReplicaPreSerializeTick(void)
{
	if (this->queuedMessages.Size() > 0)
	{
		RakNet::BitStream bitstream;
		bitstream.Write(this->GetNetworkID());

		Ptr<BitWriter> writer = BitWriter::Create();
		writer->SetStream(&bitstream);

		int count = this->queuedMessages.Size();
		n_assert(count < 256);
		writer->WriteUChar(count);

		Ptr<IO::BinaryWriter> bwriter = IO::BinaryWriter::Create();
		bwriter->SetStream(writer.cast<IO::Stream>());
		bwriter->Open();

		for (int i = 0; i < count; i++)
		{
			const Ptr<Messaging::Message> & msg = this->queuedMessages[i];
			writer->WriteUInt(msg->GetClassFourCC().AsUInt());
			msg->Encode(bwriter);
		}
		this->queuedMessages.Clear();
		bwriter->Close();
		NetworkServer::Instance()->SendMessageStream(&bitstream);
	}
}

}

