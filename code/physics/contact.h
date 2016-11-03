#pragma once
//------------------------------------------------------------------------------
/**
    Contact stub

	(C) 2013-2016 Individual contributors, see AUTHORS file
*/
#if (__USE_BULLET__)
#include "base/basecontact.h"
namespace Physics
{
class Contact : public Physics::BaseContact
{
	__DeclareClass(Contact);     
};
}
#elif(__USE_PHYSX__)
#include "base/basecontact.h"
namespace Physics
{
class Contact : public Physics::BaseContact
{
	__DeclareClass(Contact);     
};
}
#elif(__USE_HAVOK__)	
#include "havok/havokcontact.h"
namespace Physics
{
class Contact : public Havok::HavokContact
{
	__DeclareClass(Contact);     
};
}
#else
#error "Physics::Contact not implemented"
#endif