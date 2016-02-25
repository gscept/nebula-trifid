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
#include "PxShape.h"
#include "PxRigidActor.h"
#include "geometry/PxBoxGeometry.h"
#include "geometry/PxSphereGeometry.h"
#include "geometry/PxCapsuleGeometry.h"
#include "geometry/PxConvexMeshGeometry.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "../model/templates.h"
#include "PxFiltering.h"
#include "extensions/PxDefaultSimulationFilterShader.h"

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
PhysXCollider::CreatePlane(PxRigidActor * target, const ColliderDescription & desc, const Math::float4 &scale, const Math::quaternion & quat, const Math::float4 &trans, const physx::PxMaterial& material)
{
	PxPlaneGeometry * geom = n_new(PxPlaneGeometry);
    Math::float4 point = desc.plane.plane.get_point();
    point *= scale;
	PxPlane pxplane(Neb2PxVec(point), Neb2PxVec(desc.plane.plane.get_normal()));
	PxTransform ptrans = PxTransformFromPlaneEquation(pxplane);
    if (!quat.isidentity())
    {
        n_warning("ignoring local rotation with plane %s", desc.name.AsCharPtr());
    }
    target->createShape(*geom, material, ptrans);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::CreateBox(PxRigidActor * target, const ColliderDescription & desc, const Math::float4 &scale, const Math::quaternion & quat, const Math::float4 &trans, const physx::PxMaterial& material)
{
    Math::float4 half = desc.box.halfWidth;
    half *= scale;
    PxBoxGeometry * geom = n_new(PxBoxGeometry(Neb2PxVec(half)));    
    PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
    target->createShape(*geom, material, ptrans);
}


//------------------------------------------------------------------------------
/**
*/
void 
PhysXCollider::CreateSphere(PxRigidActor * target, const ColliderDescription & desc, const Math::float4 &scale, const Math::float4 &trans, const physx::PxMaterial& material)
{
    float rscale = n_max(n_max(scale.x(), scale.y()), scale.z());
	PxSphereGeometry * geom = n_new(PxSphereGeometry(desc.sphere.radius *rscale));
    PxTransform ptrans(Neb2PxVec(trans));
    target->createShape(*geom, material, ptrans);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::CreateCapsule(PxRigidActor * target, const ColliderDescription & desc, const Math::float4 &scale, const Math::quaternion & quat, const Math::float4 &trans, const physx::PxMaterial& material)
{
	PxCapsuleGeometry * geom = n_new(PxCapsuleGeometry(desc.capsule.radius * n_max(scale.x(),scale.z()), 0.5f * desc.capsule.height * scale.y()));
    PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
    target->createShape(*geom, material, ptrans);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::CreatePhysicsMesh(PxRigidActor * target, const ColliderDescription & desc, const Math::float4 &scale, const Math::quaternion & quat, const Math::float4 &trans, const physx::PxMaterial& material)
{
    Ptr<ManagedPhysicsMesh> colliderMesh = Resources::ResourceManager::Instance()->CreateManagedResource(PhysicsMesh::RTTI, desc.mesh.meshResource,0,true).cast<ManagedPhysicsMesh>();
	Ptr<PhysXPhysicsMesh> mesh = colliderMesh->GetMesh().cast<PhysXPhysicsMesh>();
	
	switch (desc.mesh.meshType)
	{
		case MeshConvex:
		{
			n_warning("using convex mesh as concave");
			PxConvexMesh * cmesh = mesh->GetConvexMesh(desc.mesh.primGroup);
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
            PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
            target->createShape(*geom, material, ptrans);
		}
		break;
		case MeshConcave:
		case MeshStatic:
		{
			PxTriangleMesh * tmesh = mesh->GetMesh(desc.mesh.primGroup);
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
			PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
            target->createShape(*geom, material, ptrans);
		}
		break;
		case MeshConvexHull:
		case MeshConvexDecomposition:
		{
			PxConvexMesh * cmesh = mesh->GetConvexHullMesh(desc.mesh.primGroup);
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
            PxTransform ptrans(Neb2PxVec(trans), Neb2PxQuat(quat));
            target->createShape(*geom, material, ptrans);
		}
		break;			
	}

}

//------------------------------------------------------------------------------
/**
*/
void
PhysXCollider::CreateInstance(PxRigidActor * target, Math::vector & xscale, const physx::PxMaterial& material)
{
    for (int i = 0; i < this->descriptions.Size(); i++)
    {
        const ColliderDescription & desc = this->descriptions[i];

        Math::float4 scale;
        Math::quaternion quat;
        Math::float4 trans;
        desc.transform.decompose(scale, quat, trans);
        scale *= xscale;
        trans *= xscale;
        switch (desc.type)
        {
        default:
            break;
        case ColliderSphere:
            this->CreateSphere(target, desc, scale, trans, material);
            break;
        case ColliderCube:
            this->CreateBox(target, desc, scale, quat, trans, material);
            break;
        case ColliderCylinder:
            this->CreateCapsule(target, desc, scale, quat, trans, material);
            break;
        case ColliderCapsule:
            this->CreateCapsule(target, desc, scale, quat, trans, material);
            break;
        case ColliderPlane:
            this->CreatePlane(target, desc, scale, quat, trans, material);
            break;
        case ColliderMesh:
            this->CreatePhysicsMesh(target, desc, scale, quat, trans, material);
            break;
        }
    }

}

} // namespace PhysX

