#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::NavigationFeatureUnit

    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "game/featureunit.h"
#include "navigation/crowdmanager.h"
#include "navigation/navigationserver.h"

namespace Navigation
{
class NavigationFeatureUnit : public Game::FeatureUnit

{
    __DeclareClass(NavigationFeatureUnit);
    __DeclareSingleton(NavigationFeatureUnit);
public:
    /// Constructor
    NavigationFeatureUnit();
    /// Destructor
    ~NavigationFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();	

private:	
    Ptr<Navigation::CrowdManager> crowdManager;
	Ptr<Navigation::NavigationServer> navigationServer;	
};
}; // namespace Navigation