//------------------------------------------------------------------------------
//  physics/stateentity.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/stateentity.h"
#include "physics/compositeloader.h"

namespace Physics
{
__ImplementClass(Physics::StateEntity, 'PSTE', Physics::PhysicsEntity);

//------------------------------------------------------------------------------
/**
*/
StateEntity::StateEntity() :
    activeStateIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StateEntity::~StateEntity()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
StateEntity::OnActivate()
{
    n_assert(!this->IsActive());
    n_assert(this->resourceName.IsValid());
    
    // check if we're already loaded
    if (this->composites.IsEmpty())
    {
        CompositeLoader compLoader;
        this->composites = compLoader.Load(this->resourceName);
        n_assert(this->composites.Size() > 0);
    }
    this->active = true;
    this->ActivateStateByIndex(0);

    // NOTE: DO NOT CALL PARENT CLASS
}

//------------------------------------------------------------------------------
/**
*/
int
StateEntity::FindStateIndex(const Util::String& stateName) const
{
    IndexT i;
    for (i = 0; i < this->composites.Size(); i++)
    {
        if (this->composites[i]->GetName() == stateName)
        {
            return i;
        }
    }
    // fallthrough: not found
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
void
StateEntity::ActivateStateByIndex(int index)
{
    if (this->activeStateIndex != index)
    {
        Math::matrix44 curTransform = PhysicsEntity::GetTransform();

        // this removes the currently active composite from the world
        this->SetAlive(false);

        // set new composite and attach it to the world
        this->SetComposite(this->composites[index]);
        this->SetAlive(true);

        this->composites[index]->SetTransform(curTransform);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StateEntity::ActivateStateByName(const Util::String& stateName)
{
    n_assert(this->HasState(stateName));
    this->ActivateStateByIndex(this->FindStateIndex(stateName));
}

} // namespace Physics