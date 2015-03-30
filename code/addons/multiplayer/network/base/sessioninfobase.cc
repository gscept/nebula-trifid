//------------------------------------------------------------------------------
//  playerbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/base/sessioninfobase.h"

namespace Base
{
//------------------------------------------------------------------------------
/**
*/
SessionInfoBase::SessionInfoBase():
    openPublicSlots(0),
    openPrivateSlots(0),
    filledPublicSlots(0),
    filledPrivateSlots(0),
    curSessionType(Multiplayer::MultiplayerType::CoachCoop)
{

}

//------------------------------------------------------------------------------
/**
*/
Util::String 
SessionInfoBase::GetIpAddressaAsString() const
{
    n_error("SessionInfoBase::GetIpAddressaAsString called!");

    return Util::String();
}
} // namespace Multiplayer
