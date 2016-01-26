//------------------------------------------------------------------------------
//  levelattrsmanager.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/levelattrsmanager.h"
#include "categorymanager.h"
#include "basegamefeatureunit.h"

namespace BaseGameFeature
{
__ImplementClass(LevelAttrsManager, 'LATM', Game::Manager);
__ImplementSingleton(LevelAttrsManager);

//------------------------------------------------------------------------------
/**
*/
LevelAttrsManager::LevelAttrsManager()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
LevelAttrsManager::~LevelAttrsManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
LevelAttrsManager::OnLoad()
{
    Manager::OnLoad();
	Util::String level = BaseGameFeature::BaseGameFeatureUnit::Instance()->GetCurrentLevel();
	this->table = BaseGameFeature::CategoryManager::Instance()->GetTemplateTable("Levels");
	this->row = table->FindRowIndexByAttr(Attr::Attribute(Attr::Id, level));
	n_assert(this->row != InvalidIndex);
}
}