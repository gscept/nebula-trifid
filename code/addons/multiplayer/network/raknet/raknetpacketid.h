#pragma once
//------------------------------------------------------------------------------
/**
    @class RakNet::RakNetPackedId
        
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "raknet/MessageIdentifiers.h"

namespace RakNet
{
#define PACKET_DATA_BYTEOFFSET  1

class RakNetPackedId
{

public:
    // used for writing id into stream with correct byte size
    typedef unsigned char PacketIdType;

    enum PacketIdCode
    {
        NebulaPlayerinfo = ID_USER_PACKET_ENUM,        
        NebulaStartGame,
        NebulaClientStartgameFinished,
        NebulaAllStartgameFinished,
        NebulaEndgame,  
        NebulaPlayerReadynessChanged,    
        NebulaMessage,
        NebulaMigrateHost,
        NebulaNewHost,
		NebulaChatMessage,
        
        NumNebulaPacketIds,
        InvalidNebulaId,
		NebulaClientEndgameFinished,
		NebulaAllEndgameFinished
    };

};
} // RakNet