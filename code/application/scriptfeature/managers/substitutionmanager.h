#pragma once
//------------------------------------------------------------------------------
/**
    @class Script::SubstitutionManager
    
    replaces a text string token, depending on the current selection leader 

    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "core/singleton.h"

namespace Game
{
    class Entity;
}

//------------------------------------------------------------------------------
namespace Script
{
class SubstitutionManager : public Game::Manager
{
    __DeclareClass(SubstitutionManager);
    __DeclareSingleton(SubstitutionManager);

public:
    /// constructor
    SubstitutionManager();
    /// destructor
    virtual ~SubstitutionManager();
    /// get the substituted string by a given string and corresponding entity
    Util::String GetSubstitution(const Util::String& s, Ptr<Game::Entity> entity);
};

}; // namespace Script
//------------------------------------------------------------------------------
