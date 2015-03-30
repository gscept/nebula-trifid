//------------------------------------------------------------------------------
//  scriptfeature/scriptfeatureunit.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/scriptfeatureunit.h"
#include "scriptfeature/fsm/behaviourproperty.h"

namespace ScriptFeature
{
__ImplementClass(ScriptFeatureUnit, 'SCFU' , Game::FeatureUnit);
__ImplementSingleton(ScriptFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
ScriptFeatureUnit::ScriptFeatureUnit()
{	
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ScriptFeatureUnit::~ScriptFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptFeatureUnit::OverwriteScriptManager(const Ptr<Script::ScriptManager>& s)
{
	n_assert(s.isvalid());
	this->scriptManager = s;
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptFeatureUnit::OnActivate()
{	
	//this->dialogManager = Script::DialogManager::Create();
	this->logManager = Script::LogManager::Create();
	//this->questManager = Script::QuestManager::Create();
	this->scriptTemplateManager = Script::ScriptTemplateManager::Create();
	this->substitutionManager = Script::SubstitutionManager::Create();

	if(!this->scriptManager.isvalid())
	{
		this->scriptManager = Script::ScriptManager::Create();
	}

	//this->AttachManager(this->dialogManager.upcast<Game::Manager>());
	this->AttachManager(this->logManager.upcast<Game::Manager>());
	//this->AttachManager(this->questManager.upcast<Game::Manager>());
	this->AttachManager(this->scriptTemplateManager.upcast<Game::Manager>());
	this->AttachManager(this->substitutionManager.upcast<Game::Manager>());
	this->AttachManager(this->scriptManager.cast<Game::Manager>());

	FeatureUnit::OnActivate();	
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptFeatureUnit::OnDeactivate()
{		
	FeatureUnit::OnDeactivate();	

	//this->dialogManager = 0;
	this->logManager = 0;
	//this->questManager = 0;
	this->scriptTemplateManager = 0;
	this->substitutionManager = 0;
	this->scriptManager = 0;
}
}; // namespace Game
