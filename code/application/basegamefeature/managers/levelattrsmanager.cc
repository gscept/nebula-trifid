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
LevelAttrsManager::LevelAttrsManager() :
    row(InvalidIndex)
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
LevelAttrsManager::UpdateLevelTable(const Util::String& level)
{			
	if (this->currentLevel != level)
	{
		this->table = BaseGameFeature::CategoryManager::Instance()->GetTemplateTable("Levels");
		this->row = table->FindRowIndexByAttr(Attr::Attribute(Attr::Id, level));
		n_assert(this->row != InvalidIndex);
		this->currentLevel = level;
	}	
}
}