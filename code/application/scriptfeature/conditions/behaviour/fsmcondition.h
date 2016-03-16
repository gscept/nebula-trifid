#pragma once
//------------------------------------------------------------------------------
/**
    @class Conditions::FSMCondition
    
    A specialized condition for the finite state machine system which
    is activated when its state becomes active and which gets notified
    about messages to the parent entity. This enabled the condition
    to be triggered by incoming messages instead of a polling mechanism.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/conditions/condition.h"

//------------------------------------------------------------------------------
namespace Conditions
{
class FSMCondition : public Condition
{    
    __DeclareClass(FSMCondition);
public:
    /// notify about incoming message
    virtual void Notify(const Ptr<Messaging::Message>& msg);
    /// called on state activation
    virtual void OnActivate();
    /// called on state deactivation
    virtual void OnDeactivate();
};
__RegisterClass(FSMCondition);

}; // namespace FSMCondition
//------------------------------------------------------------------------------