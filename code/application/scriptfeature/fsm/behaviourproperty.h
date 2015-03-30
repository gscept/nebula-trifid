#pragma once
//------------------------------------------------------------------------------
/**
    @class Properties::BehaviourProperty
  
    Adds state-machine-driven "behaviour" to an entity. Behaviours are 
    defined in XML scripts which define a state machine from standard 
    actions and conditions. 
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "attr/attribute.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Attr
{
    DeclareString(Behaviour, 'BEHV', ReadWrite);
}
//------------------------------------------------------------------------------
namespace FSM
{
class StateMachine;

class BehaviourProperty : public Game::Property
{
    __DeclareClass(BehaviourProperty);
	__SetupExternalAttributes();
public:
    /// setup callbacks for this property
    virtual void SetupCallbacks();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called on begin of frame
    virtual void OnBeginFrame();
    /// called on render debug
    virtual void OnRenderDebug();

protected:
    /// switch state machine
    void SwitchStateMachine(const Util::String& stateMachineName);

    int stateMachineIndex;
    static int numStateMachines;
    static Timing::Time updateTime;
    Ptr<FSM::StateMachine> stateMachine;
};
__RegisterClass(BehaviourProperty);

} // namespace Properties
//------------------------------------------------------------------------------