#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::PlayerInfo

    This struct is just used for messages at the moment.

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "util/string.h"
#include "network/handle.h"
#include "network/base/playerbase.h"

namespace Multiplayer
{
struct PlayerInfo 
{   
    Util::StringAtom gamerTag;
    PlayerHandle playerHandle;
    Base::PlayerBase::SignInState signInState;
};
} // namespace Multiplayer
//-------------------