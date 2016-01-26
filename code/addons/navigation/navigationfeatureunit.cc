//------------------------------------------------------------------------------
//  navigation/navigationfeatureunit.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "navigationfeatureunit.h"
#include "loader/loaderserver.h"
#include "navmeshloader.h"

namespace Navigation
{
__ImplementClass(Navigation::NavigationFeatureUnit, 'NAFU', Game::FeatureUnit);
__ImplementSingleton(Navigation::NavigationFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
NavigationFeatureUnit::NavigationFeatureUnit()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
NavigationFeatureUnit::~NavigationFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
NavigationFeatureUnit::OnActivate()
{
    FeatureUnit::OnActivate();
    Ptr<Navigation::NavMeshLoader> meshLoader = Navigation::NavMeshLoader::Create();
    BaseGameFeature::LoaderServer::Instance()->AttachEntityLoader(meshLoader.cast<BaseGameFeature::EntityLoaderBase>());
    this->crowdManager = Navigation::CrowdManager::Create();
    this->AttachManager(this->crowdManager.upcast<Game::Manager>());
}

//------------------------------------------------------------------------------
/**
*/
void
NavigationFeatureUnit::OnDeactivate()
{	
    this->RemoveManager(this->crowdManager.upcast<Game::Manager>());
    this->crowdManager = 0;
    FeatureUnit::OnDeactivate();    
}

//------------------------------------------------------------------------------
/**
*/
void
NavigationFeatureUnit::OnBeforeCleanup()
{
	this->crowdManager->Cleanup();	
}

};