//------------------------------------------------------------------------------
//  network/multiplayer/session.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "multiplayer/session.h"

namespace Multiplayer
{
__ImplementClass(Session, 'MPSE', Core::RefCounted);

using namespace InternalMultiplayer;

//------------------------------------------------------------------------------
/**
*/
Session::Session() : privateSlots(InvalidIndex),
                     publicSlots(InvalidIndex),
                     type(InternalMultiplayer::MultiplayerType::CoachCoop)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Session::~Session()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Session::RemovePlayer(const InternalMultiplayer::PlayerHandle& handle)
{
    // first check public player
    IndexT index;
    for (index = 0; index < this->publicPlayers.Size(); index++)
    {
        if (this->publicPlayers[index]->GetPlayerHandle() == handle)
        {
            this->publicPlayers.EraseIndex(index);
            return;
        }
    }

    for (index = 0; index < this->privatePlayers.Size(); index++)
    {
        if (this->privatePlayers[index]->GetPlayerHandle() == handle)
        {
            this->privatePlayers.EraseIndex(index);
            return;
        }
    }

    n_error("Session::RemovePlayer() -> where do you have this handle from?! its invalid!");
}

//------------------------------------------------------------------------------
/**
*/
void 
Session::ClearSession()
{
    this->privatePlayers.Clear();
    this->publicPlayers.Clear();
}
} // namespace Multiplayer
