//------------------------------------------------------------------------------
//  fsmaction.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/actions/behaviour/fsmaction.h"

namespace Actions
{
__ImplementClass(Actions::FSMAction, 'FSAC', Actions::Action);

//------------------------------------------------------------------------------
/**
    The state machine will forward any incoming messages to the action.
*/
void
FSMAction::Notify(const Ptr<Messaging::Message>& msg)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
FSMAction::OnActivate()
{
    // empty, derive in subclass
}

//------------------------------------------------------------------------------
/**
*/
void
FSMAction::OnDeactivate()
{
    // empty, derive in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
FSMAction::Trigger()
{
    // TODO: [np] until there is a consistent solution for continuous actions
    // and FSM actions, fake a trigger function, which only does "Execute" once
    this->Execute();
    return false;
}

} // namespace FSM

