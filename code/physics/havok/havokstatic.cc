//------------------------------------------------------------------------------
//  havokstatic.cc
//  (C) 2013-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "havokstatic.h"
#include "havokcontactlistener.h"
#include "havokscene.h"

#include <Physics2012/Collide/Shape/Compound/Collection/List/hkpListShape.h>

namespace Havok
{
using namespace Physics;

__ImplementClass(Havok::HavokStatic,'HKST', Physics::BaseStatic);

//------------------------------------------------------------------------------
/**
*/
HavokStatic::HavokStatic()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
HavokStatic::~HavokStatic()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokStatic::SetupFromTemplate(const PhysicsCommon& templ)
{
	n_assert(HK_NULL == this->rigidBody);

	BaseStatic::SetupFromTemplate(templ);

	Math::vector scale;
	templ.startTransform.get_scale(scale);

	// note: the scale will be changed by rotations, so even if the scale is actually 1, 1, 1 it will become something
	// else in the transformation matrix because of the rotation
	if(scale.x() != 1 || scale.y() != 1 || scale.z() != 1)
	{
		// need scaled collider. check if supported by shape
		n_assert2(templ.collider->GetDescriptions().Size() == 1, "only singular shapes support scale");
		//n_assert2(templ.collider->GetDescriptions().Size() == 1, "only singular box shapes support scale");
		//n_assert2(templ.collider->GetDescriptions()[0].type == ColliderCube, "only box shapes support scale");    

		if (templ.collider->GetDescriptions()[0].type != ColliderCube)
		{
			n_warning("HavokStatic::SetupFromTemplate: ###### Scale is set for a non-cube static physics body! The scale will not be applied #####\n");
		}

		Ptr<HavokCollider> newbox = HavokCollider::Create();
		newbox->SetScale(scale);
		newbox->AddFromDescription(templ.collider->GetDescriptions()[0]);
		this->common.collider = 0;
		this->common.collider = newbox.cast<Collider>();
	}

    const hkArray<hkpShape*>& shapes = this->common.collider->GetShapes();	

    //FIXME: this should be a StaticCompound, but it causes crashes with raycasts for some reason(?)
    // using a static rigid body with a listshape now

	// set up the static compound shape
	hkpListShape* compoundShape = new hkpListShape(shapes.begin(),shapes.getSize());

	

/*	IndexT i;
	for (i = 0; i < shapes.getSize(); i++)
	{
		const hkpShape* shape = shapes[i];

		// note: since every shape is some kind of transform-shape, add it with an identity transform
		//compoundShape->addInstance(shape, hkQsTransform::getIdentity());
	}

	// bake the shape to its final form
	compoundShape->bake();
    */    

	hkpRigidBodyCinfo bodyInfo;
	bodyInfo.m_shape = compoundShape;
	bodyInfo.m_mass = 0.0f;
	bodyInfo.m_motionType = hkpMotion::MOTION_FIXED;

	Math::quaternion startRotation = Math::quaternion::rotationmatrix(templ.startTransform);
	startRotation = Math::quaternion::normalize(startRotation);
	Math::vector startPosition = templ.startTransform.get_position();

	bodyInfo.m_rotation = Neb2HkQuaternion(startRotation);
	bodyInfo.m_position = Neb2HkFloat4(startPosition);
	bodyInfo.m_mass = templ.mass;

	bodyInfo.m_collisionFilterInfo = (int)Math::n_log2(Physics::Static); /*//TODO: log2 is used until collide categories are changed to 1, 2, 3... instead of 1, 2, 4, 8...*/

	this->rigidBody = new hkpRigidBody(bodyInfo);

	this->rigidBody->setUserData((hkUlong)this);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokStatic::Attach(Physics::BaseScene* inWorld)
{
	n_assert(HK_NULL != this->rigidBody);

	this->world = ((HavokScene*)inWorld)->GetWorld();
	world->addEntity(this->rigidBody);
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokStatic::Detach()
{
	n_assert(HK_NULL != this->rigidBody);
	n_assert(HK_NULL != this->world);
    
    if(this->rigidBody->isAddedToWorld())
    {
	    this->world->removeEntity(this->rigidBody);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokStatic::RenderDebug()
{
	PhysicsObject::RenderDebug();
	n_assert2(this->common.collider.isvalid(),"empty collider");
	this->common.collider->RenderDebug(this->GetTransform());
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokStatic::SetEnableCollisionCallback(bool enable)
{
	PhysicsObject::SetEnableCollisionCallback(enable);

	if (enable)
	{
		Ptr<HavokContactListener> contactListener = HavokContactListener::Create();
		contactListener->AttachToObject(this);
	}
}
//------------------------------------------------------------------------------
/**
*/
void 
HavokStatic::SetCollideFilter(uint mask)
{
#if _DEBUG
	n_error(
#else
	n_warning(
#endif
		"HavokStatic::SetCollideFilter: Can not set collide filter directly on a havok-physobject! Instead you must update the collidefilter in the scene");
}

//------------------------------------------------------------------------------
/**
*/
void 
HavokStatic::SetCollideCategory(Physics::CollideCategory coll)
{
	n_assert(this->rigidBody);

	PhysicsObject::SetCollideCategory(coll);
	this->rigidBody->setCollisionFilterInfo(
		//TODO: blä, fix CollideCategory
		(int)(((int)coll > 1)? Math::n_log2((Math::scalar)coll): coll));

	if (this->world)
	{
		this->world->updateCollisionFilterOnEntity(this->rigidBody, 
			HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, 
			HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS);
	}
}

}