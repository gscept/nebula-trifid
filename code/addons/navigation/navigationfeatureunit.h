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
	/// called from within GameServer::NotifyBeforeCleanup() before shutting down a level
	virtual void OnBeforeCleanup();
private:	
    Ptr<Navigation::CrowdManager> crowdManager;
	Ptr<Navigation::NavigationServer> navigationServer;	
};
}; // namespace Navigation