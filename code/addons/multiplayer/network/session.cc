//------------------------------------------------------------------------------
//  session.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/session.h"

namespace Multiplayer
{
__ImplementClass(Multiplayer::Session, 'MPSE', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
Session::Session():
    started(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Session::~Session()
{
}

//------------------------------------------------------------------------------
/**
*/
void 
Session::AddPlayer(const Ptr<Multiplayer::Player>& player, bool publicSlot)
{
    n_assert(!this->joinedPlayers.Contains(player->GetUnqiueId()));
    player->OnJoinSession(this);
    player->SetIsInPublicSlot(publicSlot);
    this->joinedPlayers.Add(player->GetUnqiueId(), player);    

    // update our session info
    if (publicSlot)
    {
        ushort curOpenPublicSlots = this->sessionInfo.GetOpenPublicSlots();
        n_assert(curOpenPublicSlots > 0);
        this->sessionInfo.SetOpenPublicSlots(curOpenPublicSlots - 1);
        ushort curFilledPublicSlots = this->sessionInfo.GetFilledPublicSlots();
        this->sessionInfo.SetFilledPublicSlots(curFilledPublicSlots + 1);
    }
    else
    {
        ushort curOpenPrivateSlots = this->sessionInfo.GetOpenPrivateSlots();
        n_assert(curOpenPrivateSlots > 0);
        this->sessionInfo.SetOpenPrivateSlots(curOpenPrivateSlots - 1);
        ushort curFilledPrivateSlots = this->sessionInfo.GetFilledPrivateSlots();
        this->sessionInfo.SetFilledPrivateSlots(curFilledPrivateSlots + 1);
    }

    if (player->IsHost())
    {
        n_assert(!this->hostPlayer.isvalid());
        this->SetHostPlayer(player);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Session::RemovePlayer(const Ptr<Multiplayer::Player>& player)
{
    n_assert(this->joinedPlayers.Contains(player->GetUnqiueId()));
    player->OnLeaveSession();
    this->joinedPlayers.Erase(player->GetUnqiueId());

    // update our session info
    if (player->GetIsInPublicSlot())
    {
        ushort curFilledPublicSlots = this->sessionInfo.GetFilledPublicSlots();
        n_assert(curFilledPublicSlots > 0);
        this->sessionInfo.SetFilledPublicSlots(curFilledPublicSlots - 1);
        ushort curOpenPublicSlots = this->sessionInfo.GetOpenPublicSlots();
        this->sessionInfo.SetOpenPublicSlots(curOpenPublicSlots + 1);
    }
    else
    {
        ushort curFilledPrivateSlots = this->sessionInfo.GetFilledPrivateSlots();
        n_assert(curFilledPrivateSlots > 0);
        this->sessionInfo.SetFilledPrivateSlots(curFilledPrivateSlots - 1);
        ushort curOpenPrivateSlots = this->sessionInfo.GetOpenPrivateSlots();
        this->sessionInfo.SetOpenPrivateSlots(curOpenPrivateSlots + 1);
    }

    if (player->IsHost())
    {
        n_assert(this->hostPlayer.isvalid());
        this->SetHostPlayer(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Session::StartSession()
{
    n_assert(!this->started);
    this->started = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
Session::EndSession()
{
    n_assert(this->started);
    this->started = false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Player>& 
Session::GetPlayerByUniqueId(const UniquePlayerId& id) const
{
    n_assert(this->joinedPlayers.Contains(id));
    return this->joinedPlayers[id];
}

//------------------------------------------------------------------------------
/**
*/
void 
Session::ClearSession()
{
    Util::Array<Ptr<Player> > players = this->joinedPlayers.ValuesAsArray();
    IndexT i;
    for (i = 0; i < players.Size(); ++i)
    {
    	this->RemovePlayer(players[i]);
    }        
}
} // namespace Multiplayer
