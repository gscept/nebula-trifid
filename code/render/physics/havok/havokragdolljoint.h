#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokRagdollJoint
    
    A Havok ragdoll joint.

	What motors and the twist, plane, and cone axes are is explained in the 
	Havok user guide under 2.2.1.3 Constraints.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/joints/baseragdolljoint.h"

#include <Physics/Dynamics/Constraint/Bilateral/Ragdoll/hkpRagdollConstraintData.h>
#include <Physics/Dynamics/Constraint/Motor/Position/hkpPositionConstraintMotor.h>

namespace Physics
{
	class PhysicsBody;
	class BaseScene;
}

namespace Havok
{
class HavokRagdollJoint : public Physics::BaseRagdollJoint
{
	__DeclareClass(HavokRagdollJoint);
public:
	/// constructor
	HavokRagdollJoint();
	/// destructor
	~HavokRagdollJoint();

	/// set up the joint
	void Setup(bool worldSpacePivot, const Ptr<Physics::PhysicsBody>& body1, const Ptr<Physics::PhysicsBody>& body2, const Math::vector& pivot, const Math::vector& twistAxis, const Math::vector& planeAxis);
	/// set up the joint
	void Setup(bool worldSpacePivot, hkRefPtr<hkpRigidBody> body1, hkRefPtr<hkpRigidBody> body2, const Math::vector& pivot, const Math::vector& twistAxis, const Math::vector& planeAxis);
	/// set up and activate joint motors, requires that the joint has been set up
	void SetupMotors();
	/// activate motor usage with the given motors, requires that the joint has been set up
	void ActivateMotors(hkRefPtr<hkpPositionConstraintMotor> twistMotor, hkRefPtr<hkpPositionConstraintMotor> planeMotor, hkRefPtr<hkpPositionConstraintMotor> coneMotor);

	/// get a specific motor, requires that motors has been set up
	hkRefPtr<hkpConstraintMotor> GetMotor(MotorSelection sel);
	/// set a motor for an axis (NOTE/FIXME: this sets the motor to the joint directly, and does not update the "motors" array), requires that motors has been set up
	void SetMotor(MotorSelection sel, hkRefPtr<hkpConstraintMotor> m);

	/// attach to scene
	void Attach(Physics::BaseScene * world);
	/// detach from scene
	void Detach();

	float GetConeAngularLimit() const;
	void  SetConeAngularLimit(float limit);
	float GetPlaneMinAngularLimit() const;
	void  SetPlaneMinAngularLimit(float limit);
	float GetPlaneMaxAngularLimit() const;
	void  SetPlaneMaxAngularLimit(float limit);
	float GetTwistMinAngularLimit() const;
	void  SetTwistMinAngularLimit(float limit);
	float GetTwistMaxAngularLimit() const;
	void  SetTwistMaxAngularLimit(float limit);

	Math::vector GetTwistAxis() const;
	Math::vector GetPlaneAxis() const;
	Math::vector GetConeAxis() const;

	float GetMaxFrictionTorque() const;
	void  SetMaxFrictionTorque(float maxTorque);
	float GetMaxLinearImpulse() const;
	void  SetMaxLinearImpulse(float maxImpulse);
	float GetAngularLimitsTauFactor() const;
	void  SetAngularLimitsTauFactor(float tauFactor);
	// returns true if the factor is defined for the constraint
	bool  GetInertiaStabilizationFactor(float& outFactor) const;
	bool  SetInertiaStabilizationFactor(float factor);

	// motor-related methods
	// NOTE/FIXME: currently these methods only refer to the default motors (meaning those in the "motors" array), so if you've exchanged any axis motor the old ones will still be accessed when calling these
	float GetMotorStiffness(MotorSelection sel) const;	//< tau
	void  SetMotorStiffness(float stiffness, MotorSelection sel);
	float GetMotorDamping(MotorSelection sel) const;
	void  SetMotorDamping(float damping, MotorSelection sel);
	float GetMotorMaxForce(MotorSelection sel) const;
	void  SetMotorMaxForce(float maxforce, MotorSelection sel);
	float GetMotorMinForce(MotorSelection sel) const;
	void  SetMotorMinForce(float minforce, MotorSelection sel);
	/// set min and max value to -absValue and absValue respectively
	void  SetMotorMinMaxForce(float absValue, MotorSelection sel);
	float GetMotorConstantRecoverlyVelocity(MotorSelection sel) const;
	void  SetMotorConstantRecoverlyVelocity(float vel, MotorSelection sel);
	float GetMotorProportionalRecoverlyVelocity(MotorSelection sel) const;
	void  SetMotorProportionalRecoverlyVelocity(float vel, MotorSelection sel);
	void  SetTargetRotation(const Math::quaternion& rot);
	void  SetTargetRelativeRotation(const Math::quaternion& rot);

	/// get havok constraint
	hkRefPtr<hkpConstraintInstance> GetConstraintInstance();

private:

	Math::vector twistAxis, planeAxis, 
		coneAxis;	//< cone = twist crossed with plane axis

	hkArray<hkRefPtr<hkpPositionConstraintMotor>> motors;
	hkRefPtr<hkpRagdollConstraintData> joint;
	hkRefPtr<hkpConstraintInstance> constraint;	//< this is what is added to the physicsworld

	hkRefPtr<hkpWorld> world;
}; 


//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
HavokRagdollJoint::GetTwistAxis() const
{
	return this->twistAxis;
}

//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
HavokRagdollJoint::GetPlaneAxis() const
{
	return this->planeAxis;
}
//------------------------------------------------------------------------------
/**
*/
inline Math::vector 
HavokRagdollJoint::GetConeAxis() const
{
	return this->coneAxis;
}

} 
// namespace Havok
//------------------------------------------------------------------------------