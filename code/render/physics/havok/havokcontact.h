#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokContact
    
    A contact during a collision, this contains havok-specific data.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "physics/base/basecontact.h"

#include <Common/Base/hkBase.h>
#include <Physics/Dynamics/Collide/ContactListener/hkpContactPointEvent.h>
//------------------------------------------------------------------------------
namespace Havok
{
class HavokContact : public Physics::BaseContact
{
__DeclareClass(HavokContact);
public:
	/// constructor
	HavokContact();
	/// destructor
	virtual ~HavokContact();

	/// set separating velocity
	void SetSeparatingVelocity(float vel);
	/// get separating velocity
	float GetSeparatingVelocity() const;

	/// set the contact event type
	void SetContactEventType(const hkpContactPointEvent::Type& type);
	/// get the contact event type
	hkpContactPointEvent::Type GetContactEventType() const;

	/// calculate and return the impulse applied on the owner object (separating velocity is used)
	float CalculateImpulse();

protected:

	float separatingVelocity;	//< this will differ depending on the contact event type
	hkpContactPointEvent::Type contactEventType;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokContact::SetSeparatingVelocity(float vel)
{
	this->separatingVelocity = vel;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokContact::GetSeparatingVelocity() const
{
	return this->separatingVelocity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokContact::SetContactEventType(const hkpContactPointEvent::Type& type)
{
	this->contactEventType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline hkpContactPointEvent::Type 
HavokContact::GetContactEventType() const
{
	return this->contactEventType;
}

} 
// namespace Havok
//------------------------------------------------------------------------------