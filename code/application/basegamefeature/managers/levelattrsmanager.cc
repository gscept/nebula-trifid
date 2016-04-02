//------------------------------------------------------------------------------
//  levelattrsmanager.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "managers/levelattrsmanager.h"
#include "basegamefeatureunit.h"
#include "dbserver.h"

using namespace Db;
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
        if (!this->table.isvalid())
        {
            const Ptr<Db::Database>& db = Db::DbServer::Instance()->GetGameDatabase();
            Ptr<Dataset> dataset = db->GetTableByName("_Instance_Levels")->CreateDataset();
            dataset->AddAllTableColumns();
            dataset->PerformQuery();
            this->table = dataset->Values();
        }
		this->row = table->FindRowIndexByAttr(Attr::Attribute(Attr::Id, level));
		n_assert(this->row != InvalidIndex);
		this->currentLevel = level;
	}	
}
}