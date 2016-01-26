//------------------------------------------------------------------------------
//  loader/levelloader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loader/levelloader.h"
#include "loader/loaderserver.h"
#include "appgame/appconfig.h"
#include "core/factory.h"
#include "db/dbserver.h"
#include "db/reader.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "loader/entityloader.h"
#include "game/gameserver.h"
#include "loader/loaderserver.h"
#include "managers/categorymanager.h"
#include "math/float4.h"

using namespace Math;
namespace BaseGameFeature
{

//------------------------------------------------------------------------------
/**
*/
bool
LevelLoader::Load(const Util::String& levelName, const Util::Array<Util::String>& activeLayers)
{
    // update progress bar window
    BaseGameFeature::LoaderServer* loaderServer = BaseGameFeature::LoaderServer::Instance();
    loaderServer->SetProgressText("Query Database...");
    loaderServer->UpdateProgressDisplay(); 
    
    // query level instance attributes from database
    Ptr<Db::Reader> dbReader = Db::Reader::Create();
    dbReader->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    dbReader->SetTableName("_Instance_Levels");
    dbReader->AddFilterAttr(Attr::Attribute(Attr::Id, levelName));
    bool success = dbReader->Open();
    n_assert(success);
    if (dbReader->GetNumRows() == 0)
    {
        n_error("LevelLoader::Load(): level '%s' not found in world database!", levelName.AsCharPtr());
        return false;
    }
    else if (dbReader->GetNumRows() > 1)
    {
        // more then one level of that name, corrupt database?
        n_error("LevelLoader::Load(): more then one level '%s' in world database!", levelName.AsCharPtr());
        return false;
    }
    dbReader->SetToRow(0);

    // get the active layers from the level
    if (activeLayers.IsEmpty())
    {
        Util::Array<Util::String> defaultLayers = dbReader->GetString(Attr::_Layers).Tokenize(";");

        // ask CategoryManager to load level entities
        CategoryManager* categoryManager = CategoryManager::Instance();
        categoryManager->LoadInstances(levelName);
        //loaderServer->SetMaxProgressValue(categoryManager->GetNumInstances());
        loaderServer->LoadEntities(defaultLayers);
    }
    else
    {
        // ask CategoryManager to load level entities
        CategoryManager* categoryManager = CategoryManager::Instance();
        categoryManager->LoadInstances(levelName);
        //loaderServer->SetMaxProgressValue(categoryManager->GetNumInstances());
        loaderServer->LoadEntities(activeLayers);
    }

   	dbReader->Close();
		
	// update progress bar window
    //loaderServer->SetProgressText("Level Loader Done...");
    //loaderServer->UpdateProgressDisplay();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String> LevelLoader::GetLayers(const Util::String& levelName)
{
    // query level instance attributes from database
    Ptr<Db::Reader> dbReader = Db::Reader::Create();
    dbReader->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
    dbReader->SetTableName("_Instance_Levels");
    dbReader->AddFilterAttr(Attr::Attribute(Attr::Id, levelName));
    bool success = dbReader->Open();
    n_assert(success);
    if (dbReader->GetNumRows() == 0)
    {
        n_error("LevelLoader::Load(): level '%s' not found in world database!", levelName.AsCharPtr());
        n_assert(1 == 2);
    }
    else if (dbReader->GetNumRows() > 1)
    {
    // more then one level of that name, corrupt database?
    n_error("LevelLoader::Load(): more then one level '%s' in world database!", levelName.AsCharPtr());
    n_assert(1 == 2);
    }
    dbReader->SetToRow(0);

    // get the active layers from the level
    return dbReader->GetString(Attr::_Layers).Tokenize(";");
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Util::String>
LevelLoader::GetLevels()
{
	const BaseGameFeature::CategoryManager::Category & cat = CategoryManager::Instance()->GetCategoryByName("Levels");
	const Ptr<Db::ValueTable> vals = cat.GetTemplateDataset()->Values();
	Util::Array<Util::String> levels;
	for (int i = 0; i < vals->GetNumRows(); i++)
	{
		levels.Append(vals->GetString(Attr::Id, i));
	}	
	return levels;
}

}; // namespace BaseGameFeature