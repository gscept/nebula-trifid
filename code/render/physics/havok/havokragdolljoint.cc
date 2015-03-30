//------------------------------------------------------------------------------
//  havokragdolljoint.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokragdolljoint.h"
#include "physics/physicsbody.h"

using namespace Physics;

namespace Havok
{
__ImplementClass(Havok::HavokRagdollJoint, 'HKRJ', Physics::BaseRagdollJoint);

//------------------------------------------------------------------------------
/**
*/
HavokRagdollJoint::HavokRagdollJoint()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
HavokRagdollJoint::~HavokRagdollJoint()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::Setup(bool worldSpacePivot, const Ptr<PhysicsBody>& body1, const Ptr<PhysicsBody>& body2, const Math::vector& pivot, const Math::vector& twistAxis, const Math::vector& planeAxis)
{
	this->Setup(worldSpacePivot, body1->GetRigidBody(), body2->GetRigidBody(), pivot, twistAxis, planeAxis);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::Setup(bool worldSpacePivot, hkRefPtr<hkpRigidBody> body1, hkRefPtr<hkpRigidBody> body2, const Math::vector& pivot, const Math::vector& tAxis, const Math::vector& pAxis)
{
	n_assert(!this->initialized);
	n_assert(HK_REAL_EPSILON > Math::vector::dot3(tAxis, pAxis));	//< the given twist and plane must be perpendicular

	// default values
	hkReal planeVal = 0.5f;
	hkReal twistVal = 0.5f;
	hkReal coneVal = 0.5f;

	hkReal planeMin =  HK_REAL_PI * -planeVal;
	hkReal planeMax =  HK_REAL_PI * planeVal;
	hkReal twistMin =  HK_REAL_PI * -twistVal;
	hkReal twistMax =  HK_REAL_PI *  twistVal;
	hkReal coneLimit  =  HK_REAL_PI * coneVal;

	this->joint = n_new(hkpRagdollConstraintData());

	this->joint->setConeAngularLimit(coneLimit);
	this->joint->setPlaneMinAngularLimit(planeMin);
	this->joint->setPlaneMaxAngularLimit(planeMax);
	this->joint->setTwistMinAngularLimit(twistMin);
	this->joint->setTwistMaxAngularLimit(twistMax);

	this->twistAxis = tAxis;
	this->planeAxis = pAxis;
	this->coneAxis = Math::vector::cross3(this->twistAxis, this->planeAxis);

	if (worldSpacePivot)
	{
		this->joint->setInWorldSpace(body1->getTransform(), body2->getTransform(), Neb2HkFloat4(pivot), Neb2HkFloat4(tAxis), Neb2HkFloat4(pAxis));
	}
	else
	{
		//FIXME
		n_error("HavokRagdollJoint::Setup: Setting up joint in local space not implemented");
	}

	this->constraint = n_new(hkpConstraintInstance(body1, body2, this->joint));

	this->constraint->setUserData((hkUlong)this);

	this->initialized = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetupMotors()
{
	n_assert(this->initialized);
	n_assert(!this->motorInitialized);

	// set up new motors with default values
	hkRefPtr<hkpPositionConstraintMotor> twistMotor = n_new(hkpPositionConstraintMotor(0));
	hkRefPtr<hkpPositionConstraintMotor> planeMotor = n_new(hkpPositionConstraintMotor(0));
	hkRefPtr<hkpPositionConstraintMotor> coneMotor = n_new(hkpPositionConstraintMotor(0));

	this->ActivateMotors(twistMotor, planeMotor, coneMotor);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::ActivateMotors(hkRefPtr<hkpPositionConstraintMotor> twistMotor, hkRefPtr<hkpPositionConstraintMotor> planeMotor, hkRefPtr<hkpPositionConstraintMotor> coneMotor)
{
	n_assert(this->initialized);
	n_assert(!this->motorInitialized);

	this->motors.pushBack(twistMotor);
	this->motors.pushBack(planeMotor);
	this->motors.pushBack(coneMotor);

	this->joint->setTwistMotor(twistMotor); 
	this->joint->setPlaneMotor(planeMotor); 
	this->joint->setConeMotor(coneMotor); 
	this->joint->setMotorsActive(this->constraint, true);

	this->motorInitialized = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::Attach(Physics::BaseScene * w)
{
	n_assert(this->initialized);
	n_assert(HK_NULL != this->constraint);

	this->world = ((HavokScene*)w)->GetWorld();
	this->world->addConstraint(this->constraint);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::Detach()
{
	this->world->removeConstraint(this->constraint);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetConeAngularLimit() const
{
	n_assert(this->initialized);

	return this->joint->getConeAngularLimit();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetConeAngularLimit(float limit)
{
	n_assert(this->initialized);

	this->joint->setConeAngularLimit(limit);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetPlaneMinAngularLimit() const
{
	n_assert(this->initialized);

	return this->joint->getPlaneMinAngularLimit();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetPlaneMinAngularLimit(float limit)
{
	n_assert(this->initialized);

	this->joint->setPlaneMinAngularLimit(limit);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetPlaneMaxAngularLimit() const
{
	n_assert(this->initialized);

	return this->joint->getPlaneMaxAngularLimit();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetPlaneMaxAngularLimit(float limit)
{
	n_assert(this->initialized);

	this->joint->setPlaneMaxAngularLimit(limit);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetTwistMinAngularLimit() const
{
	n_assert(this->initialized);

	return this->joint->getTwistMinAngularLimit();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetTwistMinAngularLimit(float limit)
{
	n_assert(this->initialized);

	this->joint->setTwistMinAngularLimit(limit);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetTwistMaxAngularLimit() const
{
	n_assert(this->initialized);

	return this->joint->getTwistMaxAngularLimit();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetTwistMaxAngularLimit(float limit)
{
	n_assert(this->initialized);

	this->joint->setTwistMaxAngularLimit(limit);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMotorStiffness(MotorSelection sel) const
{
	n_assert(this->motorInitialized);
	n_assert(All != sel);

	return this->motors[(int)sel]->m_tau;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotorStiffness(float stiffness, MotorSelection sel)
{
	n_assert(this->motorInitialized);
	n_assert(stiffness >= 0.0f && stiffness <= 1.0f);

	if (All != sel)
	{
		this->motors[(int)sel]->m_tau = stiffness;
	}
	else
	{
		this->motors[0]->m_tau = stiffness;
		this->motors[1]->m_tau = stiffness;
		this->motors[2]->m_tau = stiffness;
	}
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMotorMaxForce(MotorSelection sel) const
{
	n_assert(this->motorInitialized);
	n_assert(All != sel);

	return this->motors[(int)sel]->m_maxForce;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotorMaxForce(float maxforce, MotorSelection sel)
{
	n_assert(this->motorInitialized);

	if (All != sel)
	{
		this->motors[(int)sel]->m_maxForce = maxforce;
	}
	else
	{
		this->motors[0]->m_maxForce = maxforce;
		this->motors[1]->m_maxForce = maxforce;
		this->motors[2]->m_maxForce = maxforce;
	}
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMotorConstantRecoverlyVelocity(MotorSelection sel) const
{
	n_assert(this->motorInitialized);
	n_assert(All != sel);

	return this->motors[(int)sel]->m_constantRecoveryVelocity;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotorConstantRecoverlyVelocity(float vel, MotorSelection sel)
{
	n_assert(this->motorInitialized);

	if (All != sel)
	{
		this->motors[(int)sel]->m_constantRecoveryVelocity = vel;
	}
	else
	{
		this->motors[0]->m_constantRecoveryVelocity = vel;
		this->motors[1]->m_constantRecoveryVelocity = vel;
		this->motors[2]->m_constantRecoveryVelocity = vel;
	}
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMotorProportionalRecoverlyVelocity(MotorSelection sel) const
{
	n_assert(this->motorInitialized);
	n_assert(All != sel);

	return this->motors[(int)sel]->m_proportionalRecoveryVelocity;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotorProportionalRecoverlyVelocity(float vel, MotorSelection sel)
{
	n_assert(this->motorInitialized);

	if (All != sel)
	{
		this->motors[(int)sel]->m_proportionalRecoveryVelocity = vel;
	}
	else
	{
		this->motors[0]->m_proportionalRecoveryVelocity = vel;
		this->motors[1]->m_proportionalRecoveryVelocity = vel;
		this->motors[2]->m_proportionalRecoveryVelocity = vel;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetTargetRelativeRotation(const Math::quaternion& rot)
{
	n_assert(this->motorInitialized);

	hkRotation rotation;
	rotation.set(Neb2HkQuaternion(rot));
	this->joint->setTargetRelativeOrientationOfBodies(rotation);
}


//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetTargetRotation(const Math::quaternion& rot)
{
	n_assert(this->motorInitialized);

	hkRotation rotation;
	rotation.set(Neb2HkQuaternion(rot));
	this->joint->setTarget(rotation);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMotorMinForce(MotorSelection sel) const
{
	n_assert(this->motorInitialized);
	n_assert(All != sel);

	return this->motors[(int)sel]->m_minForce;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotorMinForce(float minforce, MotorSelection sel)
{
	n_assert(this->motorInitialized);

	if (All != sel)
	{
		this->motors[(int)sel]->m_minForce = minforce;	
	}
	else
	{
		this->motors[0]->m_minForce = minforce;	
		this->motors[1]->m_minForce = minforce;	
		this->motors[2]->m_minForce = minforce;	
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotorMinMaxForce(float absValue, MotorSelection sel)
{
	this->SetMotorMaxForce(absValue, sel);
	this->SetMotorMinForce(-absValue, sel);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMotorDamping(MotorSelection sel) const
{
	n_assert(this->motorInitialized);
	n_assert(All != sel);

	return this->motors[(int)sel]->m_damping;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotorDamping(float damping, MotorSelection sel)
{
	n_assert(this->motorInitialized);
	n_assert(damping >= 0.0f && damping <= 1.0f);

	if (All != sel)
	{
		this->motors[(int)sel]->m_damping = damping;
	}
	else
	{
		this->motors[0]->m_damping = damping;
		this->motors[1]->m_damping = damping;
		this->motors[2]->m_damping = damping;
	}
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMaxFrictionTorque() const
{
	return this->joint->getMaxFrictionTorque();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMaxFrictionTorque(float maxTorque)
{
	this->joint->setMaxFrictionTorque(maxTorque);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetMaxLinearImpulse() const
{
	// when this equals HK_REAL_MAX (= 3.40282e+38f, currently for this version) there is no limit
	return this->joint->getMaxLinearImpulse();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMaxLinearImpulse(float maxImpulse)
{
	this->joint->setMaxLinearImpulse(maxImpulse);
}

//------------------------------------------------------------------------------
/**
*/
float 
HavokRagdollJoint::GetAngularLimitsTauFactor() const
{
	return this->joint->getAngularLimitsTauFactor();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetAngularLimitsTauFactor(float tauFactor)
{
	this->joint->setAngularLimitsTauFactor(tauFactor);
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokRagdollJoint::GetInertiaStabilizationFactor(float& outFactor) const
{
	return HK_SUCCESS == this->joint->getInertiaStabilizationFactor(outFactor);
}

//------------------------------------------------------------------------------
/**
*/
bool 
HavokRagdollJoint::SetInertiaStabilizationFactor(float factor)
{
	return HK_SUCCESS == this->joint->setInertiaStabilizationFactor(factor);
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpConstraintMotor> 
HavokRagdollJoint::GetMotor(MotorSelection sel)
{
	n_assert(this->motorInitialized);
	n_assert(All != sel);

	if (Twist == sel)	return this->joint->getTwistMotor();
	if (Plane == sel)	return this->joint->getPlaneMotor();
	if (Cone == sel)	return this->joint->getConeMotor();
	
	// fallback
	n_error("Invalid motor selection '%d'", (int)sel);
	return HK_NULL;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokRagdollJoint::SetMotor(MotorSelection sel, hkRefPtr<hkpConstraintMotor> m)
{
	n_assert(this->motorInitialized);

	if (All == sel || Twist == sel)	this->joint->setTwistMotor(m);
	if (All == sel || Plane == sel)	this->joint->setPlaneMotor(m);
	if (All == sel || Cone == sel)	this->joint->setConeMotor(m);
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpConstraintInstance> 
HavokRagdollJoint::GetConstraintInstance()
{
	return this->constraint;
}

}