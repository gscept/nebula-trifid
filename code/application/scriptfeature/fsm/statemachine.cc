//------------------------------------------------------------------------------
//  statemachine.cc
//  (C) 2006 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptfeature/fsm/statemachine.h"
#include "scriptfeature/fsm/state.h"
#include "scriptfeature/fsm/transition.h"
#include "scriptfeature/actions/action.h"
#include "scriptfeature/actions/actionlist.h"
#include "scriptfeature/actions/sequenceaction.h"
#include "messaging/message.h"
#include "game/entity.h"

namespace FSM
{
__ImplementClass(FSM::StateMachine, 'FSMA', Core::RefCounted);

using namespace Util;
//------------------------------------------------------------------------------
/**
*/
StateMachine::StateMachine() :
    isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StateMachine::~StateMachine()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Add a state to the state machine.
*/
void
StateMachine::AddState(const Ptr<State>& s)
{
    n_assert(s.isvalid());
    this->states.Append(s);
}

//------------------------------------------------------------------------------
/**
    Find a state by its name. Returns 0 if not found.
*/
Ptr<State>
StateMachine::FindStateByName(const Util::String& n)
{
    n_assert(n.IsValid());
    int i;
    for (i = 0; i < this->states.Size(); i++)
    {
        if (this->states[i]->GetName() == n)
        {
            return this->states[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Open the state machine. This will set the first state in the state 
    array as active state.
*/
void
StateMachine::Open()
{
    n_assert(!this->IsOpen());
    n_assert(this->states.Size() > 0);

    // make sure that all target states of all transitions exist
    int stateIndex;
    for (stateIndex = 0; stateIndex < this->states.Size(); stateIndex++)
    {
        const Ptr<State>& curState = this->states[stateIndex];
        int transIndex;
        for (transIndex = 0; transIndex < this->states[stateIndex]->GetNumTransitions(); transIndex++)
        {
            const Ptr<Transition>& curTransition = curState->GetTransitionAtIndex(transIndex);
            const Util::String& targetState = curTransition->GetTargetState();
            if (targetState.IsValid())
            {
                if (0 == this->FindStateByName(curTransition->GetTargetState()))
                {
                    n_error("StateMachine::Open(): target state '%s' not defined!", curTransition->GetTargetState().AsCharPtr());
                }
            }
        }
    }

    // activate the start state or if none defined the first state
    const Ptr<State>& startState = this->FindStateByName("Start");
    if (startState.isvalid())
    {
        this->SetCurrentState(startState);
    }
    else
    {
        this->SetCurrentState(this->states[0]);
    }
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
    Close the state machine. This will unset the current state, so that
    it may cleanup itself properly.
*/
void
StateMachine::Close()
{
    n_assert(this->IsOpen());
    this->ClearCurrentState();
    this->stateHistory.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    This updates the state machine, must be called frequently while the 
    state machine is open.
*/
bool
StateMachine::Update()
{
    n_assert(this->IsOpen());
    n_assert(this->currentState.isvalid());

    // call OnFrame() on current state
    this->currentState->OnFrame();

    // evaluate transitions of current frame, the first that evaluates to
    // true will cause a state switch
    const Ptr<Transition>& transition = this->currentState->EvaluateTransitions();
    if (transition.isvalid())
    {
        // need to switch to a new state, first call the OnExit()
        // method of the current state
        this->ClearCurrentState();

        // now execute the transition actions
        transition->ExecuteActions();
        
        // set the new state as active state
        const Util::String& targetStateName = transition->GetTargetState();
    #if NEBULA3_DEBUG
        //n_printf("%s_%i StateMachine: %s Transition to %s\n", this->entity->GetCategory().AsCharPtr(), this->entity->GetUniqueId(), this->GetName().AsCharPtr(), targetStateName.AsCharPtr());
    #endif
        if (targetStateName.IsValid())
        {
            const Ptr<State>& newState = this->FindStateByName(transition->GetTargetState());
            this->SetCurrentState(newState);
        }
        else
        {
            // transition has no target state given, this just means we
            // should activate the previous state
            const Ptr<State>& newState = this->PopState();
            this->SetCurrentState(newState);
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Set a new current state.
*/
void
StateMachine::SetCurrentState(const Ptr<State>& s)
{
    n_assert(!this->currentState.isvalid());
    n_assert(s.isvalid());
    
    this->PushState(s);
    this->currentState = s;
    s->OnEntry();
}

//------------------------------------------------------------------------------
/**
    Returns the current state.
*/
const Ptr<State>&
StateMachine::GetCurrentState() const
{
    return this->currentState;
}

//------------------------------------------------------------------------------
/**
    Clears the current state.
*/
void
StateMachine::ClearCurrentState()
{
    n_assert(this->currentState.isvalid());
    this->currentState->OnExit();
    this->currentState = 0;
}

//------------------------------------------------------------------------------
/**
    Push a state on the state history stack, but limit the stack size
    to MaxHistorySize.
*/
void
StateMachine::PushState(const Ptr<State>& s)
{
    n_assert(s.isvalid());
    this->stateHistory.Append(s);

    // clamp history stack to max history size
    while (this->stateHistory.Size() > MaxHistorySize)
    {
        this->stateHistory.EraseIndex(0);
    }
}
    
//------------------------------------------------------------------------------
/**
    Pop a state from the history stack. Throws an assertion if the
    stack is empty.
*/
Ptr<State>
StateMachine::PopState()
{
    n_assert(this->stateHistory.Size() > 0);
    Ptr<State> s = this->stateHistory.Back();
    this->stateHistory.EraseIndex(this->stateHistory.Size() - 1);
    return s;
}

//------------------------------------------------------------------------------
/**
    This notifies the current state about a message.
*/
void
StateMachine::Notify(const Ptr<Messaging::Message>& msg)
{
    if (this->currentState.isvalid())
    {
        this->currentState->Notify(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
StateMachine::GetDebugTxt() const
{
    const Ptr<FSM::State> state = this->GetCurrentState();
    Util::String dbgTxt = state->GetName();
    dbgTxt.Append("\n");
    const Ptr<Actions::SequenceAction>& actions = state->GetFrameActions();
    String allActions;    
    Util::Array<Ptr<Actions::Action> > list = actions->GetActionList();    
    while (list.Size() > 0 && list[0]->IsA(Actions::ActionList::RTTI))
    {
        list = list[0].cast<Actions::ActionList>()->GetActionList();
    }
    IndexT actionIdx;
    for (actionIdx = 0; actionIdx < list.Size(); ++actionIdx)
    {
        allActions.Append(list[actionIdx]->GetDebugTxt());
        allActions.Append("; ");
    }
    dbgTxt.Append(allActions);

    return dbgTxt;
}

//------------------------------------------------------------------------------
/**
*/
Math::float4 
StateMachine::GetDebugColor() const
{
    SizeT numStates = this->states.Size();
    IndexT curStateIndex = this->states.FindIndex(this->currentState);
    float r = (float)curStateIndex / (float)numStates;    
    Math::float4 color(r, Math::n_saturate(float(curStateIndex % 3)), float(curStateIndex % 2),0.8f);    
    return color;
}
} // namespace FSM
