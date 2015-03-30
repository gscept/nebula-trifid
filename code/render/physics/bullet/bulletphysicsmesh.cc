#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "physics/bullet/conversion.h"
#include "physics/bullet/bulletphysicsmesh.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

namespace Bullet
{

	using namespace Physics;

__ImplementClass(Bullet::BulletPhysicsMesh,'PBPM', Physics::PhysicsMeshBase);

BulletPhysicsMesh::BulletPhysicsMesh() 
{

}
BulletPhysicsMesh::~BulletPhysicsMesh()
{
	Util::Array<btIndexedMesh> meshdata = this->meshes.ValuesAsArray();
	for(int i=0;i<meshdata.Size();i++)
	{
		Memory::Free(Memory::PhysicsHeap,(void*)meshdata[i].m_triangleIndexBase);
		Memory::Free(Memory::PhysicsHeap,(void*)meshdata[i].m_vertexBase);
	}
	this->meshes.Clear();
}

btCollisionShape* 
BulletPhysicsMesh::GetShape(int primGroup, MeshTopologyType meshType)
{
	btCollisionShape * shape;
	btTriangleIndexVertexArray *mesh = n_new(btTriangleIndexVertexArray());	
	switch(meshType)
	{
	case MeshConcave:
		{
			mesh->addIndexedMesh(this->meshes[primGroup],PHY_INTEGER);
			btGImpactMeshShape * gshape = n_new(btGImpactMeshShape(mesh));			
			
			gshape->setLocalScaling(btVector3(1.f,1.f,1.f));
			gshape->setMargin(0.0f);
			gshape->updateBound();
			shape = (btCollisionShape *)gshape;
		}
		break;
	case MeshConvex:
		{
			
			// this is how its suppoed to be done (in theory, however as bullet documentation says
			// its faster to use the convex hull instead
#if 1
			mesh->addIndexedMesh(this->meshes[primGroup],PHY_INTEGER);
			btConvexTriangleMeshShape * cshape = n_new(btConvexTriangleMeshShape(mesh));
#else
			btConvexHullShape * cshape = n_new(btConvexHullShape((btScalar*)this->meshes[primGroup].m_vertexBase,this->meshes[primGroup].m_numVertices,this->meshes[primGroup].m_vertexStride));
#endif
			

			cshape->setLocalScaling((btVector3(1.f,1.f,1.f)));
			cshape->setMargin(0.f);
			shape = (btCollisionShape*) cshape;
		}
		break;
	case MeshConvexHull:
		{
			btConvexHullShape * cshape = n_new(btConvexHullShape((btScalar*)this->meshes[primGroup].m_vertexBase,this->meshes[primGroup].m_numVertices,this->meshes[primGroup].m_vertexStride));
			cshape->setLocalScaling(btVector3(1.f,1.f,1.f));
			cshape->setMargin(0.f);
			shape = (btCollisionShape *)cshape;
		}
		break;
	case MeshStatic:
		{
			mesh->addIndexedMesh(this->meshes[primGroup],PHY_INTEGER);
			btBvhTriangleMeshShape * bshape = n_new(btBvhTriangleMeshShape(mesh,true));
			shape = (btCollisionShape *)bshape;
		}
		break;
	default:
		n_error("Not implemented mesh topology type");

	}


	return shape;
}

void
BulletPhysicsMesh::AddMeshComponent(int id, float * vertexData, uint numVertices, uint verticeStride, uint * indexData, uint numTriangles)
{
	
	btIndexedMesh meshData;
	meshData.m_indexType = PHY_INTEGER;
	
	meshData.m_numTriangles = numTriangles;
	meshData.m_numVertices = numVertices;

	size_t indexbytes = numTriangles * sizeof(uint) * 3;
	meshData.m_triangleIndexBase = (unsigned char*)Memory::Alloc(Memory::PhysicsHeap, indexbytes );
	Memory::Copy(indexData,(void*)meshData.m_triangleIndexBase,indexbytes);
				
	size_t vertexbytes = numVertices * verticeStride * sizeof(float);
	meshData.m_vertexBase = (unsigned char*)Memory::Alloc(Memory::PhysicsHeap,vertexbytes);
	Memory::Copy(vertexData,(void*)meshData.m_vertexBase,vertexbytes);
	meshData.m_triangleIndexStride = 3 * sizeof(uint);
	meshData.m_vertexStride = verticeStride * sizeof(float);
	meshData.m_vertexType = PHY_FLOAT;
			
	this->meshes.Add(id,meshData);
	
}


}