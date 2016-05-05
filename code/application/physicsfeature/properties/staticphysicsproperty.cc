
//  physicsfeature/properties/staticphysicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsfeature/properties/staticphysicsproperty.h"
#include "game/entity.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "physics/physicsbody.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "core/factory.h"
#include "basegamefeature/basegameprotocol.h"
#include "physicsfeature/physicsprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "io/ioserver.h"
#include "io/xmlreader.h"
#include "resources/resourcemanager.h"
#include "graphicsfeature/graphicsattr/graphicsattributes.h"
#include "physics/resource/managedphysicsmodel.h"
#include "multiplayer/multiplayerfeatureunit.h"

namespace PhysicsFeature
{
using namespace Game;
using namespace Math;
using namespace BaseGameFeature;
using namespace Physics;
using namespace Util;

__ImplementClass(PhysicsFeature::StaticPhysicsProperty, 'SPPR', TransformableProperty);

//------------------------------------------------------------------------------
/**
*/
StaticPhysicsProperty::StaticPhysicsProperty()   
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
StaticPhysicsProperty::~StaticPhysicsProperty()
{
    n_assert(!this->physicsEntity.isvalid());
}

//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. This will create and setup
    the required physics entities.
*/
void
StaticPhysicsProperty::OnActivate()
{
    TransformableProperty::OnActivate();
    
    // activate physics by default
    this->EnablePhysics();	
}

//------------------------------------------------------------------------------
/** 
    Called when property is going to be removed from its game entity.
    This will release the physics entity owned by the game entity.
*/
void
StaticPhysicsProperty::OnDeactivate()
{
    if (this->IsEnabled())
    {
        this->DisablePhysics();
    }
    this->physicsEntity = 0;
    TransformableProperty::OnDeactivate();
}



//------------------------------------------------------------------------------
/**
*/
void
StaticPhysicsProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(SetTransform::Id);    
	this->RegisterMessage(EnableCollisionCallback::Id);	
	this->RegisterMessage(SetCollideCategory::Id);
	this->RegisterMessage(SetCollideCategoryMask::Id);
	this->RegisterMessage(Collision::Id);	
    TransformableProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
StaticPhysicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(0 != msg);
    if (this->IsEnabled())
    {
		if (msg->CheckId(SetTransform::Id))
		{
			// set transform of physics entity
			SetTransform* transformMsg = (SetTransform*) msg.get();
			this->physicsEntity->SetTransform(transformMsg->GetMatrix());
		}
		else if (msg->CheckId(EnableCollisionCallback::Id))
 		{
 			Ptr<PhysicsFeature::EnableCollisionCallback> velMsg = msg.downcast<PhysicsFeature::EnableCollisionCallback>(); 			
 				this->physicsEntity->SetEnableCollisionCallback(velMsg->GetEnableCallback());
 		}				
		else if (msg->CheckId(SetCollideCategory::Id))
		{
			Ptr<PhysicsFeature::SetCollideCategory> velMsg = msg.downcast<PhysicsFeature::SetCollideCategory>();
			this->physicsEntity->SetCollideCategory(velMsg->GetCategory());
		}
		else if (msg->CheckId(SetCollideCategoryMask::Id))
		{
			Ptr<PhysicsFeature::SetCollideCategoryMask> velMsg = msg.downcast<PhysicsFeature::SetCollideCategoryMask>();
			this->physicsEntity->SetCollideFilter(velMsg->GetCategoryMask());
		}
    }
	
	// need to forward to parent
    TransformableProperty::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
StaticPhysicsProperty::EnablePhysics()
{
	n_assert(!this->IsEnabled());
	if (this->entity->HasAttr(Attr::Graphics)
		&& this->entity->GetString(Attr::Graphics).IsValid())
	{
		Util::String path;
		path.Format("physics:%s.np3",this->entity->GetString(Attr::Graphics).AsCharPtr());
		Ptr<ManagedPhysicsModel> model = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsModel::RTTI,path).cast<ManagedPhysicsModel>();
		Ptr<PhysicsObject> object;

		object = model->GetModel()->CreateStaticInstance(this->entity->GetMatrix44(Attr::Transform))[0];	

		PhysicsServer::Instance()->GetScene()->Attach(object);
        Physics::MaterialType mat = Physics::MaterialTable::StringToMaterialType(this->entity->GetString(Attr::PhysicMaterial));
        object->SetMaterialType(mat);

		this->physicsEntity = object.cast<StaticObject>();		
		this->physicsEntity->SetUserData(this->entity.cast<Core::RefCounted>());
		
		if(this->entity->HasAttr(Attr::CollisionFeedback))
		{
			this->physicsEntity->SetEnableCollisionCallback(this->entity->GetBool(Attr::CollisionFeedback));
		}
		
	}						
}

//------------------------------------------------------------------------------
/**
*/
void
StaticPhysicsProperty::DisablePhysics()
{
    n_assert(this->IsEnabled());

	if (this->physicsEntity.isvalid())
	{
		PhysicsServer::Instance()->GetScene()->Detach(this->physicsEntity.cast<PhysicsObject>());
		this->physicsEntity = 0;
	}    
}

//------------------------------------------------------------------------------
/**
*/
void
StaticPhysicsProperty::SetEnabled(bool setEnabled)
{
    if (this->IsEnabled() != setEnabled)
    {
        if (setEnabled)
        {
            this->EnablePhysics();
        }
        else
        {
            this->DisablePhysics();
        }
    }
}

}; // namespace Properties
