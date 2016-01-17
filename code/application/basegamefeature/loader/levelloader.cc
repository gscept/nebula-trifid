//------------------------------------------------------------------------------
//  loader/levelloader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loader/levelloader.h"
#include "loader/loaderserver.h"
#include "appgame/appconfig.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "core/factory.h"
#include "addons/db/dbserver.h"
#include "addons/db/reader.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "loader/environmentloader.h"
#include "loader/entityloader.h"
#include "game/gameserver.h"
#include "loader/loaderserver.h"
#include "managers/categorymanager.h"
#include "math/float4.h"
#include "posteffectentity.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "visibility/visibilityprotocol.h"
#include "graphics/graphicsinterface.h"
#include "navigation/navigationserver.h"
#include "posteffectfeatureunit.h"

using namespace Math;
using namespace Graphics;
using namespace Visibility;
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

    // setup new physics level
    Ptr<Physics::Scene> physicsLevel = Physics::Scene::Create();
    Physics::PhysicsServer::Instance()->SetScene(physicsLevel);

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

    if (GraphicsFeature::GraphicsFeatureUnit::HasInstance())
    {
        GraphicsFeature::GraphicsFeatureUnit::Instance()->OnEntitiesLoaded();

	
		// get world extents
		Math::float4 bbCenter = dbReader->GetFloat4(Attr::WorldCenter);
		Math::float4 bbExtents = dbReader->GetFloat4(Attr::WorldExtents);
		Math::bbox box = Math::bbox(bbCenter, bbExtents);

		Ptr<ChangeVisibilityBounds> msg = ChangeVisibilityBounds::Create();
		msg->SetWorldBoundingBox(box);
		msg->SetStageName("DefaultStage");
		GraphicsInterface::Instance()->Send(msg.upcast<Messaging::Message>());

		Math::matrix44 trans = dbReader->GetMatrix44(Attr::GlobalLightTransform);
		GraphicsFeature::GraphicsFeatureUnit::Instance()->GetGlobalLightEntity()->SetTransform(trans);
    } 

	if (PostEffect::PostEffectFeatureUnit::HasInstance())
	{
		Util::String preset = dbReader->GetString(Attr::PostEffectPreset);
		PostEffect::PostEffectFeatureUnit::Instance()->ApplyPreset(preset);
	}

	dbReader->Close();

	if(Db::DbServer::Instance()->GetGameDatabase()->HasTable("_Instance_NavMeshData"))
	{
		Ptr<Db::Reader> dbReader = Db::Reader::Create();
		dbReader->SetDatabase(Db::DbServer::Instance()->GetGameDatabase());
		dbReader->SetTableName("_Instance_NavMeshData");
		dbReader->AddFilterAttr(Attr::Attribute(Attr::_Level, levelName));
		dbReader->Open();
		for(int i = 0 ; i<dbReader->GetNumRows() ; i++)		
		{
			Util::String nav;
			dbReader->SetToRow(i);
			nav = dbReader->GetString(Attr::NavMeshData);
			Navigation::NavigationServer::Instance()->LoadNavigationData(nav,nav);
			Navigation::NavigationServer::Instance()->LoadNavMeshGenerationData(nav,dbReader);
            Navigation::NavigationServer::Instance()->SelectNavMesh(nav);
		}
		dbReader->Close();
	}
	
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