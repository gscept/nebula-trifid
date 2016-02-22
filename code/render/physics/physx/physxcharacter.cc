//------------------------------------------------------------------------------
//  physxcharacter.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/physx/physxcharacter.h"
#include "characterkinematic/PxCapsuleController.h"
#include "physics/physx/physxscene.h"
#include "characterkinematic/PxControllerManager.h"
#include "physxphysicsserver.h"

using namespace Physics;
using namespace Math;
using namespace physx;

namespace PhysX
{
__ImplementClass(PhysX::PhysXCharacter, 'PXCK', Physics::BaseCharacter);

//------------------------------------------------------------------------------
/**
*/
PhysXCharacter::PhysXCharacter():
	controller(NULL),
	jumpHeight(1.0f)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PhysXCharacter::~PhysXCharacter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::Attach(Physics::BaseScene* world)
{
	PxCapsuleControllerDesc desc;
	desc.height = this->height;
	desc.radius = this->radius;	
	desc.maxJumpHeight = this->jumpHeight;
	desc.material = PhysXServer::Instance()->GetMaterial(InvalidMaterial);
	desc.climbingMode = PxCapsuleClimbingMode::eEASY;
	desc.userData = this;
	this->controller = ((PhysXScene*)world)->controllerManager->createController(desc);	
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::Detach()
{
	this->controller->release();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetHeight(float height)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetCrouchingHeight(float height)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetRadius(float radius)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetShape(CharacterShape shape)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetMotionVector(const Math::vector& movement)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetMaxTraversableSlopeAngle(float angle)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetFallSpeed(float fallSpeed)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::Jump()
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetJumpSpeed(float jumpSpeed)
{

}

//------------------------------------------------------------------------------
/**
*/
bool
PhysXCharacter::OnGround()
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
Math::vector
PhysXCharacter::GetLinearVelocity()
{
	return Math::vector(0, 0, 0);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetMaxJumpHeight(float maxJumpHeight)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetCrouching(bool enable)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetMovementSpeed(float speed)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetMaxLinearAcceleration(float acceleration)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCharacter::SetVelocityGain(float gain, float airGain)
{

}

} // namespace PhysX