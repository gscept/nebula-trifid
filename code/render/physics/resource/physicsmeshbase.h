#pragma once

#include "resources/resource.h"
#include "coregraphics/indextype.h"


namespace Physics
{

class PhysicsMeshBase : public Resources::Resource
{
	__DeclareClass(PhysicsMeshBase);
public:
	PhysicsMeshBase(){}
	virtual ~PhysicsMeshBase(){};
	
	virtual void AddMeshComponent(int id, float * vertexData, uint numVertices, uint verticeStride, uint * indexData, uint numTriangles) = 0;
};

}
