//------------------------------------------------------------------------------
//  managers/story/substitutionmanager.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/managers/substitutionmanager.h"
#include "game/entity.h"

namespace Script
{
__ImplementClass(SubstitutionManager, 'SUBM', Game::Manager);
__ImplementSingleton(SubstitutionManager);

//------------------------------------------------------------------------------
/**
*/
SubstitutionManager::SubstitutionManager()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
SubstitutionManager::~SubstitutionManager()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    override to get localized string by token and entity
*/
Util::String 
SubstitutionManager::GetSubstitution(const Util::String& s, Ptr<Game::Entity> entity)
{
    return "<invalid token>";
}

}; // namespace Managers
