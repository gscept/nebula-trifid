//------------------------------------------------------------------------------
//  physicsfeature/properties/physicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/physicsproperty.h"
#include "game/entity.h"
#include "physics/physicsserver.h"
#include "physics/level.h"
#include "physicsattr/physicsattributes.h"
#include "core/factory.h"
#include "basegameprotocol.h"
#include "physicsprotocol.h"

namespace PhysicsFeature
{
using namespace Game;
using namespace Math;
using namespace BaseGameFeature;

__ImplementPropertyClass(PhysicsFeature::PhysicsProperty, 'PPRO', TransformableProperty);

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
    Get pointer to physics entity. Note that this method may return 0!
*/
Ptr<Physics::PhysicsEntity>
PhysicsProperty::GetPhysicsEntity() const
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
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsProperty::SetupDefaultAttributes()
{
    TransformableProperty::SetupDefaultAttributes();
    SetupAttr(Attr::VelocityVector);
}

//------------------------------------------------------------------------------
/** 
    Called after the physics subsystem has been triggered. This will transfer
    the physics entity's new transform back into the game entity.
*/
void
PhysicsProperty::OnMoveAfter()
{
    if (this->IsEnabled() && this->GetPhysicsEntity()->HasTransformChanged())
    {
        Ptr<UpdateTransform> msg = UpdateTransform::Create();
        msg->SetMatrix(this->GetPhysicsEntity()->GetTransform());
        this->entity->SendSync(msg.upcast<Messaging::Message>());
        this->entity->SetFloat4(Attr::VelocityVector, this->GetPhysicsEntity()->GetVelocity());
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
            this->GetPhysicsEntity()->SetTransform(transformMsg->GetMatrix());
        }
        else if (msg->CheckId(ApplyImpulseAtPos::Id))
        {
            // apply an impulse to the physics entity
            Ptr<PhysicsFeature::ApplyImpulseAtPos> impulseMsg = msg.downcast<PhysicsFeature::ApplyImpulseAtPos>();
            this->ApplyImpulseAtPos(impulseMsg->GetImpulse(), impulseMsg->GetPosition(), impulseMsg->GetMultiplyByMass());
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
    if (this->entity->HasAttr(Attr::Physics)
        && this->entity->GetString(Attr::Physics).IsValid())
    {
        if (this->physicsEntity == 0)
        {
            // create and setup physics entity
            this->physicsEntity = Physics::PhysicsEntity::Create();
            this->physicsEntity->SetResourceName(this->entity->GetString(Attr::Physics));
            this->physicsEntity->SetUserData(this->entity->GetUniqueId());
        }

        // attach physics entity to physics level
        this->physicsEntity->SetTransform(this->entity->GetMatrix44(Attr::Transform));
        Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
        n_assert(physicsLevel);
        physicsLevel->AttachEntity(this->physicsEntity);

        // apply small impulse down to earth
        this->ApplyImpulseAtPos(vector(0,0.1,0), point::origin());
    }

    this->enabled = true;
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
        // release the physics entity
        Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
        n_assert(physicsLevel);
        physicsLevel->RemoveEntity(this->physicsEntity);
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
    Apply an impulse vector at a position in the global coordinate frame.
*/
void
PhysicsProperty::ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multByMass)
{
    n_assert(this->GetPhysicsEntity() != 0);
    
    this->GetPhysicsEntity()->ApplyImpulseAtPos(impulse, pos, multByMass);
}
}; // namespace Properties
