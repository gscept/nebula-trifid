#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokCollider
	
	Consists of an array of shapes, where each shape consists of either a
	hkpConvexTransformShape or hkpTransformShape, which is just a shape coupled
	with a transform.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/basecollider.h"
#include "physics/resource/managedphysicsmesh.h"
#include "physics/model/templates.h"

#include <Common/Base/hkBase.h>
#include <Physics2012/Dynamics/Entity/hkpRigidBodyCinfo.h>
#include "Common/Base/Types/Geometry/hkGeometry.h"
#include "havokdebugdrawer.h"

class hkpMoppCode;
class hkpShape;

namespace Physics
{
	class ManagedPhysicsMesh;
}

namespace Havok
{
class HavokCollider : public Physics::BaseCollider
{
	__DeclareClass(HavokCollider);
public:
	/// default constructor
	HavokCollider();
	/// destructor
	~HavokCollider();

	/// render debug visualization
	virtual void RenderDebug(const Math::matrix44& t);
	/// add a plane
	virtual void AddPlane(const Math::plane &plane, const Math::matrix44 &localTransform = Math::matrix44::identity());

	/// add a box to the collision shape.
	virtual void AddBox(const Math::vector &halfWidth, const Math::matrix44 &localTransform = Math::matrix44::identity());
	/// add a box to the collision shape.
	virtual void AddBox(const Math::bbox & box );

	/// add a sphere to the collision shape.
	virtual void AddSphere(float radius, const Math::matrix44 &localTransform = Math::matrix44::identity());

	/// Add a cylinder to the collision shape.
	virtual void AddCylinder(float radius, float height, const Math::matrix44 &localTransform = Math::matrix44::identity());
	/// Add a cylinder to the collision shape.
	virtual void AddCylinder(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform = Math::matrix44::identity());

	/// add a Y axis capsule to the collision shape.
	virtual void AddCapsule(float radius, float height, const Math::matrix44  &localTransform = Math::matrix44::identity());
	/// add a capsule to the collision shape.
	virtual void AddCapsule(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform = Math::matrix44::identity());

	/// add a triangle mesh to the collision shape.
	virtual void AddPhysicsMesh(Ptr<Physics::ManagedPhysicsMesh> colliderMesh, const Math::matrix44 & localTransform, Physics::MeshTopologyType meshType, int primGroup );
	
	/// get the array of shapes (remember that each shape is either a transformshape or a convextransformshape)
	const hkArray<hkpShape*>& GetShapes() const;

	/// get the coupled hkQsTransform of a shape
	static hkQsTransform GetTransformFromTransformShape(const hkpShape* transformShape);
	/// get the shape-pointer of the transformshape
	static const hkpShape* GetShapeFromTransformShape(const hkpShape* transformShape);

	/// get scale factor
	const Math::vector& GetScale() const;
	/// set scale factor
	void SetScale(const Math::vector& sc);

private:

	/// returns wether the shape is a supported transformshape
	static bool IsSupportedTransformShape(const hkpShape* shape);

	/// append shape
	void AppendShape(hkpShape* shape);

	/// note: every shape object is a transformshape, which pairs together a shape and a transform (see hkpTransformShape or hkpConvextTransformShape for example)
	hkArray<hkpShape*> shapes;

    /// create dbgShape from a hkGeometry
    HavokDebugDrawer::dbgShape * CreateDbgShape(hkGeometry * geom);

    /// for storing hkGeometry of hkpBvCompressedMeshShapes for debug visualization as they are expensive to generate
    Util::Dictionary<int, HavokDebugDrawer::dbgShape *> debugGeometry;    

	/// optional scale
	Math::vector scale;
};

//------------------------------------------------------------------------------
/**
*/
inline void 
HavokCollider::AppendShape(hkpShape* shape)
{
	this->shapes.pushBack(shape);
}

//------------------------------------------------------------------------------
/**
*/
inline const hkArray<hkpShape*>& 
HavokCollider::GetShapes() const
{
	return this->shapes;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector& 
HavokCollider::GetScale() const
{
	return this->scale;
}

//------------------------------------------------------------------------------
/**
*/
inline void
HavokCollider::SetScale(const Math::vector& sc )
{
	this->scale = sc;
}


}