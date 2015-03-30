//------------------------------------------------------------------------------
//  physicsfeatyre/gamefilterset.cc
//  (C) 2012 Johannes Hirche
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsfeature/gamefilterset.h"
#include "game/entity.h"

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
