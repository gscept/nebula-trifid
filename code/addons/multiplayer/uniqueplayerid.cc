//------------------------------------------------------------------------------
//  uniqueplayerid.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "uniqueplayerid.h"

using namespace RakNet;

namespace Multiplayer
{
//------------------------------------------------------------------------------
/**
*/
UniquePlayerId::UniquePlayerId()
{

}

//------------------------------------------------------------------------------
/**
*/

UniquePlayerId::UniquePlayerId(const RakNetGUID& rakNetId)
{
    this->guid = rakNetId;
}

//------------------------------------------------------------------------------
/**
*/    
UniquePlayerId::~UniquePlayerId()
{

}

//------------------------------------------------------------------------------
/**
*/
bool 
operator==(const UniquePlayerId& a, const UniquePlayerId& b)
{
    return a.guid == b.guid;
}

//------------------------------------------------------------------------------
/**
*/
bool 
operator!=(const UniquePlayerId& a, const UniquePlayerId& b)
{
    return a.guid != b.guid;
}

//------------------------------------------------------------------------------
/**
*/
bool 
operator<(const UniquePlayerId& a, const UniquePlayerId& b)
{
    return a.guid < b.guid;
}

//------------------------------------------------------------------------------
/**
*/
bool 
operator>(const UniquePlayerId& a, const UniquePlayerId& b)
{
    return a.guid > b.guid;
}

} // namespace Multiplayer
