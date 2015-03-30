#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "physics/bullet/conversion.h"
#include "physics/collider.h"
#include "math/bbox.h"


namespace Bullet
{
	__ImplementClass(Bullet::BulletCollider, 'PBCO', Physics::BaseCollider);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
BulletCollider::BulletCollider()
{
	this->shape = (btCompoundShape *)n_new(btCompoundShape(true));
}

//------------------------------------------------------------------------------
/**
*/
BulletCollider::~BulletCollider()
{
	n_delete(shape);
	this->shape = 0;
	this->meshes.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddPlane( const Math::plane &plane, const Math::matrix44 &localTransform)
{	
	btCollisionShape * childShape = (btCollisionShape *)n_new(btStaticPlaneShape(btVector3(plane.a(),plane.b(),plane.c()),plane.d()));
	this->shape->addChildShape(Neb2BtM44Transform(localTransform),childShape);
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddBox( const Math::vector &halfWidth, const Math::matrix44 &localTransform )
{	
	btCollisionShape * childShape = (btCollisionShape *)n_new(btBoxShape(Neb2BtVector(halfWidth)));
	this->shape->addChildShape(Neb2BtM44Transform(localTransform),childShape);
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddBox( const Math::bbox & box)
{
	Math::matrix44 trans;
	trans.translate(Math::vector(box.center()));
	AddBox(box.extents(),trans);
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddSphere( float radius, const Math::matrix44 &localTransform )
{
	btCollisionShape * childShape = (btCollisionShape *)n_new(btSphereShape(radius));
	this->shape->addChildShape(Neb2BtM44Transform(localTransform),childShape);
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddCapsule(float radius, float height, const Math::matrix44  &localTransform )
{
	btCollisionShape * childShape = (btCollisionShape *)n_new(btCapsuleShape(radius,height));
	this->shape->addChildShape(Neb2BtM44Transform(localTransform),childShape);
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddCylinder(float radius, float height, const Math::matrix44  &localTransform )
{
	btVector3 cylinderSize = btVector3(radius, height, 1);
	btCollisionShape * childShape = (btCollisionShape *)n_new(btCylinderShape(cylinderSize));
	this->shape->addChildShape(Neb2BtM44Transform(localTransform),childShape);
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddCapsule(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform)
{
	n_error("BulletCollider::AddCapsule: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
BulletCollider::AddPhysicsMesh(Ptr<Physics::ManagedPhysicsMesh> colliderMesh, const Math::matrix44 & localTransform, Physics::MeshTopologyType meshType, int primGroup)
{
	meshes.Append(colliderMesh);
	this->shape->addChildShape(Neb2BtM44Transform(localTransform),colliderMesh->GetMesh().cast<BulletPhysicsMesh>()->GetShape(primGroup,meshType));
}

//------------------------------------------------------------------------------
/**
*/
Ptr<BulletCollider> 
BulletCollider::GetScaledCopy(const vector &scale)
{
	Ptr<BulletCollider> newColl = BulletCollider::Create();

	for(IndexT i = 0; i < this->descriptions.Size() ; i++)
	{
		newColl->AddFromDescription(descriptions[i]);
	}	
	newColl->name = this->name;
	newColl->shape->setLocalScaling(Neb2BtVector(scale));
	return newColl;
}

//------------------------------------------------------------------------------
/**
*/
void
BulletCollider::RenderDebug(const matrix44 & trans)
{
	// empty
}
}