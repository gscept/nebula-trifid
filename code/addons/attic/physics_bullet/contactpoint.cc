//------------------------------------------------------------------------------
//  physics/contactpoint.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "Physics/physicsserver.h"
#include "Physics/contactpoint.h"
#include "Physics/rigidbody.h"

namespace Physics
{
__ImplementClass(Physics::ContactPoint, 'CTPT', Core::RefCounted);
//------------------------------------------------------------------------------
/**
    Returns pointer to rigid body of contact point. 
*/
RigidBody*
ContactPoint::GetRigidBody() const
{
    if(rigidBody.isvalid())
		return rigidBody.get_unsafe();
	
    return 0;    
}

const Util::Array<Ptr<ContactPoint>> & ContactPoint::GetAllContacts()
{
	return this->otherContacts;
}
void ContactPoint::AddContact(Ptr<ContactPoint> contacts)
{
	this->otherContacts.Append(contacts);
}

}