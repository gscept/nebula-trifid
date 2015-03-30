//------------------------------------------------------------------------------
//  havokutil.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokutil.h"
#include "..\physicsobject.h"
#include "havokbody.h"
#include "havokcharacterrigidbody.h"
#include "havokstatic.h"

namespace Havok
{
	
//------------------------------------------------------------------------------
/**
*/
void 
HavokUtil::CheckWorldObjectUserData(hkUlong userData)
{
	n_assert2(0 != userData, "Userdata for havok entity is zero! When creating the entity its userdata must be set to the nebula-physobject that owns it");

	Ptr<Physics::PhysicsObject> physObject = (Physics::PhysicsObject*)userData;
	n_assert(physObject.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokUtil::HasHavokRigidBody(const Ptr<Physics::PhysicsObject>& obj)
{
	return obj->IsA(HavokBody::RTTI) ||
		obj->IsA(HavokCharacterRigidBody::RTTI) ||
		obj->IsA(HavokStatic::RTTI);
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpRigidBody> 
HavokUtil::GetHavokRigidBody(const Ptr<Physics::PhysicsObject>& obj)
{
	if (obj->IsA(HavokBody::RTTI))
	{
		return ((HavokBody*)obj.get())->GetRigidBody();
	}
	else if (obj->IsA(HavokCharacterRigidBody::RTTI))
	{
		return ((HavokCharacterRigidBody*)obj.get())->GetRigidBody();
	}
	//FIXME: check if works with phantoms - then this needs restructuring since phantoms use no hkpRigidBody
	//else if (object->IsA(HavokCharacterPhantom::RTTI))
	//{
	//	rigidBody = ((HavokCharacterPhantom*)object.get())->GetRigidBody();
	//}
	else if (obj->IsA(HavokStatic::RTTI))
	{
		return ((HavokStatic*)obj.get())->GetRigidBody();
	}
	else
	{
		n_error("HavokUtil::GetHavokRigidBody: The given physicsobject is not recognized to contain a hkpRigidBody!");
	}

	return 0;
}

}