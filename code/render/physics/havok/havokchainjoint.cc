//------------------------------------------------------------------------------
//  havokchainjoint.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokchainjoint.h"
#include "havokscene.h"
#include "havokbody.h"


namespace Havok
{
__ImplementClass(Havok::HavokChainJoint, 'HCHJ', Physics::BaseChainJoint);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
HavokChainJoint::HavokChainJoint()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
HavokChainJoint::~HavokChainJoint()
{
	this->chainData = HK_NULL;
	this->chainInstance = HK_NULL;

	IndexT i;
	for (i = 0; i < this->subJoints.Size(); i++)
	{
		n_delete(this->subJoints[i]);
	}
	this->subJoints.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::Setup()
{
	n_assert(!this->initialized);

	this->chainData = new hkpPoweredChainData();
	this->chainInstance = new hkpConstraintChainInstance(chainData);

	this->initialized = true;

	this->chainInstance->setUserData((hkUlong)this);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::Setup(hkRefPtr<hkpRigidBody> body)
{
	this->Setup();

	this->AppendPhysicsBody(body);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::Setup(const Ptr<Physics::PhysicsBody>& body)
{
	this->Setup(body->GetRigidBody());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::Attach(Physics::BaseScene * w)
{
	n_assert(this->initialized);
	n_assert(HK_NULL != this->chainInstance);

	this->world = ((HavokScene*)w)->GetWorld();
	this->world->addConstraint(this->chainInstance);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::Detach()
{
	n_assert(HK_NULL != this->world);

	this->world->removeConstraint(this->chainInstance);
	this->world = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::AppendJoint(const Math::point& pivotInA, const Math::point& pivotInB, const Math::quaternion& targetRotation)
{
	n_assert(this->initialized);

	// create new motors for the x, y, and z axis rotations
	hkRefPtr<hkpPositionConstraintMotor> motorX = n_new(hkpPositionConstraintMotor(0));
	hkRefPtr<hkpPositionConstraintMotor> motorY = n_new(hkpPositionConstraintMotor(0));
	hkRefPtr<hkpPositionConstraintMotor> motorZ = n_new(hkpPositionConstraintMotor(0));

	this->AppendJoint(pivotInA, pivotInB, targetRotation, motorX, motorY, motorZ);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::AppendJoint(const Math::point& pivotInA, const Math::point& pivotInB, const Math::quaternion& targetRotation, hkRefPtr<hkpPositionConstraintMotor> motorX, hkRefPtr<hkpPositionConstraintMotor> motorY, hkRefPtr<hkpPositionConstraintMotor> motorZ)
{
	n_assert(this->initialized);

	this->chainData->addConstraintInfoInBodySpace(Neb2HkFloat4(pivotInA), Neb2HkFloat4(pivotInB), Neb2HkQuaternion(targetRotation), motorX, motorY, motorZ);

	IndexT jointIndex = this->GetNumJoints() - 1;

	// wrap the new joint
	SubJoint* newJoint = n_new(SubJoint(this, jointIndex));
	this->subJoints.Append(newJoint);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::AppendPhysicsBody(const Ptr<Physics::PhysicsBody>& body)
{
	this->AppendPhysicsBody(body->GetRigidBody());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::AppendPhysicsBody(hkRefPtr<hkpRigidBody> body)
{
	n_assert(this->initialized);

	this->chainInstance->addEntity(body);
}

//------------------------------------------------------------------------------
/**
*/
int 
HavokChainJoint::GetNumJoints()
{
	return this->chainData->getNumConstraintInfos();
}

//------------------------------------------------------------------------------
/**
*/
HavokChainJoint::SubJoint* 
HavokChainJoint::GetSubJoint(IndexT i)
{
	return this->subJoints[i];
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::SetMotorsEnabled(bool enabled)
{
	n_assert(HK_NULL != this->world);

	IndexT i;
	for (i = 0; i < this->GetNumJoints(); i++)
	{
		enabled? this->chainData->enableMotor(this->chainInstance, i, 0): this->chainData->disableMotor(this->chainInstance, i, 0);
		enabled? this->chainData->enableMotor(this->chainInstance, i, 1): this->chainData->disableMotor(this->chainInstance, i, 1);
		enabled? this->chainData->enableMotor(this->chainInstance, i, 2): this->chainData->disableMotor(this->chainInstance, i, 2);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::SetTargetRelativeRotation(const IndexT& i, const Math::quaternion& rotation)
{
	n_assert(HK_NULL != this->chainData);

	this->chainData->m_infos[i].m_bTc = Neb2HkQuaternion(rotation);
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpRigidBody> 
HavokChainJoint::GetBodyPart(IndexT i)
{
	hkpEntity* entity = this->chainInstance->m_chainedEntities[i];
	hkRefPtr<hkpRigidBody> bodypart = static_cast<hkpRigidBody*>(entity);

	return bodypart;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::SubJoint::SetTargetRelativeRotation(const Math::quaternion& rotation)
{
	n_assert(NULL != this->parent);
	n_assert(InvalidIndex != this->infoIndex);

	this->parent->SetTargetRelativeRotation(this->infoIndex, rotation);
}

//------------------------------------------------------------------------------
/**
*/
HavokChainJoint::SubJoint::SubJoint(HavokChainJoint* p, IndexT infoIdx)
{
	n_assert(NULL != p);
	n_assert(InvalidIndex != infoIdx);

	this->parent = p;
	this->infoIndex = infoIdx;
}

//------------------------------------------------------------------------------
/**
*/
const hkpPoweredChainData::ConstraintInfo* 
HavokChainJoint::SubJoint::GetConstraintInfo()
{
	return &this->parent->chainData->m_infos[this->infoIndex];
}

//------------------------------------------------------------------------------
/**
*/
const hkRefPtr<hkpRigidBody> 
HavokChainJoint::SubJoint::GetBodyA()
{
	return static_cast<hkpRigidBody*>(this->parent->chainInstance->m_chainedEntities[this->infoIndex]);
}

//------------------------------------------------------------------------------
/**
*/
const hkRefPtr<hkpRigidBody> 
HavokChainJoint::SubJoint::GetBodyB()
{
	return static_cast<hkpRigidBody*>(this->parent->chainInstance->m_chainedEntities[this->infoIndex + 1]);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokChainJoint::SubJoint::SetMotor(IndexT axisIndex, hkRefPtr<hkpConstraintMotor> motor)
{
	n_assert(HK_NULL != motor);
	n_assert(0 <= axisIndex && 2 >= axisIndex);

	this->parent->chainData->m_infos[this->infoIndex].m_motors[axisIndex] = motor;
}

//------------------------------------------------------------------------------
/**
*/
hkRefPtr<hkpConstraintMotor> 
HavokChainJoint::SubJoint::GetMotor(IndexT axisIndex)
{
	n_assert(0 <= axisIndex && 2 >= axisIndex);

	return this->parent->chainData->m_infos[this->infoIndex].m_motors[axisIndex];
}

}