//------------------------------------------------------------------------------
//  grassplantmesh.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grassplantmesh.h"
#include "coregraphics/primitivegroup.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassPlantMesh, 'VGPM', Core::RefCounted);

using namespace CoreGraphics;
using namespace Math;
using namespace Util;
//------------------------------------------------------------------------------
/**
*/
GrassPlantMesh::GrassPlantMesh() :
    numVertices(0),
    vertexWidth(0),
    numIndices(0),
    vertices(0),
    indices(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GrassPlantMesh::~GrassPlantMesh()
{
    if (0 != this->vertices)
    {
        Memory::Free(Memory::ResourceHeap, this->vertices);
        this->vertices = 0;
    }
    if (0 != this->indices)
    {
        Memory::Free(Memory::ResourceHeap, this->indices);
        this->indices = 0;
    }
    this->refShapeNode = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
GrassPlantMesh::Setup(const Ptr<Models::ShapeNode>& shapeNode, const Ptr<Resources::ManagedMesh>& managedMesh, IndexT meshGroupIndex, const Math::matrix44& localTransform)
{
    n_assert(managedMesh.isvalid());
    n_assert(0 == this->vertices);
    n_assert(0 == this->indices);

    const Ptr<Mesh> mesh = managedMesh->GetMesh();
    n_assert((meshGroupIndex >= 0) && (meshGroupIndex < mesh->GetNumPrimitiveGroups()));    
    this->refShapeNode = shapeNode;

    // extract the mesh data into private buffers    
    const PrimitiveGroup& meshGroup = mesh->GetPrimitiveGroupAtIndex(meshGroupIndex);
    this->vertexWidth = 5;  // coord + uv0s2 + uv1s2
    this->numVertices = meshGroup.GetNumVertices();
    this->numIndices  = meshGroup.GetNumIndices();
    int vertexBufferSize = this->numVertices * this->vertexWidth * sizeof(float);
    this->vertices = (float*) Memory::Alloc(Memory::ResourceHeap, vertexBufferSize);
    this->indices = (ushort*) Memory::Alloc(Memory::ResourceHeap, this->numIndices * sizeof(ushort));

    // create a transform matrix which centers the current plant on the origin,
    // but keep the height above ground as it is, also create a matrix33
    // for rotation and scaling normals
    Math::matrix44 m44 = localTransform;
    m44.set_position(point(0.0f, m44.get_position().y(), 0.0f));

    // get source vertex components 
    Ptr<VertexLayout> layout = mesh->GetVertexBuffer()->GetVertexLayout();
    n_assert(layout->HasComponent(VertexComponent::Position, 0)
        && layout->HasComponent(VertexComponent::Normal, 0)
        && layout->HasComponent(VertexComponent::TexCoord, 0)
        && layout->HasComponent(VertexComponent::Tangent, 0)
        && layout->HasComponent(VertexComponent::Binormal, 0));

    // copy vertices   
    const Ptr<VertexBuffer>& vertexBuffer = mesh->GetVertexBuffer();
    Math::vector vec3;
    float* vPtr = (float*)vertexBuffer->Map(VertexBuffer::MapRead);
    float* srcPtr = vPtr + (meshGroup.GetBaseVertex() * layout->GetVertexByteSize() / 4);
    float* dstPtr = (float*)this->vertices;
    int vi;
    for (vi = 0; vi < this->numVertices; vi++)
    {
        // coord coord
        vec3.set(srcPtr[0], srcPtr[1], srcPtr[2]);
        vec3 = float4::transform(vec3, m44);            
        *dstPtr++ = vec3.x(); *dstPtr++ = vec3.y(); *dstPtr++ = vec3.z();
        srcPtr += 3;

        // skip packed normal
        srcPtr += 1;
        
        // copy packed uv0
        *(uint*)dstPtr++ = *(uint*)srcPtr++;

        // add packed uv1 for our own custom data
        *(uint*)dstPtr++ = 0;
        
        // skip optional tangent and binormal
        if (layout->HasComponent(VertexComponent::Tangent, 0))
        {
            srcPtr += 1;
        }
        if (layout->HasComponent(VertexComponent::Binormal, 0))
        {
            srcPtr += 1;
        }
    }
    vertexBuffer->Unmap();

    // copy indices, shift index range so that they are 0-based
    Ptr<IndexBuffer> indexBuffer = mesh->GetIndexBuffer();
    ushort* srcIndices = (ushort*)indexBuffer->Map(IndexBuffer::MapRead) + meshGroup.GetBaseIndex();
    ushort baseIndex = (ushort) meshGroup.GetBaseVertex();
    int ii;
    for (ii = 0; ii < this->numIndices; ii++)
    {
        ushort srcIndex = srcIndices[ii];
        n_assert(srcIndex >= baseIndex);
        this->indices[ii] = srcIndex - baseIndex;
    }
    indexBuffer->Unmap();
}

//------------------------------------------------------------------------------
/**
    Returns the offset (in number of floats) of the dynamic height 
    vertex component.
*/
int
GrassPlantMesh::GetDynamicHeightVertexOffset()
{
    // 3 coord + 1 packed uv
    return 4;
}

//------------------------------------------------------------------------------
/**
    Copy vertices to destination vertex buffer, transforming them on the way.
    Returns the updated vertex pointer, pointing to the end of the copied
    area.
*/
float*
GrassPlantMesh::CopyVertices(float* dstPtr, const Math::matrix44& transform)
{
    n_assert(0 != this->vertices);
    n_assert(this->vertexWidth == 5);
    Math::point coord;
    float* srcPtr = this->vertices;
    int i;
    for (i = 0; i < this->numVertices; i++)
    {
        // coord
        coord.set(srcPtr[0], srcPtr[1], srcPtr[2]);
        coord = Math::float4::transform(coord, transform);
        dstPtr[0] = coord.x(); dstPtr[1] = coord.y(), dstPtr[2] = coord.z();
        
        // packed uv0
        ((uint*)dstPtr)[3] = ((uint*)srcPtr)[3];

        // pakced uv1
        ((uint*)dstPtr)[4] = ((uint*)srcPtr)[4];
        
        // update ptrs
        srcPtr += this->vertexWidth;
        dstPtr += this->vertexWidth;
    }
    return dstPtr;
}

//------------------------------------------------------------------------------
/**
    Copy indices to destination index buffer, returns updated destination
    index buffer
*/
ushort*
GrassPlantMesh::CopyIndices(ushort* dstPtr, ushort baseVertexIndex)
{
    n_assert(0 != this->indices);
    ushort* srcPtr = this->indices;
    int i;
    for (i = 0; i < this->numIndices; i++)
    {
        dstPtr[i] = srcPtr[i] + baseVertexIndex;
    }
    dstPtr += this->numIndices;
    return dstPtr;
}

} // namespace Vegetation
