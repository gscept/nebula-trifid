//------------------------------------------------------------------------------
//  RakNetuniqueplayerid.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/raknet/raknetuniqueplayerid.h"

namespace RakNet
{
//------------------------------------------------------------------------------
/**
*/
RakNetUniquePlayerId::RakNetUniquePlayerId()
{

}

//------------------------------------------------------------------------------
/**
*/

RakNetUniquePlayerId::RakNetUniquePlayerId(const RakNetGUID& rakNetId)
{
    this->guid = rakNetId;
}

//------------------------------------------------------------------------------
/**
*/    
RakNetUniquePlayerId::~RakNetUniquePlayerId()
{

}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetUniquePlayerId::Encode(const Ptr<Multiplayer::BitWriter>& writer) const
{
    IndexT i;
    for (i = 0; i < 6; ++i)
    {
        writer->WriteUInt(this->guid.g[i]);	
    }       
}

//------------------------------------------------------------------------------
/**
*/
void 
RakNetUniquePlayerId::Decode(const Ptr<Multiplayer::BitReader>& reader)
{    
    IndexT i;
    for (i = 0; i < 6; ++i)
    {
        this->guid.g[i] = reader->ReadUInt();	
    }
}

//------------------------------------------------------------------------------
/**
*/
Util::Blob 
RakNetUniquePlayerId::AsBlob() const
{
    Util::Blob blob(this->guid.g, sizeof(this->guid.g));
    return blob;
}

//------------------------------------------------------------------------------
/**
*/
bool 
operator==(const RakNetUniquePlayerId& a, const RakNetUniquePlayerId& b)
{
    return a.guid == b.guid;
}

//------------------------------------------------------------------------------
/**
*/
bool 
operator!=(const RakNetUniquePlayerId& a, const RakNetUniquePlayerId& b)
{
    return a.guid != b.guid;
}

//------------------------------------------------------------------------------
/**
*/
bool 
operator<(const RakNetUniquePlayerId& a, const RakNetUniquePlayerId& b)
{
    return a.guid < b.guid;
}

//------------------------------------------------------------------------------
/**
*/
bool 
operator>(const RakNetUniquePlayerId& a, const RakNetUniquePlayerId& b)
{
    return a.guid > b.guid;
}

} // namespace Multiplayer
