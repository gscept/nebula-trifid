#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::PacketChannelBase

    ChannelIds for different channels where packets can be send through

	(C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
//------------------------------------------------------------------------------
#include "core/types.h"

namespace Base
{
class PacketChannelBase
{
public:
    enum Code
    {
        DefaultChannel = 0, // default channel
        ChatChannel,        // channel for sending chat messages
        MiscChannel         // channel for own ordering
    };
};

} // namespace Multiplayer
//-------------------