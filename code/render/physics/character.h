#pragma once
//------------------------------------------------------------------------------
/**
	Physics::Character

    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#if (__USE_BULLET__)
#include "physics/bullet/bulletcharacter.h"
namespace Physics
{
class Character : public Bullet::BulletCharacter
{
	__DeclareClass(Character);
};
}
#elif (__USE_HAVOK__)	
#include "physics/havok/havokcharacterphantom.h"
#include "physics/havok/havokcharacterrigidbody.h"
namespace Physics
{
class Character : public Havok::HavokCharacterRigidBody
{
	__DeclareClass(Character);
};
}
#else
#error "Character is not implemented on this platform!"
#endif