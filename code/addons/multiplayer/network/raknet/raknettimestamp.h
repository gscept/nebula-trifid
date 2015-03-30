#pragma once
//------------------------------------------------------------------------------
/**
@class RakNet::RakNetTimeStamp

    RakNet will adjust the timestamp of every packet with a ID_TIMESTAMP identifier.
    It automatically computes the delta time the msg needs travelling over the network.
    To get accurate timestamps call SetOccasionalPing(true) on rakpeerinterface.   

	(C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "network/bitwriter.h"
#include "network/bitreader.h"

namespace RakNet
{

class RakNetTimeStamp
{
public:
    /// write time message id and time stamp, raknet will adjust the time correctly
    static void WriteTimeStamp(const Ptr<InternalMultiplayer::BitWriter>& writer, Timing::Tick time);	 
    /// write a flag that no timestamp will be used, on raknet this will write nothing
    static void WriteNoTimeStampFlag(const Ptr<InternalMultiplayer::BitWriter>& writer);
    /// check stream if any send duration was written
    static bool HasRelativeTimeStamp(const Ptr<InternalMultiplayer::BitReader>& reader);	 
    /// read timestamp from stream, this is automatically adjusted by raknet to synchroize the timestamp
    static Timing::Tick ReadRelativeTimeStamp(const Ptr<InternalMultiplayer::BitReader>& reader);	     
};
}