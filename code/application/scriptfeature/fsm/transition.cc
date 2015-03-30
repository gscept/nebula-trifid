//------------------------------------------------------------------------------
//  transition.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/fsm/transition.h"
#include "messaging/message.h"
#include "scriptfeature/conditions/behaviour/fsmcondition.h"
#include "scriptfeature/actions/actionlist.h"
#include "scriptfeature/conditions/condition.h"
#include "scriptfeature/actions/sequenceaction.h"

namespace FSM
{
__ImplementClass(FSM::Transition, 'FSMR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Transition::Transition()
{
	this->actionList = Actions::SequenceAction::Create();
}

//------------------------------------------------------------------------------
/**
*/
Transition::~Transition()
{
	this->actionList = 0;
}

//------------------------------------------------------------------------------
/**
    This method gets called when the parent state becomes active.
*/
void
Transition::OnActivate()
{
	this->conditionBlock->OnActivate();
	this->actionList->OnActivate();
}

//------------------------------------------------------------------------------
/**
    This method gets called when the parent state becomes inactive.
*/
void
Transition::OnDeactivate()
{
	this->conditionBlock->OnDeactivate();
	this->actionList->OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This method notifies FSMConditions about any incoming messages
    they are interested in.
*/
void
Transition::Notify(const Ptr<Messaging::Message>& msg)
{
	this->conditionBlock->Notify(msg);
	this->actionList->Notify(msg);
}

//------------------------------------------------------------------------------
/**
    This evaluates the state of all conditions and ANDs them together.
    If there are no Conditions, the transition is be true.
*/
bool
Transition::EvaluateConditions()
{
    //this->conditionBlock->Assert();
    return this->conditionBlock->Evaluate();
}

//------------------------------------------------------------------------------
/**
    This executes all actions added to the transition.
*/
void
Transition::ExecuteActions()
{	
    this->actionList->Execute();
}

//------------------------------------------------------------------------------
/**
*/
void
Transition::SetTargetState(const Util::String& n)
{
    this->targetState = n;
}

//------------------------------------------------------------------------------
/**
*/
const Util::String&
Transition::GetTargetState() const
{
    return this->targetState;
}

//------------------------------------------------------------------------------
/**
*/
void
Transition::AddCondition(const Ptr<Conditions::Condition>& condition)
{
    n_assert(0 != condition);
    this->conditionBlock = condition;
}

//------------------------------------------------------------------------------
/**
*/
void
Transition::AddAction(const Ptr<Actions::ActionList>& action)
{
    n_assert(0 != action);
    this->actionList->FromList(action.cast<Actions::Action>());
}

//------------------------------------------------------------------------------
/**
*/
void
Transition::AddAction(const Ptr<Actions::Action>& action)
{
	n_assert(0 != action);
	this->actionList->AppendAction(action);
}

}; // namespace FSM
