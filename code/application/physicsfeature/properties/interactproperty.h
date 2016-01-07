#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::InteractProperty

    Entities that can be clicked or interacted with
  
    (C) 2016 Individual contributors, see AUTHORS file
*/

#include "game/property.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "scriptingfeature/properties/scriptingproperty.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
	class InteractProperty : public ScriptingFeature::ScriptingProperty
{
	__DeclareClass(InteractProperty);
	__SetupExternalAttributes();
public:
    /// constructor
	InteractProperty();
    /// destructor
	virtual ~InteractProperty();

	/// called from Entity::ActivateProperties()
	virtual void OnActivate();
	/// called from Entity:DeactivateProperties()
	virtual void OnDeactivate();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();	

    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

	/// called from within Entity::OnStart() after OnLoad when the complete world exist
	virtual void OnStart();
	
protected:

	/// finds functions in script
	virtual void DetectInteractFunctions();

	/// FIXME: this is kinda broken, but works for now
	bool onMouseEnter;	
	bool onMouseLeave;
	bool onMouseButton;
};
__RegisterClass(InteractProperty);
}; // namespace Properties
//------------------------------------------------------------------------------
