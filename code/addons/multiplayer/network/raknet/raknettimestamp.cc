//------------------------------------------------------------------------------
//  RakNetTimeStamp.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/raknet/raknettimestamp.h"
#include "network/packetid.h"
#include "network/netstream.h"
#include "raknet/RakNetTypes.h"

namespace RakNet
{

using namespace Multiplayer;
//------------------------------------------------------------------------------
/**
*/
void 
RakNetTimeStamp::WriteTimeStamp(const Ptr<Multiplayer::BitWriter>& writer, Timing::Tick time)
{
    // OccasionalPing have to be on in raknet for accurate timestamp sync
    // Message id for timestamp must come first before anything else
    MessageID timeStampId = ID_TIMESTAMP;
    writer->WriteRawBitData((const unsigned char*)&timeStampId, sizeof(PacketId::PacketIdType)*8);    
    writer->WriteRawBitData((const unsigned char*)&time, sizeof(Timing::Tick)*8);  
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetTimeStamp::WriteNoTimeStampFlag(const Ptr<Multiplayer::BitWriter>& writer)
{
    // just write nothing
}

//------------------------------------------------------------------------------
/**
*/
bool 
RakNetTimeStamp::HasRelativeTimeStamp(const Ptr<Multiplayer::BitReader>& reader)
{
    // Message id for timestamp must come first
    // check first byte without incrementing read ptr, so get directly access to bitstream data
    const Ptr<Multiplayer::NetStream>& netstream = reader->GetStream().cast<Multiplayer::NetStream>();
    BitStream* bitstream = netstream->GetBitstream();
    unsigned char* data = bitstream->GetData(); 

    return (data[0] == ID_TIMESTAMP);
}

//------------------------------------------------------------------------------
/**
*/
Timing::Tick 
RakNetTimeStamp::ReadRelativeTimeStamp(const Ptr<Multiplayer::BitReader>& reader)
{
    // check first byte, and consume it
    MessageID idCode = reader->ReadChar();
    n_assert(idCode == ID_TIMESTAMP);

    Timing::Tick timeStamp;
    return reader->ReadRawBitData((unsigned char*)&timeStamp, sizeof(Timing::Tick) * 8);
}
} // end RakNet