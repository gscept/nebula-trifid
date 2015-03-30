#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::RakNetPacketPriority

    Conversion between nebula3 and platform type.

	(C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "network/base/packetprioritybase.h"
#include "raknet/PacketPriority.h"

//------------------------------------------------------------------------------
namespace RakNet
{
class RakNetPacketPriority : public Base::PacketPriorityBase
{
public:
    /// conversion from platform type to nebula3 type
    static Base::PacketPriorityBase::Code AsNebula3Priority(PacketPriority raknetPriority);
    /// conversion from nebula3 to platform type
    static PacketPriority AsRaknetPriority(Base::PacketPriorityBase::Code nebulaPriority);

};

} // namespace Multiplayer
//-------------------