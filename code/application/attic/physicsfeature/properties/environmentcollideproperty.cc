//------------------------------------------------------------------------------
//  properties/environmentcollideproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/environmentcollideproperty.h"
#include "physics/physicsserver.h"
#include "physics/level.h"
#include "game/entity.h"

namespace PhysicsFeature
{
__ImplementPropertyClass(PhysicsFeature::EnvironmentCollideProperty, 'ENCO', Game::Property);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
EnvironmentCollideProperty::EnvironmentCollideProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentCollideProperty::~EnvironmentCollideProperty()
{
    n_assert(this->entries.Size() == 0);
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentCollideProperty::OnDeactivate()
{
    // remove collide shapes from physics level, and clear the collide shapes
    Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
    n_assert(physicsLevel);
    IndexT entryIndex;
    for (entryIndex = 0; entryIndex < this->entries.Size(); entryIndex++)
    {
        const Entry& entry = this->entries.ValueAtIndex(entryIndex);
        IndexT i;
        for (i = 0; i < entry.collideShapes.Size(); i++)
        {
            physicsLevel->RemoveShape(entry.collideShapes[i]);
        }
    }
    this->entries.Clear();

    // hand to parent class
    Game::Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentCollideProperty::AddShapes(const Util::String& id, const matrix44& worldMatrix, const Util::Array<Ptr<Physics::Shape> >& shapes)
{
    n_assert(id.IsValid());
    n_assert(!this->entries.Contains(id));
    
    // add to dictionary (prevent excessive copying)
    Entry newEntry;
    this->entries.Add(id, newEntry);    
    Entry& entry = this->entries[id];
    entry.collideShapes = shapes;

    // compute local matrices, this is necessary if the transform of the
    // shapes should be modified afterwards relativ to its origin
    matrix44 invWorldMatrix = Math::matrix44::inverse(worldMatrix);    
    IndexT i;
    for (i = 0; i < entry.collideShapes.Size(); i++)
    {
        Math::matrix44 localMatrix = Math::matrix44::multiply(entry.collideShapes[i]->GetTransform(), invWorldMatrix);
        entry.localMatrices.Append(localMatrix);
    }
    
    // add new shapes to world
    Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
    n_assert(physicsLevel);
    for (i = 0; i < shapes.Size(); i++)
    {
        physicsLevel->AttachShape(shapes[i]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentCollideProperty::DeleteShapes(const Util::String& id)
{
    n_assert(id.IsValid());
    n_assert(this->HasShapes(id));

    // remove shapes from world
    Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
    IndexT i;
    for (i = 0; i < this->entries[id].collideShapes.Size(); i++)
    {
        physicsLevel->RemoveShape(this->entries[id].collideShapes[i]);
    }

    // remove entry
    this->entries.Erase(id);
}

//------------------------------------------------------------------------------
/**
*/
bool
EnvironmentCollideProperty::HasShapes(const Util::String& id) const
{
    n_assert(id.IsValid());
    return this->entries.Contains(id);
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Ptr<Physics::Shape> >&
EnvironmentCollideProperty::GetShapes(const Util::String& id) const
{
    n_assert(id.IsValid());
    n_assert(this->entries.Contains(id));
    return this->entries[id].collideShapes;
}

//------------------------------------------------------------------------------
/**
*/
const Util::Array<Math::matrix44>&
EnvironmentCollideProperty::GetLocalMatrices(const Util::String& id) const
{
    n_assert(id.IsValid());
    n_assert(this->entries.Contains(id));
    return this->entries[id].localMatrices;
}

}; // namespace Properties
