//------------------------------------------------------------------------------
//  physxcollider.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/resource/managedphysicsmesh.h"
#include "physics/physx/physxcollider.h"
#include "resources/resourcemanager.h"
#include "geometry/PxPlaneGeometry.h"
#include "physxutils.h"
#include "geometry/PxBoxGeometry.h"
#include "geometry/PxSphereGeometry.h"
#include "geometry/PxCapsuleGeometry.h"
#include "geometry/PxConvexMeshGeometry.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "../model/templates.h"

using namespace physx;
using namespace Math;
using namespace Physics;

namespace PhysX
{
__ImplementClass(PhysX::PhysXCollider, 'PXCO', Physics::BaseCollider);

//------------------------------------------------------------------------------
/**
*/
PhysXCollider::PhysXCollider()
{
	/// empty
}

//------------------------------------------------------------------------------
/**
*/
PhysXCollider::~PhysXCollider()
{	
	///empty
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::RenderDebug(const Math::matrix44& t)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddPlane(const Math::plane &plane, const Math::matrix44 &localTransform)
{
	PxPlaneGeometry * geom = n_new(PxPlaneGeometry);
	PxPlane pxplane(Neb2PxVec(plane.get_point()), Neb2PxVec(plane.get_normal()));
	PxTransform trans = PxTransformFromPlaneEquation(pxplane);
	this->geometry.Append(geom);
	this->transforms.Append(trans);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddBox(const Math::vector &halfWidth, const Math::matrix44 &localTransform)
{
	PxBoxGeometry * geom = n_new(PxBoxGeometry(Neb2PxVec(halfWidth)));
	PxTransform trans = Neb2PxTrans(localTransform);
	this->geometry.Append(geom);
	this->transforms.Append(trans);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddBox(const Math::bbox & box)
{
	n_error("deprecated");
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddCylinder(float radius, float height, const Math::matrix44 &localTransform)
{
	n_error("not supported");
}


//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddCylinder(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform)
{
	n_error("not supported");
}


//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddSphere(float radius, const Math::matrix44 &localTransform)
{
	PxSphereGeometry * geom = n_new(PxSphereGeometry(radius));
	PxTransform trans = Neb2PxTrans(localTransform);
	this->geometry.Append(geom);
	this->transforms.Append(trans);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddCapsule(float radius, float height, const Math::matrix44 &localTransform)
{
	PxCapsuleGeometry * geom = n_new(PxCapsuleGeometry(radius, 0.5f * height));
	PxTransform trans = Neb2PxTrans(localTransform);
	this->geometry.Append(geom);
	this->transforms.Append(trans);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddCapsule(const Math::vector& pointA, const Math::vector& pointB, float radius, const Math::matrix44 &localTransform)
{
	n_error("not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::AddPhysicsMesh(Ptr<Physics::ManagedPhysicsMesh> colliderMesh, const Math::matrix44 & localTransform, Physics::MeshTopologyType meshType, int primGroup)
{
	Ptr<PhysXPhysicsMesh> mesh = colliderMesh.cast<PhysXPhysicsMesh>();
	Math::float4 scale;
	Math::quaternion quat;
	Math::float4 trans;
	localTransform.decompose(scale, quat, trans);
	switch (meshType)
	{
		case MeshConvex:
		{
			n_warning("using convex mesh as concave");
			PxConvexMesh * cmesh = mesh->GetConvexMesh(primGroup);
			PxConvexMeshGeometry* geom;
			if (!float4::nearequal3(scale, float4(1.0f, 1.0f, 1.0f, 1.0f), float4(0.01f, 0.01f, 0.01f, 0.01f)))
			{
				PxMeshScale pscale(Neb2PxVec(scale), PxQuat(PxIdentity));
				geom = n_new(PxConvexMeshGeometry(cmesh, pscale));
			}
			else
			{
				geom = n_new(PxConvexMeshGeometry(cmesh));
			}
			this->geometry.Append(geom);
			PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
			this->transforms.Append(ptrans);			
		}
		break;
		case MeshConcave:
		case MeshStatic:
		{
			PxTriangleMesh * tmesh = mesh->GetMesh(primGroup);
			PxTriangleMeshGeometry * geom;
			if (!float4::nearequal3(scale, float4(1.0f, 1.0f, 1.0f, 1.0f), float4(0.01f, 0.01f, 0.01f, 0.01f)))
			{
				PxMeshScale pscale(Neb2PxVec(scale), PxQuat(PxIdentity));
				geom = n_new(PxTriangleMeshGeometry(tmesh, pscale));

			}
			else
			{
				geom = n_new(PxTriangleMeshGeometry(tmesh));
			}			
			this->geometry.Append(geom);
			PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
			this->transforms.Append(ptrans);
		}
		break;
		case MeshConvexHull:
		case MeshConvexDecomposition:
		{
			PxConvexMesh * cmesh = mesh->GetConvexHullMesh(primGroup);
			PxConvexMeshGeometry* geom;
			if (!float4::nearequal3(scale, float4(1.0f, 1.0f, 1.0f, 1.0f), float4(0.01f, 0.01f, 0.01f, 0.01f)))
			{
				PxMeshScale pscale(Neb2PxVec(scale), PxQuat(PxIdentity));
				geom = n_new(PxConvexMeshGeometry(cmesh, pscale));
			}
			else
			{
				geom = n_new(PxConvexMeshGeometry(cmesh));
			}
			this->geometry.Append(geom);
			PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
			this->transforms.Append(ptrans);		
		}
		break;			
	}

}

} // namespace PhysX

