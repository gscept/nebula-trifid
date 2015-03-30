#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::UniquePlayerId

    A UniquePlayerId identifies each player over the whole network.
	
    (C) 2015 Individual contributors, see AUTHORS file
*/    

#include "RakNetTypes.h"

//------------------------------------------------------------------------------
namespace Multiplayer
{
class UniquePlayerId 
{
public:
    /// constructor
	UniquePlayerId();
    /// constructor
	UniquePlayerId(const RakNet::RakNetGUID& rakNetId);
    /// destructor
	virtual ~UniquePlayerId();
    /// equality operator
	friend bool operator==(const UniquePlayerId& a, const UniquePlayerId& b);
    /// unequality operator
	friend bool operator!=(const UniquePlayerId& a, const UniquePlayerId& b);
    /// less operator
	friend bool operator<(const UniquePlayerId& a, const UniquePlayerId& b);
    /// greater operator
	friend bool operator>(const UniquePlayerId& a, const UniquePlayerId& b);
    /// get Guid	
	const RakNet::RakNetGUID& GetRaknetGuid() const;
    /// set Guid
	void SetRaknetGuid(const RakNet::RakNetGUID& val);

protected:
    RakNet::RakNetGUID  guid;
};
//------------------------------------------------------------------------------
/**
*/
inline const RakNet::RakNetGUID&
UniquePlayerId::GetRaknetGuid() const
{
    return this->guid;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
UniquePlayerId::SetRaknetGuid(const RakNet::RakNetGUID& val)
{
    this->guid = val;
}
} // namespace Multiplayer
//-------------------