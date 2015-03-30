#ifndef VEGETATION_GRASSPLANTMESH_H
#define VEGETATION_GRASSPLANTMESH_H
//------------------------------------------------------------------------------
/**
    @class Vegetation::GrassPlantMesh
  
    Contains a vertex and index buffer for one plant, in system memory
    because plants are combined into pseudo random grass patch meshes.

    (C) 2006 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "resources/managedmesh.h"
#include "models/nodes/shapenode.h"

//------------------------------------------------------------------------------
namespace Vegetation
{
class GrassPlantMesh : public Core::RefCounted
{
    __DeclareClass(GrassPlantMesh);
public:
    /// constructor
    GrassPlantMesh();
    /// destructor
    virtual ~GrassPlantMesh();
    /// setup the grass plant mesh
    void Setup(const Ptr<Models::ShapeNode>& shapeNode, const Ptr<Resources::ManagedMesh>& mesh, IndexT meshGroupIndex, const Math::matrix44& localTransform);
    /// get the number of vertices
    int GetNumVertices() const;
    /// get the number of indices
    int GetNumIndices() const;
    /// get the vertex width in number of floats
    int GetVertexWidth() const;
    /// get the original shape node
    Ptr<Models::ShapeNode> GetShapeNode() const;
    /// copy vertices to destination vertex buffer, return updated vertex pointer
    float* CopyVertices(float* dstVertexPtr, const Math::matrix44& transform);
    /// copy indices to destination index buffer, return updates index pointer
    ushort* CopyIndices(ushort* dstIndexPtr, ushort baseVertexIndex);
    /// get the vertex offset of the dynamic height vertex component
    static int GetDynamicHeightVertexOffset();

private:
    Ptr<Models::ShapeNode> refShapeNode;
    int numVertices;
    int vertexWidth;
    int numIndices;
    float* vertices;
    ushort* indices;
};

//------------------------------------------------------------------------------
/**
*/
inline
Ptr<Models::ShapeNode>
GrassPlantMesh::GetShapeNode() const
{
    return this->refShapeNode;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
GrassPlantMesh::GetNumVertices() const
{
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
GrassPlantMesh::GetNumIndices() const
{
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
GrassPlantMesh::GetVertexWidth() const
{
    return this->vertexWidth;
}

} // namespace Vegetation
//------------------------------------------------------------------------------
#endif
