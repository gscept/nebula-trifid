#pragma once
//------------------------------------------------------------------------------
/**
@class Multiplayer::SyncPoint

(C) 2015-2016 Individual contributors, see AUTHORS file
*/

#include "ReadyEvent.h"
#include "util/string.h"
#include "util/keyvaluepair.h"
#include "uniqueplayerid.h"
#include "util/delegate.h"

namespace MultiplayerFeature
{
	class NetworkServer;
}
//------------------------------------------------------------------------------
namespace Multiplayer
{
class SyncPoint : public RakNet::ReadyEvent
{

public:
	

	STATIC_FACTORY_DECLARATIONS(SyncPoint)

	/// registers a player to be tracked by the event
	static void AddToTracking(const Util::String & name, const Multiplayer::UniquePlayerId & id);
	/// removes a player from the tracking
	static void RemoveFromTracking(const Util::String & name, const Multiplayer::UniquePlayerId &id);
	/// remove sync point again
	static void Discard(const Util::String & name);

	/// does it refer to a real sync object
	static bool IsValid(const Util::String & name);

	/// set ready state
	static void SetReady(const Util::String & name, bool state);
	/// set ready state (local)
	static bool GetReady(const Util::String & name);

	/// Are all clients ready
	static bool AllReady(const Util::String & name);

	/// register callback 
	template<class CLASS, void (CLASS::*METHOD)(const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>&)> 
	static void SetCallback(const Util::String &name, CLASS* obj);

	/// register callback 
	template<class CLASS, void (CLASS::*METHOD)(bool)>
	static void SetAllCallback(const Util::String &name, CLASS* obj);


	///
	static void SetupSyncPoint(RakNet::ReadyEvent *ready);

protected:
	///
	virtual void OnEventChanged(unsigned eventid, unsigned char msg, RakNet::RakNetGUID sender);
	///
	static void ReadyEventChanged(IndexT name, const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>& val);	
	///
	static void AllReadyEvent(IndexT name);

private:
	friend class MultiplayerFeature::NetworkServer;
	
	static RakNet::ReadyEvent *readyEvent;

	static Util::Dictionary<IndexT, Util::Delegate<const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>&>> DelegateSetDict;
	static Util::Dictionary<IndexT, Util::Delegate<bool>> DelegateAllSetDict;
	///
	virtual void OnEventAllSet(unsigned eventid);
};

#define __SetSyncEventCallback(CLASS, METHOD, OBJ, NAME) Multiplayer::SyncPoint::SetCallback<CLASS, &CLASS::METHOD>(NAME, OBJ);
#define __SetSyncEventAllCallback(CLASS, METHOD, OBJ, NAME) Multiplayer::SyncPoint::SetAllCallback<CLASS, &CLASS::METHOD>(NAME, OBJ);

//------------------------------------------------------------------------------
/**
*/
template<class CLASS, void (CLASS::*METHOD)(const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>&)>
inline void 
SyncPoint::SetCallback(const Util::String &name, CLASS* obj)
{
	Util::Delegate<const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>& > del = Util::Delegate<const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>& >::FromMethod<CLASS, METHOD>(obj);
	DelegateSetDict.Add(name.HashCode(), del);
}

//------------------------------------------------------------------------------
/**
*/
template<class CLASS, void (CLASS::*METHOD)(bool)>
inline void
SyncPoint::SetAllCallback(const Util::String &name, CLASS* obj)
{
	Util::Delegate<bool> del = Util::Delegate<bool>::FromMethod<CLASS, METHOD>(obj);
	DelegateAllSetDict.Add(name.HashCode(), del);
}
}
