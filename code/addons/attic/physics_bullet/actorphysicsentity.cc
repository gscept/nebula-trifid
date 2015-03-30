#include "stdneb.h"
#include "physics/actorphysicsentity.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "BulletDynamics/Character/btKinematicCharacterController.h"

#include "Physics/level.h"
#include "conversion.h"
#include "physics/composite.h"

namespace Physics
{
__ImplementClass(Physics::ActorPhysicsEntity, 'APEN', Physics::PhysicsEntity);

//------------------------------------------------------------------------------
/**
*/
ActorPhysicsEntity::ActorPhysicsEntity() : radius(1.0f), height(1.0f)
{
	ghostObject = NULL;
	characterController = NULL;
}

//------------------------------------------------------------------------------
/**
*/
ActorPhysicsEntity::~ActorPhysicsEntity()
{

}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::CreateCharacterController()
{
	ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(Neb2BtM44Transform(transform));

	btConvexShape* capsule = new btCapsuleShape(radius,height);
	ghostObject->setCollisionShape(capsule);
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	characterController = new btKinematicCharacterController(ghostObject,capsule,0.35f);

	btRigidBody * body = n_new(btRigidBody(0,0,capsule));
	this->body->SetSizes(1,0);
	Ptr<RigidBody> rb = RigidBody::Create();
	rb->SetInitialTransform(transform);
	rb->SetRigidBody(body);
	rb->SetShape(capsule);
	rb->SetMass(0);
	ghostObject->setUserPointer(rb);
	this->body->AddBody(rb);								
	this->body->SetEntity(this);	
	loaded = true;
	rb->SetName("actorphysicsentity");
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity becomes active
    (i.e. when it is attached to a game entity).
*/
void
ActorPhysicsEntity::OnActivate()
{
    n_assert(!this->active);
   
	this->CreateCharacterController();

	PhysicsEntity::OnActivate();
}

//------------------------------------------------------------------------------
/**
    This method is called when the physics entity is attached to a
    physics level object.

    @param  level   pointer to a level object
*/
void
ActorPhysicsEntity::OnAttachedToLevel(Level* levl)
{
	PhysicsEntity::OnAttachedToLevel(levl);

	this->body->GetMasterBody()->SetCollideCategory(btBroadphaseProxy::CharacterFilter);
	level->GetBulletWorld()->addCollisionObject(ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter - btBroadphaseProxy::CharacterFilter);
	level->GetBulletWorld()->addAction(characterController);
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::SetTransform( const Math::matrix44& m )
{
	if (ghostObject)
	{
		ghostObject->setWorldTransform(Neb2BtM44Transform(m));
	}
	PhysicsEntity::SetTransform(m);
}

//------------------------------------------------------------------------------
/**
*/
Math::matrix44 ActorPhysicsEntity::GetTransform() const
{
	if (ghostObject)
	{
		return Bt2NebTransform(ghostObject->getWorldTransform());
	}
	else
	{
		return this->transform;
	}
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::OnMoveAfter()
{
	this->body->SetTransform(Bt2NebTransform(ghostObject->getWorldTransform()));
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::SetMotionVector( const Math::vector& movement )
{
	characterController->setWalkDirection(Neb2BtVector(movement));
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::SetFallSpeed( float fallSpeed )
{
	characterController->setFallSpeed(fallSpeed);
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::Jump()
{
	characterController->jump();
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::SetJumpSpeed( float jumpSpeed )
{
	characterController->setJumpSpeed(jumpSpeed);
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::SetMaxJumpHeight( float maxJumpHeight )
{
	characterController->setMaxJumpHeight(maxJumpHeight);
}

//------------------------------------------------------------------------------
/**
*/
bool ActorPhysicsEntity::OnGround()
{
	return characterController->onGround();
}

//------------------------------------------------------------------------------
/**
*/
void ActorPhysicsEntity::SetMaxTraversableSlopeAngle( float angle )
{
	characterController->setMaxSlope(angle);
}

} // namespace Physics