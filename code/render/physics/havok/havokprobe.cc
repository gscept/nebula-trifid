//------------------------------------------------------------------------------
//  havokprobe.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/physicsprobe.h"
#include "physics/scene.h"
#include "havokutil.h"
#include "havokdebugdrawer.h"
#include "havokvisualdebuggerserver.h"

#include <Physics2012/Dynamics/Phantom/hkpSimpleShapePhantom.h>
#include <Physics2012/Collide/Query/Collector/BodyPairCollector/hkpAllCdBodyPairCollector.h>
#include <Geometry/Collide/Shapes/hkcdShape.h>
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Collide/Shape/Convex/Sphere/hkpSphereShape.h>

namespace Havok
{
using namespace Physics;
using namespace Math;

__ImplementClass(Havok::HavokProbe,'HKPE', Physics::BaseProbe);

//------------------------------------------------------------------------------
/**
*/
HavokProbe::HavokProbe():
	phantom(HK_NULL),
	hasOverlappingObjects(false)
{
	this->common.type = Physics::PhysicsProbe::RTTI.GetFourCC();
}

//------------------------------------------------------------------------------
/**
*/
HavokProbe::~HavokProbe()
{
	if(this->attached)
	{
		Detach();
	}
	this->common.collider = 0;
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Core::RefCounted>> 
HavokProbe::GetOverlappingObjects()
{
	Util::Array<Ptr<Core::RefCounted>> result;

	hkpAllCdBodyPairCollector collector;

	this->phantom->ensureDeterministicOrder();
	this->phantom->getPenetrations(collector);

	const hkArray<hkpRootCdBodyPair>& hits = collector.getHits();

	IndexT i;
	for (i = 0; i < hits.getSize(); i++)
	{
		const hkpRootCdBodyPair& bodyPair = hits[i];
		const hkpCollidable* bodyA = bodyPair.m_rootCollidableA;
		const hkpCollidable* bodyB = bodyPair.m_rootCollidableB;

		void* ownerA = bodyA->getOwner();
		void* ownerB = bodyB->getOwner();

		hkpWorldObject* entityA = (hkpWorldObject*)ownerA;
		hkpWorldObject* entityB = (hkpWorldObject*)ownerB;

		n_assert(HK_NULL != entityA);
		n_assert(HK_NULL != entityB);

		hkUlong userDataA = entityA->getUserData();
		hkUlong userDataB = entityB->getUserData();

		HavokUtil::CheckWorldObjectUserData(userDataA);
		HavokUtil::CheckWorldObjectUserData(userDataB);

		Ptr<PhysicsObject> physObjectA = (PhysicsObject*)userDataA;
		Ptr<PhysicsObject> physObjectB = (PhysicsObject*)userDataB;

		Ptr<PhysicsObject> other = (this == physObjectA)? physObjectB: physObjectA;

		if (other->GetUserData()->object.isvalid())
		{
			result.Append(other->GetUserData()->object);
		}
	}

	this->hasOverlappingObjects = result.Size() > 0;

	return result;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokProbe::Attach(Physics::BaseScene* inWorld)
{
	n_assert2(this->common.collider.isvalid(), "No collider object attached");
	n_assert2(this->common.collider->GetShapes().getSize() == 1, "There must be only one shape added to the probe collider!");

	this->world = ((HavokScene*)inWorld)->GetWorld();

	// since the transforms of havok objects doesn't hold scale, we need to create a new shape with the desired dimensions
	const hkpShape* tmpShape = this->common.collider->GetShapes()[0];
	const hkpShape* targetShape = HavokCollider::GetShapeFromTransformShape(tmpShape);	

	// use the scale component to create the new shape, then store only the position and rotation
	this->common.startTransform.get_scale(this->halfSize);
	this->halfSize *= float4(0.5f, 0.5f, 0.5f, 1);

	quaternion rotation = matrix44::rotationmatrix(this->common.startTransform);
	float4 position = this->common.startTransform.get_position();

	this->transform = matrix44::rotationquaternion(rotation);
	this->transform.set_position(position);

	// then create the shape for the phantom
	hkpShapeType shapeType = targetShape->getType();

	this->shape = HK_NULL;
	if (hkcdShapeType::BOX == shapeType)
	{
		hkVector4 havokScale = Neb2HkFloat4(this->halfSize);
		shape = n_new(hkpBoxShape(havokScale));
	}
	else
	{
		n_assert(hkcdShapeType::SPHERE == shapeType);

		float radius = this->halfSize.x();
		shape = n_new(hkpSphereShape(radius));
	}

	this->phantom = n_new(hkpSimpleShapePhantom(shape, NebMatrix442HkTransform(this->transform), /*FIXME*/(int)n_log2(Physics::SensorTrigger)));
	this->phantom->setUserData((hkUlong)this);

	this->world->addPhantom(this->phantom);
	this->attached = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokProbe::Detach()
{
	this->world->removePhantom(this->phantom);
	n_delete(this->phantom);
	this->phantom = 0;

	this->attached = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokProbe::SetCollideFilter(uint mask)
{
#if _DEBUG
	n_error(
#else
	n_warning(
#endif
		"HavokProbe::SetCollideFilter: Can not set collide filter directly on a havok-physobject! Instead you must update the collidefilter in the scene");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokProbe::SetCollideCategory(Physics::CollideCategory coll)
{
	n_assert(this->phantom);

	PhysicsObject::SetCollideCategory(coll);
	n_error("FIXME: Cannot set the collide category directly to the phantom, only when it is created! Must remember the collidecategory-param and recreate the phantom");

	//this->phantom->getCollidable()->setCollisionFilterInfo(
	//	//TODO: blä, fix CollideCategory
	//	(int)(((int)coll > 1)? n_log2((Math::scalar)coll): coll));

	if (this->world)
	{
		this->world->updateCollisionFilterOnPhantom(this->phantom, 
			HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokProbe::SetTransform(const Math::matrix44 & trans)
{
	this->phantom->setTransform(NebMatrix442HkTransform(trans));
	this->transform = trans;
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokProbe::RenderDebug()
{
	float4 scale;
	this->transform.get_scale(scale);

	// the transform cannot be scaled, the shape has the correct dimensions
	n_assert(1.0f == scale.x() && 1.0f == scale.y() && 1.0f == scale.z());

	PhysicsObject::RenderDebug();

	const float4 color = this->hasOverlappingObjects? float4(0.5f, 0.5f, 1, 0.2f): float4(0, 0, 1, 0.2f);
	HavokDebugDrawer::DrawShape(this->shape, NebMatrix442HkTransform(this->transform), color);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokProbe::OnStepBefore()
{
	if (HavokVisualDebuggerServer::HasInstance())
	{
		//FIXME: draw-methods for sphere and non-aabb box
		const float4 color = this->hasOverlappingObjects? float4(0.5f, 0.5f, 1, 1): float4(0, 0, 1, 1);

		HavokVisualDebuggerServer::Instance()->DrawAABB(bbox(this->transform.get_position(), this->halfSize), color);
	}
}

}