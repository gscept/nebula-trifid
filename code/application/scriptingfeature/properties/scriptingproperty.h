#pragma once
//------------------------------------------------------------------------------
/**
    @class ScriptingFeature::ScriptingProperty

    Entites with this property can register scripts that can be run on different occasions
  
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/

#include "game/property.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "scriptingfeature/scriptingprotocol.h"

#define SCRIPT_ERROR_CHECK(setup,name)\
if(ScriptServer::Instance()->HasError())\
{\
	n_printf("Error evaluating " #setup " for " #name ":\n%s\n",ScriptServer::Instance()->GetError().AsCharPtr());\
}

//------------------------------------------------------------------------------
namespace ScriptingFeature
{
class ScriptingProperty : public Game::Property
{
	__DeclareClass(ScriptingProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    ScriptingProperty();
    /// destructor
    virtual ~ScriptingProperty();   

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();

	/// setup callbacks for this property
	virtual void SetupCallbacks();

	/// called from Entity::ActivateProperties()
	virtual void OnActivate();	
	/// called from Entity:DeactivateProperties()
	virtual void OnDeactivate();
	/// called from within Entity::OnStart() after OnLoad when the complete world exist
	virtual void OnStart();
	
	/// per-frame updates
	virtual void OnBeginFrame();	

    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

	/// hack for testing
	void SetScript(const Util::String & script){entityScript = script;}

	/// registers a script into a previously registered script function table
	void RegisterScriptfunction(const Util::String& script, const Util::String& function);

	/// calls a registered function from a script function table
	void CallRegisteredfunction(const Util::String & table, const Util::String& function);

protected:

	///
	void OnKeyDown(Input::Key::Code key);
	///
	void OnKeyUp(Input::Key::Code key);
	///
	void OnMouseDown(Input::MouseButton::Code button);

	friend class ScriptingInputHandler;
	/// sets up script from attribute
	void SetupScript(const Util::String& script);

	/// finds functions in script
	virtual void DetectDefaultFunctions();

	/// FIXME: this is kinda broken, but works for now
	bool onFrame;	
	bool onCollision;
	bool onInput;
	bool onAnimEvent;
	Util::String entityScript;

};
__RegisterClass(ScriptingProperty);
}; // namespace Properties
//------------------------------------------------------------------------------
