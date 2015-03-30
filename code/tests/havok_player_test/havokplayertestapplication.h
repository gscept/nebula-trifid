#pragma once
//------------------------------------------------------------------------------
/**
    @class Tests::HavokPlayerTestApplication
    
    Test game for a player-driven Havok character.
    
    (C) 2013 gscept
*/

#include "appgame/gameapplication.h"
#include "application/appgame/gameapplication.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "basegamefeature/statehandlers/gamestatehandler.h"
#include "basegamefeature/basegamefeatureunit.h"

#if __USE_PHYSICS__
#include "physicsfeature/physicsfeatureunit.h"
#include "stateobjectfeature/stateobjectfeatureunit.h"
#endif

#if !__USE_HAVOK__
#error What are you doing, N_BUILD_HAVOK must be set!
#endif

//------------------------------------------------------------------------------
namespace Tests
{
class HavokPlayerTestState;

class HavokPlayerTestApplication : public App::GameApplication
{
	__DeclareSingleton(HavokPlayerTestApplication);
public:
	/// constructor
	HavokPlayerTestApplication();
	/// destructor
	~HavokPlayerTestApplication();
	/// open the application
	virtual bool Open();
	/// close the application
	virtual void Close();

private:
	/// setup application state handlers
	void SetupStateHandlers();
	/// cleanup application state handlers
	void CleanupStateHandlers();
	/// setup game features
	void SetupGameFeatures();
	/// cleanup game features
	void CleanupGameFeatures();

	Ptr<BaseGameFeature::BaseGameFeatureUnit> baseFeature;
	Ptr<GraphicsFeature::GraphicsFeatureUnit> graphicsFeature;
	Ptr<PhysicsFeature::PhysicsFeatureUnit> physicsFeature;		

	Ptr<HavokPlayerTestState> havokState;
	
}; 
} 
// namespace Tests
//------------------------------------------------------------------------------