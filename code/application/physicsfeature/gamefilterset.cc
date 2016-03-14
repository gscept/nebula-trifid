//------------------------------------------------------------------------------
//  physicsfeature/gamefilterset.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "collider.h"
#include "physicsfeature/gamefilterset.h"
#include "game/entity.h"
#include "physicsobject.h"


using namespace Physics;

namespace PhysicsFeature
{
//------------------------------------------------------------------------------
/**
*/
bool
GameFilterSet::CheckObject(const PhysicsObject * object) const
{
    n_assert(object != NULL);
	if(object->GetUserData()->object.isvalid() && object->GetUserData()->object->IsA(Game::Entity::RTTI.GetFourCC()))
	{
		if(this->CheckEntityId(object->GetUserData()->object.cast<Game::Entity>()->GetUniqueId()))
		{
			return true;
		}
	}
	return FilterSet::CheckObject(object);	
}

};
