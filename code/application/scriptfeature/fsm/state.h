#pragma once
//------------------------------------------------------------------------------
/**
    @class FSM::State
    
    Implements a state of a state machine.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "game/entity.h"

namespace Actions
{
    class SequenceAction;
    class ActionList;
	class Action;
}

namespace Messaging
{
    class Message;
}

//------------------------------------------------------------------------------
namespace FSM
{

class Transition;

class State : public Core::RefCounted
{    
    __DeclareClass(State);
public:
    /// constructor
    State();
    /// destructor
    virtual ~State();
    /// set name of the state
    virtual void SetName(const Util::String& n);
    /// get name of the state
    virtual const Util::String& GetName() const;
	/// Set optional entity to `v'.
	virtual void SetEntity(const Ptr<Game::Entity>& v);
	/// Get optional target entity if exists.
	const Ptr<Game::Entity>& GetEntity() const;
	/// Does this contain a target entity?
	bool HasEntity() const;
    /// add a state entry action
    void AddEntryAction(const Ptr<Actions::ActionList>& action);
	/// add single entry action
	void AddEntryAction(const Ptr<Actions::Action>& action);
	/// add a state frame action
    void AddFrameAction(const Ptr<Actions::ActionList>& action);
	/// add single frame action
	void AddFrameAction(const Ptr<Actions::Action>& action);
	/// add a state exit action
    void AddExitAction(const Ptr<Actions::ActionList>& action);
	/// add single exit action
	void AddExitAction(const Ptr<Actions::Action>& action);
	/// add a transition object to the state
    void AddTransition(const Ptr<Transition>& transition);
    /// called when state is entered
    virtual void OnEntry();
    /// called per-frame while state is active
    virtual void OnFrame();
    /// called when state is left
    virtual void OnExit();
	///
	virtual void OnRenderDebug();
    /// notify the state about incoming message
   virtual  void Notify(const Ptr<Messaging::Message>& msg);
    /// evaluate transitions, return first transition which evaluates true
    Ptr<Transition> EvaluateTransitions();
    /// get number of transitions
    int GetNumTransitions() const;
    /// get pointer to transition at index
    const Ptr<Transition>& GetTransitionAtIndex(int index) const;
    /// get all frame actions
    const Ptr<Actions::SequenceAction>& GetFrameActions() const;

private:
 
    Util::String name;
    Ptr<Actions::SequenceAction> entryActions;
    Ptr<Actions::SequenceAction> frameActions;
    Ptr<Actions::SequenceAction> exitActions;
    Util::Array<Ptr<Transition>> transitions; 
	Ptr<Game::Entity> entity;
};
__RegisterClass(State)

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Actions::SequenceAction>& 
State::GetFrameActions() const
{
    return this->frameActions;
}
}; // namespace FSM
//------------------------------------------------------------------------------