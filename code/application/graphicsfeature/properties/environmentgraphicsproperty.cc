//------------------------------------------------------------------------------
//  properties/environmentgraphicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/environmentgraphicsproperty.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "game/entity.h"

using namespace Util;
using namespace Math;

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::EnvironmentGraphicsProperty, 'PENV', Game::Property);

//------------------------------------------------------------------------------
/**
*/
EnvironmentGraphicsProperty::EnvironmentGraphicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
EnvironmentGraphicsProperty::~EnvironmentGraphicsProperty()
{
    n_assert(this->entries.Size() == 0);
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentGraphicsProperty::OnDeactivate()
{
    // remove graphics properties from level, and release the graphics entities
    Graphics::Stage* stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
    n_assert(stage);
    int entryIndex;
    int num = this->entries.Size();
    for (entryIndex = 0; entryIndex < num; entryIndex++)
    {
        const Entry& entry = this->entries.ValueAtIndex(entryIndex);
        IndexT entityIndex;
        for (entityIndex = 0; entityIndex < entry.graphicsEntities.Size(); entityIndex++)
        {
            stage->RemoveEntity(entry.graphicsEntities[entityIndex].upcast<Graphics::GraphicsEntity>());
        }
    }
    this->entries.Clear();
    Game::Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentGraphicsProperty::AddGraphicsEntities(const String& id, const matrix44& worldMatrix, const Array<Ptr<Graphics::ModelEntity> >& gfxEntities)
{
    n_assert(id.IsValid());

    // add to dictionary (prevent excessive copying)
    Entry newEntry;
    this->entries.Add(id, newEntry);    
    Entry& entry = this->entries[id];
    entry.graphicsEntities = gfxEntities;

    // compute local matrices, this is necessary if the transform of the
    // entities should be modified afterwards relativ to its origin
    matrix44 invWorldMatrix = matrix44::inverse(worldMatrix);    
    IndexT i;
    for (i = 0; i < entry.graphicsEntities.Size(); i++)
    {
        matrix44 localMatrix = matrix44::multiply(entry.graphicsEntities[i]->GetTransform(), invWorldMatrix);
        entry.localMatrices.Append(localMatrix);
    }

    // add graphics entities to world
    const Ptr<Graphics::Stage>& stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
    n_assert(stage);
    IndexT entityIndex;
    for (entityIndex = 0; entityIndex < gfxEntities.Size(); entityIndex++)
    {
        stage->AttachEntity(gfxEntities[entityIndex].upcast<Graphics::GraphicsEntity>());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EnvironmentGraphicsProperty::DeleteGraphicsEntities(const String& id)
{
    n_assert(id.IsValid());
    n_assert(this->HasGraphicsEntities(id));

    // remove graphics entities from world
    Graphics::Stage* stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
    IndexT i;
    for (i = 0; i < this->entries[id].graphicsEntities.Size(); i++)
    {
        stage->RemoveEntity(this->entries[id].graphicsEntities[i].upcast<Graphics::GraphicsEntity>());
    }

    // remove entry
    this->entries.Erase(id);
}

//------------------------------------------------------------------------------
/**
*/
bool 
EnvironmentGraphicsProperty::HasGraphicsEntities(const String& id) const
{
    n_assert(id.IsValid());
    return this->entries.Contains(id);
}

//------------------------------------------------------------------------------
/**
*/
const Array<Ptr<Graphics::ModelEntity> >&
EnvironmentGraphicsProperty::GetGraphicsEntities(const String& id) const
{
    n_assert(id.IsValid());
    n_assert(this->entries.Contains(id));
    return this->entries[id].graphicsEntities;
}

//------------------------------------------------------------------------------
/**
*/
const Array<matrix44>&
EnvironmentGraphicsProperty::GetLocalMatrices(const String& id) const
{
    n_assert(id.IsValid());
    n_assert(this->entries.Contains(id));
    return this->entries[id].localMatrices;
}

}; // namespace GraphicsFeature
