#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXCollider
    
    
    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "math/bbox.h"
#include "physics/model/templates.h"
#include "physics/base/basecollider.h"
#include "foundation/PxTransform.h"

namespace physx
{
	class PxGeometry;
}

namespace Physics
{
	class ManagedPhysicsMesh;
}

namespace PhysX
{
class PhysXCollider : public Physics::BaseCollider
{
    __DeclareClass(PhysXCollider);
public:
	   
    /// default constructor
	PhysXCollider();
    /// destructor
    virtual ~PhysXCollider();
    /// render debug visualization
	virtual void RenderDebug(const Math::matrix44& t);

	virtual void AddPlane(const Math::plane &plane, const Math::matrix44 &localTransform );

	/// Add a box to the collision shape.
	virtual void AddBox(const Math::vector &halfWidth, const Math::matrix44 &localTransform );
	/// Add a box to the collision shape.
	virtual void AddBox(const Math::bbox & box );

	/// Add a Y axis cylinder to the collision shape.
	virtual void AddCylinder(float radius, float height, const Math::matrix44 &localTransform);
	/// Add a cylinder to the collision shape.
	virtual void AddCylinder(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform);

	/// Add a sphere to the collision shape.
	virtual void AddSphere(float radius, const Math::matrix44 &localTransform );

	/// Add a Y axis capsule to the collision shape.
	virtual void AddCapsule(float radius, float height, const Math::matrix44 &localTransform);
	/// Add a capsule to the collision shape.
	virtual void AddCapsule(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform);

	/// Add a triangle mesh to the collision shape.
	virtual void AddPhysicsMesh(Ptr<Physics::ManagedPhysicsMesh> colliderMesh, const Math::matrix44 & localTransform, Physics::MeshTopologyType meshType, int primGroup );

	Util::Array<physx::PxGeometry*> geometry;
	Util::Array<physx::PxTransform> transforms;
		
};

}; // namespace PhysX


    