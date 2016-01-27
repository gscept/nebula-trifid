#pragma once
//------------------------------------------------------------------------------
/**
    @class Physics::BaseCollider
    
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "math/bbox.h"
#include "physics/model/templates.h"

namespace Physics
{
class ManagedPhysicsMesh;

class BaseCollider : public Core::RefCounted
{
    __DeclareAbstractClass(BaseCollider);
public:
	   
    /// default constructor
    BaseCollider();
    /// destructor
    virtual ~BaseCollider();
    /// render debug visualization
	virtual void RenderDebug(const Math::matrix44& t) = 0;

	virtual void AddPlane(const Math::plane &plane, const Math::matrix44 &localTransform ) = 0;

	/// Add a box to the collision shape.
	virtual void AddBox(const Math::vector &halfWidth, const Math::matrix44 &localTransform ) = 0;
	/// Add a box to the collision shape.
	virtual void AddBox(const Math::bbox & box ) = 0;

	/// Add a Y axis cylinder to the collision shape.
	virtual void AddCylinder(float radius, float height, const Math::matrix44 &localTransform);
	/// Add a cylinder to the collision shape.
	virtual void AddCylinder(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform);

	/// Add a sphere to the collision shape.
	virtual void AddSphere(float radius, const Math::matrix44 &localTransform ) = 0;

	/// Add a Y axis capsule to the collision shape.
	virtual void AddCapsule(float radius, float height, const Math::matrix44 &localTransform) = 0;
	/// Add a capsule to the collision shape.
	virtual void AddCapsule(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform);

	/// Add a triangle mesh to the collision shape.
	virtual void AddPhysicsMesh(Ptr<ManagedPhysicsMesh> colliderMesh, const Math::matrix44 & localTransform, Physics::MeshTopologyType meshType, int primGroup ) = 0;

	void AddFromDescription(const ColliderDescription & description);
    
	const Util::String & GetName() const;

	const Util::Array<ColliderDescription>& GetDescriptions() const;
	
protected:    
	Util::String name;
	Util::Array<ColliderDescription> descriptions;
		
};

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String &
BaseCollider::GetName() const
{
	return this->name;    
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::Array<ColliderDescription>&
BaseCollider::GetDescriptions() const
{
	return this->descriptions;    
}

}; // namespace Physics


    