#pragma once
//------------------------------------------------------------------------------
/**
	@class ScriptingFeature::ScriptingFeatureUnit
    
    The FeatureUnit initializes the scripting subsystem
	(C) 2012-2015 Individual contributors, see AUTHORS file	
*/
#include "game/featureunit.h"
#include "scripting/scriptserver.h"
#include "managers/factorymanager.h"
#include "managers/entitymanager.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "game/entity.h"
#include "scriptinginputhandler.h"

//------------------------------------------------------------------------------
namespace ScriptingFeature
{

class ScriptingFeatureUnit : public Game::FeatureUnit    
{
    __DeclareClass(ScriptingFeatureUnit);
    __DeclareSingleton(ScriptingFeatureUnit);   

public:
    /// constructor
    ScriptingFeatureUnit();
    /// destructor
    virtual ~ScriptingFeatureUnit();

    /// called from GameServer::ActivateProperties()
    virtual void OnActivate();
    /// called from GameServer::DeactivateProperties()
    virtual void OnDeactivate();
    
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();

	/// called when game debug visualization is on
	virtual void OnRenderDebug();

	/// execute script in string
	bool EvalScript(const Util::String & scriptString);
	/// execute a script attached to an entity
	bool EvalEntityScript(const Ptr<Game::Entity> & entity, const Util::String & functionName, const Util::String & scriptString);

	/// execute a script in a file
	bool EvalScriptFile(const IO::URI & file);

	void SetAttribute(unsigned int entityid, const Util::String& attr, const Util::String & value);
	Util::String GetAttribute(unsigned int entityid, const Util::String& attr);
	unsigned int CreateEntityByTemplate(const Util::String& category, const Util::String& templ);

protected:
	Ptr<Scripting::ScriptServer> scriptingServer;
	Ptr<ScriptingFeature::ScriptingInputHandler> inputHandler;
};

}; // namespace ScriptingFeature
//------------------------------------------------------------------------------