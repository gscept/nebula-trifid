//------------------------------------------------------------------------------
//  baseshape.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/resource/managedphysicsmesh.h"
#include "physics/base/basecollider.h"
#include "resources/resourcemanager.h"

namespace Physics
{
__ImplementAbstractClass(Physics::BaseCollider, 'PHBC', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
BaseCollider::BaseCollider()     
{
	/// empty
}

//------------------------------------------------------------------------------
/**
*/
BaseCollider::~BaseCollider()
{	
	///empty
}

void 
BaseCollider::AddFromDescription(const ColliderDescription & description)
{	
	switch(description.type)
	{
		case ColliderSphere:
			{
				AddSphere(description.sphere.radius,description.transform);
			}
			break;
		case ColliderCube:
			{
				AddBox(description.box.halfWidth,description.transform);
			}
			break;
		case ColliderCylinder:
			{
				AddCylinder(description.cylinder.radius,description.cylinder.height,description.transform);
			}
			break;
		case ColliderCapsule:
			{
				AddCapsule(description.capsule.radius,description.capsule.height,description.transform);
			}
			break;
		case ColliderPlane:
			{
				AddPlane(description.plane.plane,description.transform);
			}
			break;
		case ColliderMesh:
			{
				Ptr<ManagedPhysicsMesh> mesh = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsMesh::RTTI,description.mesh.meshResource).cast<ManagedPhysicsMesh>();				
				AddPhysicsMesh(mesh,description.transform,description.mesh.meshType,description.mesh.primGroup);				
			}
			break;
	}
	this->descriptions.Append(description);
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseCollider::AddCapsule(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform)
{
	n_error("BaseCollider::AddCapsule: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseCollider::AddCapsule(float radius, float height, const Math::matrix44 &localTransform)
{
	n_error("BaseCollider::AddCapsule: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseCollider::AddCylinder(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform)
{
	n_error("BaseCollider::AddCylinder: Not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void 
BaseCollider::AddCylinder(float radius, float height, const Math::matrix44 &localTransform)
{
	n_error("BaseCollider::AddCylinder: Not implemented");
}

} // namespace Physics
