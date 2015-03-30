#pragma once
//------------------------------------------------------------------------------
/**
    @class FSM::StateMachine
    
    Implements the state machine for the FSM subsystem. 

    Note: On first start the state "Start" will be started if available, 
    otherwise the first in row.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

namespace Game
{
    class Entity;
}
namespace Messaging
{
    class Message;
}

//------------------------------------------------------------------------------
namespace FSM
{
class State;

class StateMachine : public Core::RefCounted
{    
    __DeclareClass(StateMachine);
public:
    /// constructor
    StateMachine();
    /// destructor
    virtual ~StateMachine();
    /// set the name of the state machine
    void SetName(const Util::String& n);
    /// get name of state machine
    const Util::String& GetName() const;
    /// set pointer to entity which owns the state machine
    void SetEntity(const Ptr<Game::Entity>& e);
    /// get pointer to entity
    const Ptr<Game::Entity>& GetEntity() const;
    /// add a state to the state machine
    void AddState(const Ptr<State>& s);
    /// find a state by name
    Ptr<State> FindStateByName(const Util::String& n);
    /// called when the state machine is initially opened
    void Open();
    /// called when the state machine is closed
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// update the state machine while open (called per-frame)
    bool Update();
    /// get current state
    const Ptr<State>& GetCurrentState() const;
    /// notify state machine about a message
    void Notify(const Ptr<Messaging::Message>& msg);
    /// get current debug txt
    Util::String GetDebugTxt() const;
    /// get debug color
    Math::float4 GetDebugColor() const;

private:
    /// perform a state switch
    void SetCurrentState(const Ptr<State>& s);
    /// clear current state
    void ClearCurrentState();
    /// push a state on the history stack
    void PushState(const Ptr<State>& state);
    /// pop a state from the history stack
    Ptr<State> PopState();

    bool isOpen;
    Ptr<Game::Entity> entity;
    Util::String name;
    Util::Array<Ptr<State> > states;
    Ptr<State> currentState;
    static const int MaxHistorySize = 8;
    Util::Array<Ptr<State> > stateHistory;    // history of previously active states, clamped at MaxHistorySize
};
__RegisterClass(StateMachine)

//------------------------------------------------------------------------------
/**
*/
inline
bool
StateMachine::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
StateMachine::SetEntity(const Ptr<Game::Entity>& e)
{
    n_assert(0 != e);
    this->entity = e;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Ptr<Game::Entity>&
StateMachine::GetEntity() const
{
    return this->entity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
StateMachine::SetName(const Util::String& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
StateMachine::GetName() const
{
    return this->name;
}

}; // namespace FSM
//------------------------------------------------------------------------------