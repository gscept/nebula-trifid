//------------------------------------------------------------------------------
//  syncpoint.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ReadyEvent.h"
#include "RakNetTypes.h"
#include "syncpoint.h"
#include "MessageIdentifiers.h"


using namespace RakNet;

RakNet::ReadyEvent * Multiplayer::SyncPoint::readyEvent = NULL;
Util::Dictionary<IndexT, Util::Delegate<const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>&>> Multiplayer::SyncPoint::DelegateSetDict;
Util::Dictionary<IndexT, Util::Delegate<bool>> Multiplayer::SyncPoint::DelegateAllSetDict;


namespace Multiplayer
{
	STATIC_FACTORY_DEFINITIONS(SyncPoint, SyncPoint);

//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::AddToTracking(const Util::String & name, const Multiplayer::UniquePlayerId & id)
{	
	IndexT eventId = name.HashCode();
	SyncPoint::readyEvent->AddToWaitList(eventId, id.GetRaknetGuid());	
}


//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::RemoveFromTracking(const Util::String & name, const Multiplayer::UniquePlayerId &id)
{
	IndexT eventId = name.HashCode();
	SyncPoint::readyEvent->RemoveFromWaitList(eventId, id.GetRaknetGuid());
}

//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::Discard(const Util::String & name)
{
	if (SyncPoint::IsValid(name))
	{
		IndexT eventId = name.HashCode();
		SyncPoint::readyEvent->AddToWaitList(eventId, UNASSIGNED_RAKNET_GUID);
		SyncPoint::readyEvent->DeleteEvent(eventId);
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncPoint::IsValid(const Util::String & name)
{
	return SyncPoint::readyEvent->HasEvent(name.HashCode());
}

//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::SetReady(const Util::String & name, bool state)
{
	SyncPoint::readyEvent->SetEvent(name.HashCode(), state);
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncPoint::GetReady(const Util::String & name)
{
	return SyncPoint::readyEvent->IsEventSet(name.HashCode());
}

//------------------------------------------------------------------------------
/**
*/
bool
SyncPoint::AllReady(const Util::String & name)
{
	return SyncPoint::readyEvent->IsEventCompleted(name.HashCode());
}

//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::SetupSyncPoint(RakNet::ReadyEvent *ready)
{
	SyncPoint::readyEvent = ready;
}

//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::ReadyEventChanged(IndexT name, const Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>& val)
{
	if (SyncPoint::DelegateSetDict.Contains(name))
	{
		SyncPoint::DelegateSetDict[name](val);
	}	
}

//------------------------------------------------------------------------------
/**
	bool is a dummy value 
*/
void
SyncPoint::AllReadyEvent(IndexT name)
{
	if (SyncPoint::DelegateAllSetDict.Contains(name))
	{
		SyncPoint::DelegateAllSetDict[name](true);
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::OnEventChanged(unsigned eventid, unsigned char msg, RakNet::RakNetGUID sender)
{
	n_printf("event id: %d, packet: %d\n", eventid, msg);
	switch (msg)
	{
	case ID_READY_EVENT_SET:
		SyncPoint::ReadyEventChanged(eventid, Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>(UniquePlayerId(sender), true));
		break;
	case ID_READY_EVENT_UNSET:
		SyncPoint::ReadyEventChanged(eventid, Util::KeyValuePair<Multiplayer::UniquePlayerId, bool>(UniquePlayerId(sender), false));
		break;
	case ID_READY_EVENT_ALL_SET:
		
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
SyncPoint::OnEventAllSet(unsigned eventid)
{
	SyncPoint::AllReadyEvent(eventid);
}

}