#pragma once
//------------------------------------------------------------------------------
/**
    @class Actions::FSMAction
    
    A specialized action subclass with support for the behaviour state machine.
    This just adds an OnActivate() and OnDeactivate() method to allow
    masterEvents which continue over several frames and need a proper 
    initialization and cleanup point.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "scriptfeature/actions/action.h"

//------------------------------------------------------------------------------
namespace Actions
{
class FSMAction : public Actions::Action
{
    __DeclareClass(FSMAction);

public:
    /// notify about incoming message
    virtual void Notify(const Ptr<Messaging::Message>& msg);
    /// called when state is activated
    virtual void OnActivate();
    /// called when state is deactivated
    virtual void OnDeactivate();
    
    /// do every frame
    virtual bool Trigger();
};
__RegisterClass(FSMAction);

}; // namespace FSM
//------------------------------------------------------------------------------