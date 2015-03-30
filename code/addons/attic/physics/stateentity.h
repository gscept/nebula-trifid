#ifndef PHYSICS_STATEENTITY_H
#define PHYSICS_STATEENTITY_H
//------------------------------------------------------------------------------
/**
    @class Physics::StateEntity
    
    A physics entity which can switch between different states (each
    state is a composite).
    
    (C) 2006 Radon Labs GmbH
*/
#include "physics/physicsentity.h"
#include "physics/composite.h"

//------------------------------------------------------------------------------
namespace Physics
{
class StateEntity :  public PhysicsEntity
{
    __DeclareClass(StateEntity);
public:
    /// constructor
    StateEntity();
    /// destructor
    virtual ~StateEntity();
    
    /// called when attached to game entity
    virtual void OnActivate();

    /// get number of states
    int GetNumStates() const;
    /// return true if a state exists by name
    bool HasState(const Util::String& stateName) const;
    /// get state composite by index
    Composite* GetStateCompositeByIndex(int index) const;
    /// get state composite by name
    Composite* GetStateCompositeByName(const Util::String& stateName) const;
    /// activate a new current state by index
    void ActivateStateByIndex(int index);
    /// activate a new current state by name
    void ActivateStateByName(const Util::String& stateName);
    /// get active state index
    int GetActiveStateIndex() const;
    /// get active state name
    const Util::String& GetActiveStateName() const;
    /// get active state composite
    Composite* GetActiveStateComposite() const;

private:
    /// find state index by name, returns -1 if not found
    int FindStateIndex(const Util::String& stateName) const;
    
    Util::Array<Ptr<Composite> > composites;
    int activeStateIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
StateEntity::GetNumStates() const
{
    return this->composites.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
StateEntity::HasState(const Util::String& stateName) const
{
    return (-1 != this->FindStateIndex(stateName));
}

//------------------------------------------------------------------------------
/**
*/
inline
Composite*
StateEntity::GetStateCompositeByIndex(int index) const
{
    return this->composites[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
Composite*
StateEntity::GetStateCompositeByName(const Util::String& stateName) const
{
    n_assert2(this->HasState(stateName), stateName.AsCharPtr());
    return this->composites[this->FindStateIndex(stateName)];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
StateEntity::GetActiveStateIndex() const
{
    n_assert(-1 != this->activeStateIndex);
    return this->activeStateIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
StateEntity::GetActiveStateName() const
{
    n_assert(-1 != this->activeStateIndex);
    return this->composites[this->activeStateIndex]->GetName();
}

//------------------------------------------------------------------------------
/**
*/
inline
Composite*
StateEntity::GetActiveStateComposite() const
{
    n_assert(-1 != this->activeStateIndex);
    return this->composites[this->activeStateIndex];
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
