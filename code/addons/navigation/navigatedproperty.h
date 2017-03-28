#pragma once
//------------------------------------------------------------------------------
/**
    @class Navigation::NavigatedProperty

    Entities that can be crowd navigated
      
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "basegamefeature/properties/transformableproperty.h"
#include "messaging/staticmessagehandler.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class NavigatedProperty : public BaseGameFeature::TransformableProperty
{
	__DeclareClass(NavigatedProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    NavigatedProperty();
    /// destructor
    virtual ~NavigatedProperty();

    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();    
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// called from within Entity::OnStart() after OnLoad when the complete world exist    
    virtual void OnStart();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
private:
	/// enable/disable crowd management
	void EnableCrowdManagement(bool enable);
    friend class Messaging::StaticMessageHandler;   	   
	bool isRegistered;
};
__RegisterClass(NavigatedProperty);
}; // namespace Navigation
//------------------------------------------------------------------------------
