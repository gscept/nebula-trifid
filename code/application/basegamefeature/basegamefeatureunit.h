#pragma once
//------------------------------------------------------------------------------
/**
    @class Game::BaseGameFeatureUnit
    
    The BaseGameFeatureUnit creates everything to allow load and run a game level.
    Therefore it creates managers to allow creation and handling of
    entities and its properties. It supports loading of a game level from a database
    with the db server and offers simple methods for opening a new game, loading a level 
    or a savegame.
    The Feature also creates the timemanager and different timesources used by subsystems.

    If you wanna use your own managers (other specilized entitymanager), derive from this class 
    and overwrite OnActivate() OnDeactivate().

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/featureunit.h"
#include "math/bbox.h"
#include "basegametiming/timemanager.h"
#include "managers/factorymanager.h"
#include "managers/focusmanager.h"
#include "managers/entitymanager.h"
#include "managers/categorymanager.h"
#include "managers/globalattrsmanager.h"
#include "managers/audiomanager.h"
#include "db/dbserver.h"
#include "loader/loaderserver.h"
#include "http/httprequesthandler.h"
#include "vibration/vibrationinterface.h"
#include "faudio/audioserver.h"
#include "managers/envquerymanager.h"
#include "managers/levelattrsmanager.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{

class BaseGameFeatureUnit : public Game::FeatureUnit    
{
    __DeclareClass(BaseGameFeatureUnit);
    __DeclareSingleton(BaseGameFeatureUnit);    

public:

    /// constructor
    BaseGameFeatureUnit();
    /// destructor
    virtual ~BaseGameFeatureUnit();

    /// called from BaseGameFeatureUnit::ActivateProperties()
    virtual void OnActivate();
    /// called from BaseGameFeatureUnit::DeactivateProperties()
    virtual void OnDeactivate();
         
    /// called at the end of the feature trigger cycle
    virtual void OnEndFrame();
    /// called when game debug visualization is on
    virtual void OnRenderDebug();
    /// 
    virtual void OnFrame();

    /// return true if a current game exists
    static bool CurrentGameExists();
    /// setup a new game
    virtual bool NewGame();
    /// continue game 
    virtual bool ContinueGame();
    /// create a new save game
    virtual bool SaveGame(const Util::String& saveGameName);
    /// load an existing save game
    virtual bool LoadGame(const Util::String& saveGameName);
    /// start new game using level 
    virtual bool LoadLevel(const Util::String& levelName);
	/// start new game using level with entity sync enabled 
	virtual bool LoadNetworkedLevel(const Util::String& levelName);
    /// get name of startup level (defined in world database)
    Util::String GetStartupLevel();
    /// has level parameter set
    bool HasLevelParameter() const;

	/// enables/disables the auto saving feature
	void SetAutosaveEnabled(bool b);

    /// set current level name
    void SetCurrentLevel(const Util::String& n);
    /// get current level name
    Util::String GetCurrentLevel() const;
    /// set optional world dimensions
    void SetWorldDimensions(const Math::bbox& box);
    /// get world dimensions
    const Math::bbox& GetWorldDimensions() const;
    /// setup an empty game world
    virtual void SetupEmptyWorld();
    /// setup world from level
    virtual void SetupWorldFromCurrentLevel();
    /// cleanup the game world
    virtual void CleanupWorld();

    //Select layers
    void SetActivateLayers(const Util::Array<Util::String>& layers);

    /// set factory manager rtti
    void SetFactoryManager(const Ptr<FactoryManager>& manager);
        
protected:

    /// handle input keys
    void HandleInput();

	bool enableAutosave;
    Ptr<FactoryManager> factoryManager;
    Ptr<FocusManager> focusManager;
    Ptr<EntityManager> entityManager;
    Ptr<GlobalAttrsManager> globalAttrManager;
	Ptr<LevelAttrsManager> levelAttrManager;
    Ptr<CategoryManager> categoryManager;
    Ptr<TimeManager> timeManager;
    Ptr<BaseGameFeature::LoaderServer> loaderServer;
    Ptr<Db::DbServer> dbServer;    
    Ptr<Vibration::VibrationInterface> vibInterface;    
    Ptr<EnvQueryManager> envQueryManager;
    Ptr<AudioManager> audioManager;
    Util::Array<Util::String> activeLayers;
#if __NEBULA3_HTTP__
    Ptr<Http::HttpRequestHandler> debugRequestHandler;
#endif

    Math::bbox worldBox;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
BaseGameFeatureUnit::SetAutosaveEnabled( bool b )
{
	this->enableAutosave = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void
BaseGameFeatureUnit::SetWorldDimensions(const Math::bbox& box)
{
    this->worldBox = box;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox&
BaseGameFeatureUnit::GetWorldDimensions() const
{
    return this->worldBox;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
BaseGameFeatureUnit::SetFactoryManager(const Ptr<FactoryManager>& manager)
{
    this->factoryManager = manager;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
BaseGameFeatureUnit::HasLevelParameter() const
{
    return this->args.HasArg("-level");
}

} // namespace BaseGameFeature
//------------------------------------------------------------------------------
