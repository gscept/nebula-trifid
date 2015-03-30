#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokUtil
    
    Contains static utility methods
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include <Common/Base/Types/hkBaseTypes.h>
#include <Common/Base/hkBase.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include "core/ptr.h"
//------------------------------------------------------------------------------

namespace Physics
{
	class PhysicsObject;
}

namespace Havok
{
class HavokUtil
{
public:
	/// asserts that the userdata is a valid physicsobject pointer
	static void CheckWorldObjectUserData(hkUlong userData);
	/// returns wether the given physobject is one that contains an hkRigidBody
	static bool HasHavokRigidBody(const Ptr<Physics::PhysicsObject>& obj);
	/// retrieve the hkRigidBody, throws an error if there is none
	static hkRefPtr<hkpRigidBody> GetHavokRigidBody(const Ptr<Physics::PhysicsObject>& obj);
}; 

} 
// namespace Havok
//------------------------------------------------------------------------------