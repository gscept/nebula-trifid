//------------------------------------------------------------------------------
//  replicationmanager.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "util/string.h"
#include "util/array.h"
#include "replicationmanager.h"
#include "multiplayerfeatureunit.h"
#include "RakNetSocket2.h"
#include "replicafactory.h"


using namespace Math;

namespace MultiplayerFeature
{
__ImplementClass(MultiplayerFeature::ReplicationManager, 'RRPM', Core::RefCounted);
__ImplementSingleton(MultiplayerFeature::ReplicationManager);




//------------------------------------------------------------------------------
/**
*/
ReplicationManager::ReplicationManager() 
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ReplicationManager::~ReplicationManager()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
ReplicationManager::Open()
{


}

//------------------------------------------------------------------------------
/**
*/
RakNet::Connection_RM3*
ReplicationManager::AllocConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID) const
{
	return new ReplicaFactory(systemAddress, rakNetGUID);
}

//------------------------------------------------------------------------------
/**
*/
void
ReplicationManager::DeallocConnection(RakNet::Connection_RM3 *connection) const
{
	delete connection;
}
}