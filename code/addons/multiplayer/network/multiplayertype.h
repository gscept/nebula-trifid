#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::SeesionType

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"

//------------------------------------------------------------------------------
namespace Multiplayer
{
class MultiplayerType
{
public:
    /// index types enum
    enum Code
    {
        CoachCoop,          // multi player on same system 
        LocalNetwork,       // multi player on a local network or system link
        OnlineStandard,     // multi player over internet 
        OnlineRanked,       // multi player over internet with special constraints for competition play

        NumMultiplayerTypes,
    };
};
}
//------------------------------------------------------------------------------
