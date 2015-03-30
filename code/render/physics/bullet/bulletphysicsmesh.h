#pragma once

#include "physics/resource/physicsmeshbase.h"
#include "physics/model/templates.h"

class btCollisionShape;
class btBvhTriangleMeshShape;
class btTriangleIndexVertexArray;
class btGImpactMeshShape;
struct btIndexedMesh;

namespace Bullet
{

class BulletPhysicsMesh : public Physics::PhysicsMeshBase
{
	__DeclareClass(BulletPhysicsMesh);
public:
	BulletPhysicsMesh();
	~BulletPhysicsMesh();
	
	virtual void AddMeshComponent(int id, float * vertexData, uint numVertices, uint verticeStride, uint * indexData,  uint numTriangles);

	btCollisionShape* GetShape(int primGroup,Physics::MeshTopologyType meshtype);
private:	
		Util::Dictionary<int,btIndexedMesh> meshes;

};
}	