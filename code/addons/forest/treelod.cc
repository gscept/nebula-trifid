//------------------------------------------------------------------------------
//  treelod.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/treelod.h"
#include "coregraphics/vertexbuffer.h"

namespace Forest
{
__ImplementClass(Forest::TreeLOD, 'FOTL', Core::RefCounted);

using namespace Util;
using namespace Math;
using namespace Models;
using namespace CoreGraphics;

IndexT TreeLOD::uniqueId = 0;

//------------------------------------------------------------------------------
/**
*/
TreeLOD::TreeLOD() :
    minDist(0.0f),
    maxDist(0.0f),
    numVertices(0),
    vertices(0),
    numIndices(0),
    indices(0),
    vertexComps(0),
    vertexWidth(0),
    isBillboard(false),
    isValid(false),
    lodLevel(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TreeLOD::~TreeLOD()
{
    if (this->IsValid())
    {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeLOD::Setup(float minDist, float maxDist, const Ptr<ModelNode>& node, const StringAtom& nodePath, bool billboard, IndexT lodLevel)
{
    n_assert(!this->IsValid());
    n_assert(node.isvalid());
    n_assert(0 == this->vertices);
    n_assert(0 == this->indices);
    this->isValid = true;

    this->minDist = minDist;
    this->maxDist = maxDist;
    this->nodePath = nodePath;
    this->isBillboard = billboard;
    this->lodLevel = lodLevel;
    if (this->isBillboard)
    {        
		/*
        n_assert(1 == node->GetChildren().Size());  
        const Ptr<ModelNode> child = node->GetChildren()[0];
        n_assert(child->IsA(ShapeNode::RTTI));
		*/
        this->billboardShapeNode = node.downcast<ShapeNode>();
        
        // get the uv coordinates from mesh
        const Ptr<VertexBuffer>& vb = this->billboardShapeNode->GetManagedMesh()->GetMesh()->GetVertexBuffer();
        SizeT num = vb->GetVertexLayout()->GetNumComponents();
        IndexT stride = vb->GetVertexLayout()->GetVertexByteSize();
        ubyte* vertices = (ubyte*)vb->Map(VertexBuffer::MapRead);
        vb->Unmap();

        this->billboardModelInstance = node->GetModel()->CreatePartialInstance(nodePath, matrix44::identity());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TreeLOD::Discard()
{
    n_assert(this->IsValid());
    if (this->vertices)
    {
        Memory::Free(Memory::ResourceHeap, (void*) this->vertices);
        this->vertices = 0;
    }
    if (this->indices)
    {
        Memory::Free(Memory::ResourceHeap, (void*) this->indices);
        this->indices = 0;
    }
    if (this->billboardShapeNode.isvalid())
    {
        this->billboardShapeNode = 0;
    }
    
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
    NOTE: since we can't really do hardware instanced rendering on our
    MinSpec, and don't want to waste CPU cycles for transforming billboards
    into world space we're piggybacking position, the z-axis and LOD-alpha 
    onto each vertex. Since the Y-axis is always upwards we can reconstruct 
    the 4x4 transform matrix in the vertex shader.
*/
void
TreeLOD::ExtractBillboardGeometry()
{
    n_assert(this->IsBillboard());
    n_assert(0 == this->vertices);
    n_assert(0 == this->indices);
/*
    // get the mesh and mesh group index from the N2 shape node from our transform node
    nShapeNode* shapeNode = (nShapeNode*) this->nebulaNode->GetHead();
    n_assert(shapeNode->IsInstanceOf(nKernelServer::Instance()->FindClass("nshapenode")));
    nMesh2* mesh = shapeNode->GetMeshObject();
    int groupIndex = shapeNode->GetGroupIndex();

    // use a MeshGroupGeometry helper object to extract the vertices and indices 
    // from the mesh
    MeshGroupGeometry geomHelper;
    geomHelper.Setup(mesh, groupIndex);

    // copy indices
    this->numIndices = geomHelper.GetNumIndices();
    SizeT indexBufferSize = this->numIndices * sizeof(ushort);
    this->indices = (ushort*) Memory::Alloc(Memory::ResourceHeap, indexBufferSize);
    Memory::Copy(geomHelper.GetIndices(), this->indices, indexBufferSize);

    // allocate vertex array, we don't need the second uv set and the 
    // vertex colors for the billboard geometry
    this->numVertices = geomHelper.GetNumVertices();
    this->vertexComps = (mesh->GetVertexComponents() & InputVertexComponents) | nMesh2::Vector4_0 | nMesh2::VectorUBN4_0;
    this->vertexWidth = InputVertexWidth + 5;
    this->vertices = (float*) Memory::Alloc(Memory::ResourceHeap, this->numVertices * this->vertexWidth * sizeof(float));

    // copy vertices, expand vertices to with additional instance-info for position, direction and alpha
    float* srcBaseVertexPtr = geomHelper.GetVertices();
    uint srcVertexComps = mesh->GetVertexComponents();
    float* dstVertexPtr = this->vertices;
    SizeT srcVertexWidth = geomHelper.GetVertexWidth();
    IndexT vertexIndex;
    for (vertexIndex = 0; vertexIndex < this->numVertices; vertexIndex++)
    {
        float* srcVertexPtr = srcBaseVertexPtr + vertexIndex * srcVertexWidth;

        // copy coord, normal and uv0
        *dstVertexPtr++ = *srcVertexPtr++; 
        *dstVertexPtr++ = *srcVertexPtr++; 
        *dstVertexPtr++ = *srcVertexPtr++;
        *((uint*)dstVertexPtr++) = *((uint*)srcVertexPtr++);
        *((uint*)dstVertexPtr++) = *((uint*)srcVertexPtr++);

        // skip UV1
        if (srcVertexComps & nMesh2::Uv1S2)
        {
            srcVertexPtr++;
        }

        // skip Color
        if (srcVertexComps & nMesh2::ColorUB4N)
        {
            srcVertexPtr++;
        }

        // copy tangent and binormal
        *((uint*)dstVertexPtr++) = *((uint*)srcVertexPtr++);
        *((uint*)dstVertexPtr++) = *((uint*)srcVertexPtr++);
        
        // write instance position / scale
        *dstVertexPtr++ = 0.0f;
        *dstVertexPtr++ = 0.0f;
        *dstVertexPtr++ = 0.0f;
        *dstVertexPtr++ = 1.0f;

        // write instance direction + alpha as packed UBN4 vector
        *((uint*)dstVertexPtr++) = 0;
    }
    */
}

//------------------------------------------------------------------------------
/**
    Updates the position, direction and alpha components in the vertex
    buffer. The method is called to prepare the vertices for copying
    into the render vertex buffer.
*/
void
TreeLOD::UpdateInstanceData(const matrix44& tform, float fadeAlpha, float scale)
{
    n_assert(this->isBillboard);

    const float4& pos = tform.get_position();
    float4 dir = tform.get_zaxis();
	dir = float4::normalize(dir);

    // direction and alpha as packed vector4
    uint packedDirAndAlpha;
    uchar* ucharPtr = (uchar*) &packedDirAndAlpha;
    *ucharPtr++ = (uchar) (((dir.x() + 1.0f) * 0.5f) * 255.0f);
    *ucharPtr++ = (uchar) (((dir.y() + 1.0f) * 0.5f) * 255.0f);
    *ucharPtr++ = (uchar) (((dir.z() + 1.0f) * 0.5f) * 255.0f);
    *ucharPtr = (uchar) (fadeAlpha * 255.0f);

    IndexT i = InputVertexWidth;
    SizeT end = this->numVertices * this->vertexWidth;
    SizeT step = this->vertexWidth;
    for (; i < end; i += step)
    {
        // write pos as float3 vector
        this->vertices[i]     = pos.x();
        this->vertices[i + 1] = pos.y();
        this->vertices[i + 2] = pos.z();
        this->vertices[i + 3] = scale;
        ((uint*)this->vertices)[i + 4] = packedDirAndAlpha;
    }
}

//------------------------------------------------------------------------------
/**
    Creates a N2 shape node used for rendering the combined billboards
    of this tree lod (only if this is a billboard lod).
*/
void
TreeLOD::SetupBillboardShapeNode()
{
    n_assert(this->IsBillboard());

    /*
    // get the mesh and mesh group index from the N2 shape node from our transform node
    nShapeNode* origShapeNode = (nShapeNode*) this->nebulaNode->GetHead();
    n_assert(origShapeNode->IsInstanceOf(nKernelServer::Instance()->FindClass("nshapenode")));

    nKernelServer* kernelServer = nKernelServer::Instance();
    nRoot* rsrcPool = Foundation::Server::Instance()->GetResourcePool(Foundation::Server::GraphicsPool);
    kernelServer->PushCwd(rsrcPool);
    nRoot* category = rsrcPool->Find("treelods");
    if (0 == category)
    {
        category = kernelServer->New("nroot", "treelods");
    }
    kernelServer->PopCwd();
    kernelServer->PushCwd(category);
    nShapeNode* shapeNode = (nShapeNode*) origShapeNode->Clone(nString::FromInt(TreeLOD::uniqueId++).AsCharPtr());
    shapeNode->SetMesh("");
    shapeNode->SetShader("treebillboard");
    shapeNode->LoadResources();
    kernelServer->PopCwd();
    this->billboardShapeNode = shapeNode;
    */
}

} // namespace Forest
