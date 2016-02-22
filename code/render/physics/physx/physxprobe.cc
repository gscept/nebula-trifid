//------------------------------------------------------------------------------
//  physxprobe.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/physicsprobe.h"
#include "physics/collider.h"
#include "PxSimulationEventCallback.h"

using namespace Physics;
using namespace Math;

PxSimulationEventCallback sdaf;

namespace PhysX
{
__ImplementClass(PhysX::PhysXProbe, 'PXPR', Physics::BaseProbe);

//------------------------------------------------------------------------------
/**
*/

PhysXProbe::PhysXProbe()
{
    this->common.type = Physics::PhysicsProbe::RTTI.GetFourCC();
    this->common.category = SensorTrigger;
    this->common.material = InvalidMaterial;    
}

//------------------------------------------------------------------------------
/**
*/

PhysXProbe::~PhysXProbe()
{

}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Core::RefCounted>>
PhysXProbe::GetOverlappingObjects()
{

}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXProbe::Init(const Ptr<Collider> & coll, const Math::matrix44 & trans)
{
    this->common.collider = coll;
    this->common.startTransform = trans;

    Math::vector scale;
    Math::quaternion rotation;
    Math::float4 pos;
    trans.decompose(scale, rotation, pos);

    PxTransform pxStartTrans(Neb2PxVec(pos), Neb2PxQuat(rotation));
    this->body = PhysXServer::Instance()->physics->createRigidStatic(pxStartTrans);
    PxMaterial * mat;    
    mat = PhysXServer::Instance()->GetMaterial(InvalidMaterial);
    
    this->common.collider.cast<PhysXCollider>()->CreateInstance(this->body, scale, *mat);
    PxShape * shape;
    this->body->getShapes(&shape, 1);
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
    this->body->userData = this;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXProbe::Attach(Physics::BaseScene * world)
{
    this->scene = ((PhysXScene*)world)->scene;
    this->scene->addActor(*this->body);
    this->attached = true;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXProbe::Detach()
{
    n_assert(this->attached);
    this->scene->removeActor(*this->body);
}

}