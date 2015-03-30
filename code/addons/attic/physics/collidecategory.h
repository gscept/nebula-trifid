#ifndef PHYSICS_COLLIDECATEGORY_H
#define PHYSICS_COLLIDECATEGORY_H
//------------------------------------------------------------------------------
/**
    @enum Physics::CollideCategory

    Collide category bits allow to define collision relationships between
    collision shapes (which collide shapes should be tested for collision
    against other collide shapes). For instance, it makes no sense
    to check for collision between static environment objects.

    (C) 2003 RadonLabs
*/

//------------------------------------------------------------------------------
namespace Physics
{
enum CollideCategory
{
	None	= 0x00000000,
	Trigger = 0x00000008,
    Static	= (1<<0),
    Dynamic = (1<<1),

    All = 0xffffffff,
};

};
//------------------------------------------------------------------------------
#endif