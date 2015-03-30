//------------------------------------------------------------------------------
//  physics/filterset.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/filterset.h"
#include "physics/shape.h"
#include "physics/physicsentity.h"
#include "physics/rigidbody.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

namespace Physics
{
//------------------------------------------------------------------------------
/**
*/
bool
FilterSet::CheckShape(Shape* shape) const
{
    n_assert(shape);
    if (0 == (this->collideBits & shape->GetCollideCategory()))
    {
        return true;
    }    	
    return false;
}

bool
FilterSet::CheckBody(RigidBody* shape) const
{
	n_assert(shape);
	if (0 == (this->collideBits & shape->GetCollideCategory()))
	{
		return true;
	}    	
	return false;
}

};
