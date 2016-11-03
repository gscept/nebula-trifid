#pragma once
//------------------------------------------------------------------------------
/**
    Physics static object stub

    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
#if (__USE_BULLET__)
#include "bullet/bulletstatic.h"
namespace Physics
{
class StaticObject : public Bullet::BulletStatic
{
    __DeclareClass(StaticObject);     
};
}
#elif(__USE_PHYSX__)
#include "physx/physxstatic.h"
namespace Physics
{
class StaticObject : public PhysX::PhysXStatic
{
	__DeclareClass(StaticObject);	 
};
}
#elif(__USE_HAVOK__)
#include "havok/havokstatic.h"
namespace Physics
{
class StaticObject : public Havok::HavokStatic
{
	__DeclareClass(StaticObject);	 
};
}
#else
#error "Physics::StaticObject not implemented"
#endif