//------------------------------------------------------------------------------
//  basestatic.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "collider.h"
#include "base/basestatic.h"

namespace Physics
{
__ImplementAbstractClass(Physics::BaseStatic, 'PBBS', Physics::PhysicsObject);

//------------------------------------------------------------------------------
/**
*/
void 
BaseStatic::SetupFromTemplate(const PhysicsCommon & templ)
{
	PhysicsObject::SetupFromTemplate(templ);	
	this->transform = templ.startTransform;	
}
}