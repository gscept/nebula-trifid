//------------------------------------------------------------------------------
//  loader/environmentloader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "loader/environmentloader.h"
#include "basegamefeature/loader/loaderserver.h"
#include "basegamefeature/managers/categorymanager.h"
#include "managers/enventitymanager.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"

namespace GraphicsFeature
{
using namespace Game;
using namespace Util;
using namespace BaseGameFeature;

__ImplementClass(GraphicsFeature::EnvironmentLoader, 'LENV', EntityLoaderBase);

//------------------------------------------------------------------------------
/**
*/
bool
EnvironmentLoader::Load(const Util::Array<Util::String>& activeLayers)
{
    // get _Environment instance table from category manager
    CategoryManager* categoryManager = CategoryManager::Instance();
  
    // load env entities if instances exists
    if (categoryManager->HasInstanceTable("_Environment"))
    {  
        Db::ValueTable* table = categoryManager->GetInstanceTable("_Environment");

        IndexT rowIndex;
        SizeT numRows = table->GetNumRows();
        for (rowIndex = 0; rowIndex < numRows; rowIndex++)
        {
            // only load entity if part of an active layer
            if (this->EntityIsInActiveLayer(table, rowIndex, activeLayers))
            {
                const Util::String& resName = table->GetString(Attr::Graphics, rowIndex);
                this->UpdateProgressIndicator(resName);
                EnvEntityManager::Instance()->CreateEnvEntity(table, rowIndex);
            }
        }
    }
    return true;    
}

//------------------------------------------------------------------------------
/**
    Update the progress indicator...
*/
void
EnvironmentLoader::UpdateProgressIndicator(const Util::String& resName)
{
    //BaseGameFeature::LoaderServer* loaderServer = BaseGameFeature::LoaderServer::Instance();
    //Util::String progressText;
    //progressText.Format("Loading env object '%s'...", resName.Get());
    //loaderServer->SetProgressText(progressText);
    //loaderServer->AdvanceProgress(1);
    //loaderServer->UpdateProgressDisplay();
}

}; // namespace GraphicsFeature