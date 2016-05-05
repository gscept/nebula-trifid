
//  physicsfeature/properties/physicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsfeature/properties/physicsproperty.h"
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

__ImplementClass(PhysicsFeature::PhysicsProperty, 'PPR2', TransformableProperty);

//------------------------------------------------------------------------------
/**
*/
PhysicsProperty::PhysicsProperty():
    enabled(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PhysicsProperty::~PhysicsProperty()
{
    n_assert(!this->physicsEntity.isvalid());
}

//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. This will create and setup
    the required physics entities.
*/
void
PhysicsProperty::OnActivate()
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
PhysicsProperty::OnDeactivate()
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
PhysicsProperty::OnBeginFrame()
{
	if(this->IsEnabled() && this->physicsEntity.isvalid())
	{
		this->physicsEntity->OnFrameBefore();
	}
}

//------------------------------------------------------------------------------
/**
    Get pointer to physics entity. Note that this method may return 0!
*/
Ptr<PhysicsBody>
PhysicsProperty::GetPhysicsBody() const
{
    return this->physicsEntity.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, MoveAfter);
	this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/** 
    Called after the physics subsystem has been triggered. This will transfer
    the physics entity's new transform back into the game entity.
*/
void
PhysicsProperty::OnMoveAfter()
{
	if (this->IsSimulationHost())
	{
		this->UpdateFromPhysics();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(SetTransform::Id);
    this->RegisterMessage(ApplyImpulseAtPos::Id);
	this->RegisterMessage(SetLinearVelocity::Id);	
	this->RegisterMessage(SetKinematic::Id);
	this->RegisterMessage(EnableCollisionCallback::Id);	
	this->RegisterMessage(SetCollideCategory::Id);
	this->RegisterMessage(SetCollideCategoryMask::Id);
	this->RegisterMessage(Collision::Id);
	this->RegisterMessage(GetPhysicsObject::Id);
    TransformableProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(0 != msg);
    if (this->IsEnabled())
    {
		if (msg->CheckId(SetTransform::Id))
		{
			// set transform of physics entity
			SetTransform* transformMsg = (SetTransform*) msg.get();
			this->GetPhysicsBody()->SetTransform(transformMsg->GetMatrix());
		}
		if (msg->CheckId(GetPhysicsObject::Id))
		{			
			msg.cast<GetPhysicsObject>()->SetObject(this->GetPhysicsBody().cast<PhysicsObject>());		
			msg->SetHandled(true);
		}
		else if (msg->CheckId(ApplyImpulseAtPos::Id))
		{
			// apply an impulse to the physics entity
			Ptr<PhysicsFeature::ApplyImpulseAtPos> impulseMsg = msg.downcast<PhysicsFeature::ApplyImpulseAtPos>();
			this->ApplyImpulseAtPos(impulseMsg->GetImpulse(), impulseMsg->GetPosition(), impulseMsg->GetMultiplyByMass());
		}
		else if (msg->CheckId(SetLinearVelocity::Id))
		{
			Ptr<PhysicsFeature::SetLinearVelocity> velMsg = msg.downcast<PhysicsFeature::SetLinearVelocity>();
			this->GetPhysicsBody()->SetLinearVelocity(velMsg->GetVelocity());
		}		
		else if (msg->CheckId(SetKinematic::Id))
		{
			Ptr<PhysicsFeature::SetKinematic> velMsg = msg.downcast<PhysicsFeature::SetKinematic>();
			this->GetPhysicsBody()->SetKinematic(velMsg->GetEnabled());
		}
 		else if (msg->CheckId(EnableCollisionCallback::Id))
 		{
 			Ptr<PhysicsFeature::EnableCollisionCallback> velMsg = msg.downcast<PhysicsFeature::EnableCollisionCallback>(); 			
 				this->GetPhysicsBody()->SetEnableCollisionCallback(velMsg->GetEnableCallback()); 			
 		}

		else if (msg->CheckId(Collision::Id))
		{
			
		}		
		else if (msg->CheckId(SetCollideCategory::Id))
		{
			Ptr<PhysicsFeature::SetCollideCategory> velMsg = msg.downcast<PhysicsFeature::SetCollideCategory>();
			this->GetPhysicsBody()->SetCollideCategory(velMsg->GetCategory());
		}
		else if (msg->CheckId(SetCollideCategoryMask::Id))
		{
			Ptr<PhysicsFeature::SetCollideCategoryMask> velMsg = msg.downcast<PhysicsFeature::SetCollideCategoryMask>();
			this->GetPhysicsBody()->SetCollideFilter(velMsg->GetCategoryMask());
		}
    }
	
	// need to forward to parent
    TransformableProperty::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::EnablePhysics()
{
	n_assert(!this->IsEnabled());
	if (this->entity->HasAttr(Attr::Graphics)
		&& this->entity->GetString(Attr::Graphics).IsValid())
	{
		Util::String path;
		path.Format("physics:%s.np3",this->entity->GetString(Attr::Graphics).AsCharPtr());
		Ptr<ManagedPhysicsModel> model = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsModel::RTTI,path).cast<ManagedPhysicsModel>();
		Ptr<PhysicsObject> object;
		if(!model->GetModel()->HasObjects())
		{
			object = model->GetModel()->CreateDynamicInstance(this->entity->GetMatrix44(Attr::Transform));	
		}else
		{
			Util::Array<Ptr<Physics::PhysicsObject>>  objs = model->GetModel()->CreateInstance();	
			n_assert2(objs.Size() == 1, "tried to attach multiple bodies in physicsproperty");
			object = objs[0];
			object->GetTemplate().startTransform = this->entity->GetMatrix44(Attr::Transform);
		}		        

		PhysicsServer::Instance()->GetScene()->Attach(object);
		if (this->entity->HasAttr(Attr::Mass))
		{
			object.cast<Physics::PhysicsBody>()->SetMass(this->entity->GetFloat(Attr::Mass));
		}
        Physics::MaterialType mat = Physics::MaterialTable::StringToMaterialType(this->entity->GetString(Attr::PhysicMaterial));
        object->SetMaterialType(mat);

		this->physicsEntity = object.cast<PhysicsBody>();		
		this->physicsEntity->SetUserData(this->entity.cast<Core::RefCounted>());
		this->enabled = true;

		if(this->entity->HasAttr(Attr::CollisionFeedback))
		{
			this->physicsEntity->SetEnableCollisionCallback(this->entity->GetBool(Attr::CollisionFeedback));
		}
		
	}				

	if (!this->IsSimulationHost() || (this->entity->HasAttr(Attr::Kinematic) && this->entity->GetBool(Attr::Kinematic)))
	{
		this->GetPhysicsBody()->SetKinematic(true);
	}
	else
	{
		// apply small impulse down to earth
		this->ApplyImpulseAtPos(vector(0, 0.1, 0), point::origin());
	}
	
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::DisablePhysics()
{
    n_assert(this->IsEnabled());

	if (this->physicsEntity.isvalid())
	{
		PhysicsServer::Instance()->GetScene()->Detach(this->physicsEntity.cast<PhysicsObject>());
		this->physicsEntity = 0;
	}
    this->enabled = false;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::SetEnabled(bool setEnabled)
{
    if (this->enabled != setEnabled)
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


//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::SetSleeping(bool sleep)
{	
	this->physicsEntity->SetSleeping(sleep);	
}

//------------------------------------------------------------------------------
/**
*/

bool
PhysicsProperty::IsSleeping() const
{
	return this->physicsEntity->GetSleeping();    
}


// void 
// PhysicsProperty::doCollideEntity(Physics::PhysicsEntity* collidee, Ptr<Physics::Contact> contact)
// {
// 	Ptr<PhysicsFeature::CollidedWithEntity> msg = PhysicsFeature::CollidedWithEntity::Create();
// 	msg->SetOtherEntity(collidee);
// 	msg->SetContact(contact);
// 	this->entity->SendSync(msg.cast<Messaging::Message>());
// }
// 
// void PhysicsProperty::doCollideOther(Physics::Shape* collidee, Ptr<Physics::Contact> contact)
// {
// 	Ptr<PhysicsFeature::CollidedWithShape> msg = PhysicsFeature::CollidedWithShape::Create();
// 	msg->SetOtherShape(collidee);
// 	msg->SetContact(contact);
// 	this->entity->SendSync(msg.cast<Messaging::Message>());
// }
//------------------------------------------------------------------------------
/**
    Apply an impulse vector at a position in the global coordinate frame.
*/
void
PhysicsProperty::ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multByMass)
{
   
    this->physicsEntity->ApplyImpulseAtPos(impulse, pos, multByMass);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::UpdateFromPhysics()
{
	if (this->IsEnabled() && this->GetPhysicsBody()->HasTransformChanged())
	{
		Ptr<UpdateTransform> msg = UpdateTransform::Create();
		msg->SetMatrix(this->GetPhysicsBody()->GetTransform());
		this->entity->SendSync(msg.upcast<Messaging::Message>());
		this->entity->SetFloat4(Attr::VelocityVector, this->GetPhysicsBody()->GetLinearVelocity());
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
PhysicsProperty::IsSimulationHost()
{
	// check if we are networked at all
	if (MultiplayerFeature::MultiplayerFeatureUnit::HasInstance())
	{
		if (this->entity->HasAttr(Attr::_LevelEntity) && this->entity->GetBool(Attr::_LevelEntity))
		{
			// if we dont have a host we havent started a networked level yet and we just assume that we are the host for the time being
			if (MultiplayerFeature::NetworkServer::Instance()->HasHost())
			{
				return (MultiplayerFeature::NetworkServer::Instance()->IsHost());
			}
			else
			{
				return true;
			}
		}
		if (this->entity->HasAttr(Attr::IsMaster) && this->entity->GetBool(Attr::IsMaster))
		{
			// we own this networked entity, apply physics from simulation
			return true;
		}		
		else
		{
			// we are a slave apply transform from attribute to physics instead
			return false;			
		}
	}
	else
	{
		// no networking, just use normal physics behavior
		return true;
	}
}
}; // namespace Properties
