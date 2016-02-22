//------------------------------------------------------------------------------
//  physxstatic.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/collider.h"
#include "physics/staticobject.h"
#include "foundation/PxTransform.h"
#include "PxMaterial.h"
#include "physxphysicsserver.h"
#include "physxcollider.h"
#include "PxRigidStatic.h"
#include "math/quaternion.h"
#include "math/float4.h"
#include "math/vector.h"
#include "physxutils.h"
#include "PxPhysics.h"
#include "PxScene.h"

using namespace Physics;
using namespace Math;
using namespace physx;

namespace PhysX
{
__ImplementClass(PhysX::PhysXStatic, 'PXBS', Physics::BaseStatic);


//------------------------------------------------------------------------------
/**
*/

PhysXStatic::PhysXStatic():
    body(NULL),
    scene(NULL)
{
    this->common.type = Physics::StaticObject::RTTI.GetFourCC();
}

//------------------------------------------------------------------------------
/**
*/

PhysXStatic::~PhysXStatic()
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXStatic::SetupFromTemplate(const PhysicsCommon & templ)
{
    BaseStatic::SetupFromTemplate(templ);
    Math::vector scale;
    Math::quaternion rotation;
    Math::float4 pos;
    templ.startTransform.decompose(scale, rotation, pos);

    PxTransform pxStartTrans(Neb2PxVec(pos), Neb2PxQuat(rotation));
    this->body = PhysXServer::Instance()->physics->createRigidStatic(pxStartTrans);
    PxMaterial * mat;
    if (templ.material == InvalidMaterial)
    {
        mat = PhysXServer::Instance()->physics->createMaterial(templ.friction, templ.friction, templ.restitution);
    }
    else
    {
        mat = PhysXServer::Instance()->GetMaterial(templ.material);
    }
    templ.collider.cast<PhysXCollider>()->CreateInstance(this->body, scale, *mat);
    this->body->userData = this;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXStatic::Attach(Physics::BaseScene * world)
{
    this->scene = ((PhysXScene*)world)->scene;
    this->scene->addActor(*this->body);
    this->attached = true;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXStatic::Detach()
{
    n_assert(this->attached);
    this->scene->removeActor(*this->body);
}

}
