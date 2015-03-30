#pragma once
//------------------------------------------------------------------------------
/**
    @class ScriptingFeature::ScriptingTriggerProperty
    
    property that will call entity scripts on different evens like entered trigger, inside trigger and exited trigger
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "scriptingfeature/properties/scriptingproperty.h"

namespace ScriptingFeature
{
class ScriptingTriggerProperty : public ScriptingProperty
{
	__DeclareClass(ScriptingTriggerProperty);
	__SetupExternalAttributes();
public:
	/// constructor
	ScriptingTriggerProperty();
	/// destructor
	virtual ~ScriptingTriggerProperty();

	/// override to register accepted messages
	virtual void SetupAcceptedMessages();

	/// called from Entity::ActivateProperties()
	virtual void OnActivate();	
	/// called from Entity:DeactivateProperties()
	virtual void OnDeactivate();

	/// handle a single message
	virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:

	/// finds functions in script
	virtual void DetectDefaultFunctions();


	bool hasEnter;
	bool hasInside;
	bool hasExit;

}; 

__RegisterClass(ScriptingTriggerProperty);

} // namespace ScriptingFeature
//------------------------------------------------------------------------------