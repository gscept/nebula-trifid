//------------------------------------------------------------------------------
//  navigation/navmeshloader.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "navmeshloader.h"
#include "loader/loaderserver.h"
#include "managers/categorymanager.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "navigationserver.h"

namespace Navigation
{
using namespace Game;
using namespace Util;
using namespace BaseGameFeature;

__ImplementClass(NavMeshLoader, 'NMLO', BaseGameFeature::EntityLoaderBase);

//------------------------------------------------------------------------------
/**
*/
bool
NavMeshLoader::Load(const Util::Array<Util::String>& activeLayers)
{
    // get NavMeshData instance table from category manager
    CategoryManager* categoryManager = CategoryManager::Instance();
  
    // load env entities if instances exists
    if (categoryManager->HasInstanceTable("NavMeshData"))
    {  
        Db::ValueTable* table = categoryManager->GetInstanceTable("NavMeshData");

        IndexT rowIndex;
        SizeT numRows = table->GetNumRows();
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            // only load entity if part of an active layer
            if (this->EntityIsInActiveLayer(table, rowIndex, activeLayers))
            {
                const Util::String& resName = table->GetString(Attr::NavMeshData, rowIndex);
                                
                Navigation::NavigationServer::Instance()->LoadNavigationData(resName, resName);
                Navigation::NavigationServer::Instance()->LoadNavMeshGenerationData(resName, table, rowIndex);
                Navigation::NavigationServer::Instance()->SelectNavMesh(resName);                
            }
        }
    }
    return true;    
}

}; // namespace Navigation