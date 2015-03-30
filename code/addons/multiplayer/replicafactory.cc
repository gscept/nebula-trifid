//------------------------------------------------------------------------------
//  replicafactory.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "replicafactory.h"
#include "networkentity.h"
#include "managers/entitymanager.h"
#include "bitreader.h"
#include "networkfactorymanager.h"

using namespace Multiplayer;

namespace MultiplayerFeature
{
	
//------------------------------------------------------------------------------
/**
*/
MultiplayerFeature::ReplicaFactory::ReplicaFactory(const RakNet::SystemAddress &_systemAddress, RakNet::RakNetGUID _guid) : 
	RakNet::Connection_RM3(_systemAddress, _guid)
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/

MultiplayerFeature::ReplicaFactory::~ReplicaFactory()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RakNet::Replica3 *
MultiplayerFeature::ReplicaFactory::AllocReplica(RakNet::BitStream *allocationIdBitstream, RakNet::ReplicaManager3 *replicaManager3)
{
	// Types are written by WriteAllocationID()
	uint id;
	allocationIdBitstream->Read(id);
	Util::FourCC fourcc(id);
	if (fourcc == NetworkEntity::RTTI.GetFourCC())
	{
		Ptr<BitReader> reader = BitReader::Create();
		reader->SetStream(allocationIdBitstream);
		Util::String category = reader->ReadString();
		Ptr<Game::Entity> entity = NetworkFactoryManager::Instance()->CreateEmptyEntity(category);
		entity->AddRef();
		return dynamic_cast<RakNet::Replica3*>(entity.get_unsafe());
	}
	else
	{
		Core::RefCounted * object = Core::Factory::Instance()->Create(fourcc);
		return dynamic_cast<RakNet::Replica3*>(object);
	}		
}
}