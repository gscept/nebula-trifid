#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokChainJoint
    
    A Havok motor-powered constraint chain.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
#include "physics/joints/basechainjoint.h"

#include <Physics2012/Dynamics/Constraint/Chain/hkpConstraintChainInstance.h>
#include <Physics2012/Dynamics/Constraint/Chain/Powered/hkpPoweredChainData.h>
#include <Physics/Constraint/Motor/Position/hkpPositionConstraintMotor.h>
//------------------------------------------------------------------------------

namespace Physics
{
	class BaseScene;
	class PhysicsBody;
}

namespace Havok
{
class HavokChainJoint : public Physics::BaseChainJoint
{
	__DeclareClass(HavokChainJoint);
public:
	struct SubJoint;

	/// constructor
	HavokChainJoint();
	/// destructor
	virtual ~HavokChainJoint();

	/// set up the chain
	void Setup();
	/// set up the chain with the first physics body
	void Setup(const Ptr<Physics::PhysicsBody>& body);
	/// set up the chain with the first physics body
	void Setup(hkRefPtr<hkpRigidBody> body);

	/// enable or disable the motors for every subjoint, must attach to world before calling this (note: motors are enabled by default)
	void SetMotorsEnabled(bool flag);

	/// attach to scene
	virtual void Attach(Physics::BaseScene * world);
	/// detach from scene
	virtual void Detach();
	/// returns true if this has been added to a scene
	virtual bool IsAttached();

	/// append a new joint, with a target rotation of body B relative to A, and with new position motors
	virtual void AppendJoint(const Math::point& pivotInA, const Math::point& pivotInB, const Math::quaternion& targetRotation);
	/// append a new joint, with a target rotation of body B relative to A, and with a given set of motors
	virtual void AppendJoint(const Math::point& pivotInA, const Math::point& pivotInB, const Math::quaternion& targetRotation, hkRefPtr<hkpPositionConstraintMotor> motorX, hkRefPtr<hkpPositionConstraintMotor> motorY, hkRefPtr<hkpPositionConstraintMotor> motorZ);
	/// append a new physicsbody
	virtual void AppendPhysicsBody(const Ptr<Physics::PhysicsBody>& body);
	/// append a new physicsbody
	virtual void AppendPhysicsBody(hkRefPtr<hkpRigidBody> body);

	/// set the target relative rotation of a joint
	void SetTargetRelativeRotation(const IndexT& i, const Math::quaternion& rotation);

	// note: the solver only uses some of these for the linear parts and not angular, see hkpPoweredChainData.h
	void  SetStiffness(float tau);
	float GetStiffness();
	void  SetDamping(float damping);
	float GetDamping();
	void  SetMaxErrorDistance(float maxDistance);
	float GetMaxErrorDistance();
	
	// constraint force mixing parameters
	void  SetCFMLinearAddend(float addend);
	float GetCFMLinearAddend();
	void  SetCFMLinearFactor(float factor);
	float GetCFMLinearFactor();
	void  SetCFMAngularAddend(float addend);
	float GetCFMAngularAddend();
	void  SetCFMAngularFactor(float factor);
	float GetCFMAngularFactor();

	/// get the number of joints this chain consists of
	virtual int GetNumJoints();

	/// get an individual joint at index
	SubJoint* GetSubJoint(IndexT i);
	/// get a body at index
	hkRefPtr<hkpRigidBody> GetBodyPart(IndexT i);

	struct SubJoint
	{
		/*	note: for whatever reason it won't work to keep a reference or a pointer to a
			hkpPoweredChainData::ConstraintInfo from the parents hkpPoweredChainData, becase 
			then editing of the info will only have an effect if it is the last info in the 
			array - therefore the current solution is to keep reference to the parent 
			HavokChainJoint and the constraint info index, and manage the constraints via that
		*/

		/// constructor
		SubJoint(HavokChainJoint* p, IndexT infoIdx);

		/// set the target rotation of body B relative to A
		void SetTargetRelativeRotation(const Math::quaternion& rotation);
		/// get the havok constraint info, read only
		const hkpPoweredChainData::ConstraintInfo* GetConstraintInfo();
		/// get body A, read only
		const hkRefPtr<hkpRigidBody> GetBodyA();
		/// get body B, read only
		const hkRefPtr<hkpRigidBody> GetBodyB();

		/// set a motor for an axis
		void SetMotor(IndexT axisIndex, hkRefPtr<hkpConstraintMotor> motor);
		/// get a motor for an axis
		hkRefPtr<hkpConstraintMotor> GetMotor(IndexT axisIndex);

		/// get the parent joint chain
		HavokChainJoint* GetParentChain() { return this->parent; }

	private:
		HavokChainJoint* parent;
		IndexT infoIndex;
	};

protected:
	friend struct SubJoint;

	Util::Array<SubJoint*> subJoints; //< keep the hkpPoweredChainData::ConstraintInfo in an array of nebula-wrapped objects

	hkRefPtr<hkpPoweredChainData> chainData;
	hkRefPtr<hkpConstraintChainInstance> chainInstance;
	hkRefPtr<hkpWorld> world;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
HavokChainJoint::SetStiffness(float tau)
{
	n_assert(HK_NULL != this->chainData);
	this->chainData->m_tau = tau;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokChainJoint::GetStiffness()
{
	n_assert(HK_NULL != this->chainData);
	return this->chainData->m_tau;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokChainJoint::SetDamping(float damping)
{
	n_assert(HK_NULL != this->chainData);
	this->chainData->m_damping = damping;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokChainJoint::GetDamping()
{
	n_assert(HK_NULL != this->chainData);
	return this->chainData->m_damping;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokChainJoint::SetMaxErrorDistance(float maxDistance)
{
	n_assert(HK_NULL != this->chainData);
	this->chainData->m_maxErrorDistance = maxDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokChainJoint::GetMaxErrorDistance()
{
	n_assert(HK_NULL != this->chainData);
	return this->chainData->m_maxErrorDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokChainJoint::SetCFMLinearAddend(float addend)
{
	n_assert(HK_NULL != this->chainData);
	this->chainData->m_cfmLinAdd = addend;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokChainJoint::GetCFMLinearAddend()
{
	n_assert(HK_NULL != this->chainData);
	return this->chainData->m_cfmLinAdd;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokChainJoint::SetCFMLinearFactor(float factor)
{
	n_assert(HK_NULL != this->chainData);
	this->chainData->m_cfmLinMul = factor;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokChainJoint::GetCFMLinearFactor()
{
	n_assert(HK_NULL != this->chainData);
	return this->chainData->m_cfmLinMul;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokChainJoint::SetCFMAngularAddend(float addend)
{
	n_assert(HK_NULL != this->chainData);
	this->chainData->m_cfmAngAdd = addend;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokChainJoint::GetCFMAngularAddend()
{
	n_assert(HK_NULL != this->chainData);
	return this->chainData->m_cfmAngAdd;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokChainJoint::SetCFMAngularFactor(float factor)
{
	n_assert(HK_NULL != this->chainData);
	this->chainData->m_cfmAngMul = factor;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
HavokChainJoint::GetCFMAngularFactor()
{
	n_assert(HK_NULL != this->chainData);
	return this->chainData->m_cfmAngMul;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
HavokChainJoint::IsAttached()
{
	return HK_NULL != this->world;
}

} 
// namespace Havok
//------------------------------------------------------------------------------