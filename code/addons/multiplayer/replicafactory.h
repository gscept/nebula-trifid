#pragma once
//------------------------------------------------------------------------------
/**
@class MultiplayerFeature::ReplicaFactory

(C) 2015 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "ReplicaManager3.h"

namespace RakNet
{
	class ReplicaManager3;
	class NetworkIDManager;
};

//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class ReplicaFactory : public RakNet::Connection_RM3
{	
public:
	// constructor
	ReplicaFactory(const RakNet::SystemAddress &_systemAddress, RakNet::RakNetGUID _guid);
	// destructor
	virtual ~ReplicaFactory();
	// Create a replica object by the information in the bitstream
	virtual RakNet::Replica3 *AllocReplica(RakNet::BitStream *allocationIdBitstream, RakNet::ReplicaManager3 *replicaManager3);

};

}// namespace MultiplayerFeature
//------------------------------------------------------------------------------

