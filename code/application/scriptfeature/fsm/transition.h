#pragma once
//------------------------------------------------------------------------------
/**
    @class FSM::Transition
    
    Implements a state transition. If no target state name if given, the 
    previously active state will be activated when the transition
    conditions are met.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "../actions/action.h"
#include "../conditions/and.h"

namespace Conditions
{
    class Condition;
}
namespace Messaging
{
    class Message;
}
namespace Actions
{
    class SequenceAction;
    class ActionList;
}

//------------------------------------------------------------------------------
namespace FSM
{
class Transition : public Core::RefCounted
{
    __DeclareClass(Transition);

public:
    /// constructor
    Transition();
    /// destructor
    virtual ~Transition();
    /// called when parent states becomes active
    void OnActivate();
    /// called when parent states becomes inactive
    void OnDeactivate();
    /// notify FSMConditions about message
    void Notify(const Ptr<Messaging::Message>& msg);
    /// set name of target state
    void SetTargetState(const Util::String& n);
    /// get name of target state
    const Util::String& GetTargetState() const;
    /// add a condition to the transition's AND condition block
    void AddCondition(const Ptr<Conditions::Condition>& condition);
    /// add a transition action
    void AddAction(const Ptr<Actions::ActionList>& action);
	/// add a single transition action
	void AddAction(const Ptr<Actions::Action>& action);
	/// evaluate transition conditions
    bool EvaluateConditions();
    /// execute actions associated with transition
    void ExecuteActions();

private:
    Util::String targetState;
    Ptr<Conditions::And> conditionBlock;
    Ptr<Actions::SequenceAction> actionList;
};
__RegisterClass(Transition)

}; // namespace FSM
//------------------------------------------------------------------------------