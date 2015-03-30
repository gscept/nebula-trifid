//------------------------------------------------------------------------------
//  havokcontact.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokcontact.h"
#include "physics/physicsobject.h"
#include "havokutil.h"
#include "Physics/Dynamics/Entity/hkpRigidBody.h"

namespace Havok
{
__ImplementClass(Havok::HavokContact,'HKCT', Physics::BaseContact);

//------------------------------------------------------------------------------
/**
*/
HavokContact::HavokContact()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
HavokContact::~HavokContact()
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Calculation of the impulse is taken from the Intel forum post
	http://software.intel.com/en-us/forums/topic/279590.

	Important note: Impulse is not relative to mass, so the impulse created from
	a collision of two objects with mass x and y will be no different if the
	masses for example are changed to x*100 and y*100
*/
float 
HavokContact::CalculateImpulse()
{
	// impulseAppliedToBodyA = separatingVelocity * (1 + (resA * resB)) * massB / (massA + massB)
	hkRefPtr<hkpRigidBody> objA = HavokUtil::GetHavokRigidBody(this->ownerObject);
	hkRefPtr<hkpRigidBody> objB = HavokUtil::GetHavokRigidBody(this->object);

	float restitutionA = objA->getRestitution();
	float restitutionB = objB->getRestitution();

	float impulse = this->separatingVelocity * (1 + restitutionA * restitutionB);

	if (hkpMotion::MOTION_FIXED != objB->getMotionType() && hkpMotion::MOTION_KEYFRAMED != objB->getMotionType())
	{
		// if the other body is not static or keyframed, then use masses
		float massA = objA->getMass();
		float massB = objB->getMass();

		impulse *= massB / (massA + massB);
	}

	return impulse;
}

}