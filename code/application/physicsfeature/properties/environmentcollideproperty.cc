//------------------------------------------------------------------------------
//  properties/environmentcollideproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"
#include "physicsfeature/properties/environmentcollideproperty.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "physics/staticobject.h"
#include "game/entity.h"
#include "io/filestream.h"
#include "resources/resource.h"
#include "physics/resource/managedphysicsmodel.h"


namespace PhysicsFeature
{

__ImplementClass(PhysicsFeature::EnvironmentCollideProperty, 'ENC2', Game::Property);

using namespace Math;
using namespace Physics;
using namespace Util;
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
    
    IndexT entryIndex;
    for (entryIndex = 0; entryIndex < this->entries.Size(); entryIndex++)
    {
        const Util::Array<Ptr<Physics::PhysicsObject>>& entry = this->entries.ValueAtIndex(entryIndex);
        IndexT i;
        for (i = 0; i < entry.Size(); i++)
        {
            PhysicsServer::Instance()->GetScene()->Detach(entry[i].cast<PhysicsObject>());
        }
    }
    this->entries.Clear();

    // hand to parent class
    Game::Property::OnDeactivate();
}

void 
EnvironmentCollideProperty::AddShapes(const Util::String& id, const Math::matrix44& worldMatrix, const Util::String & resourceName, Util::String physicsMaterial)
{
	Util::String path;
	path.Format("physics:%s.np3",resourceName.AsCharPtr());
	Ptr<ManagedPhysicsModel> model = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsModel::RTTI,path).cast<ManagedPhysicsModel>();
	Util::Array<Ptr<PhysicsObject>> objects = model->GetModel()->CreateStaticInstance(worldMatrix);	
	MaterialType materialType = MaterialTable::StringToMaterialType(physicsMaterial);
	IndexT i;
	for (i = 0; i < objects.Size(); i++)
	{
		objects[i]->SetMaterialType(materialType);
		PhysicsServer::Instance()->GetScene()->Attach(objects[i]);
		objects[i]->SetUserData(this->entity.cast<Core::RefCounted>());
	}
	this->entries.Add(id,objects);

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
    const Ptr<Scene>& physicsLevel = PhysicsServer::Instance()->GetScene();
    IndexT i;
    for (i = 0; i < this->entries[id].Size(); i++)
    {		
        physicsLevel->Detach(this->entries[id][i].cast<PhysicsObject>());
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
const Util::Array<Ptr<PhysicsObject> >&
EnvironmentCollideProperty::GetShapes(const Util::String& id) const
{
    n_assert(id.IsValid());
    n_assert(this->entries.Contains(id));
    return this->entries[id];
}

}; // namespace Properties
