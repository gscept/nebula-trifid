#pragma once
//------------------------------------------------------------------------------
/**
    @class ScriptFeature::ScriptFeatureUnit
    
    The ScriptFeatureUnit deliveres a handfull servers for successfully loading,
	playing and controling of scripts (also cutscenes)

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/featureunit.h"
#include "scriptfeature/managers/dialogmanager.h"
#include "scriptfeature/managers/logmanager.h"
#include "scriptfeature/managers/questmanager.h"
#include "scriptfeature/managers/scripttemplatemanager.h"
#include "scriptfeature/managers/substitutionmanager.h"
#include "scriptfeature/managers/scriptmanager.h"

//------------------------------------------------------------------------------
namespace ScriptFeature
{

class ScriptFeatureUnit : public Game::FeatureUnit    
{
    __DeclareClass(ScriptFeatureUnit);
    __DeclareSingleton(ScriptFeatureUnit);   

public:
    /// constructor
    ScriptFeatureUnit();
    /// destructor
    virtual ~ScriptFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();

	/// overwrite the default script manager
	virtual void OverwriteScriptManager(const Ptr<Script::ScriptManager>& s);
    
protected:
    //Ptr<Script::DialogManager> dialogManager;
	Ptr<Script::LogManager> logManager;
	Ptr<Script::QuestManager> questManager;
	Ptr<Script::ScriptTemplateManager> scriptTemplateManager;
	Ptr<Script::SubstitutionManager> substitutionManager;
	Ptr<Script::ScriptManager> scriptManager;
};

}; // namespace ScriptFeature
//------------------------------------------------------------------------------