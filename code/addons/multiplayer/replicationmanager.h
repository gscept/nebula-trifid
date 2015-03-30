#pragma once
//------------------------------------------------------------------------------
/**
@class MultiplayerFeature::ReplicationManager

(C) 2015 Individual contributors, see AUTHORS file
*/
#include "util/dictionary.h"
#include "timing/time.h"
#include "core/refcounted.h"
#include "core/singleton.h"
#include "ReplicaManager3.h"

namespace RakNet
{
	class ReplicaManager3;
	class NetworkIDManager;
};

//------------------------------------------------------------------------------
namespace MultiplayerFeature
{
class ReplicationManager : public RakNet::ReplicaManager3, public Core::RefCounted
{
	__DeclareClass(ReplicationManager);
	__DeclareSingleton(ReplicationManager);
public:
	/// constructor
	ReplicationManager();
	/// destructor
	~ReplicationManager();

	/// 
	void Open();
	///
	void Close();
	/// create the Connection_RM3 object that will act as a factory for objects
	virtual RakNet::Connection_RM3* AllocConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID) const;
	/// remove a connection
	virtual void DeallocConnection(RakNet::Connection_RM3 *connection) const;
private:
	
};

}// namespace MultiplayerFeature
//------------------------------------------------------------------------------
