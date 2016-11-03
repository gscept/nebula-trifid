#pragma once
//------------------------------------------------------------------------------
/**
    Physics body stub

	(C) 2012-2016 Individual contributors, see AUTHORS file
*/
#if (__USE_BULLET__)
#include "bullet/bulletbody.h"
namespace Physics
{
class PhysicsBody : public Bullet::BulletBody
{
    __DeclareClass(PhysicsBody);     
};
}
#elif(__USE_PHYSX__)
#include "physx/physxbody.h"
namespace Physics
{
class PhysicsBody : public PhysX::PhysXBody
{
	__DeclareClass(PhysicsBody);	 
};
}
#elif(__USE_HAVOK__)	
#include "havok/havokbody.h"
namespace Physics
{
class PhysicsBody : public Havok::HavokBody
{
	__DeclareClass(PhysicsBody);	 
};
}
#else
#error "Physics::PhysicsBody not implemented"
#endif