#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::TreeLOD

    Describes a single LOD level of a tree.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "models/nodes/shapenode.h"
#include "models/modelinstance.h"

//------------------------------------------------------------------------------
namespace Forest
{
class TreeLOD : public Core::RefCounted
{
    __DeclareClass(TreeLOD);
public:
    /// constructor
    TreeLOD();
    /// destructor
    ~TreeLOD();

    /// setup the object
    void Setup(float minDist, float maxDist, const Ptr<Models::ModelNode>& node, const Util::StringAtom& nodePath, bool billboard, IndexT lodLevel);
    /// discard the object
    void Discard();
    /// return true if object is valid
    bool IsValid() const;
    /// return true if distance is between min/max distance
    bool IsInRange(float dist) const;
    /// get billboard flag
    bool IsBillboard() const;

    /// get path to the nebula3 model node
    const Util::StringAtom& GetNodePath() const;
    /// get LOD level
    IndexT GetLodLevel() const;

    /// write instancing data to the vertex buffer
    void UpdateInstanceData(const Math::matrix44& tform, float fadeAlpha, float scale);
    /// get number of billboard vertices
    ushort GetNumVertices() const;
    /// get pointer to billboard vertices (x, y, z)
    const float* GetVertices() const;
    /// get number of billboard indices
    SizeT GetNumIndices() const;
    /// get pointer to billboard indices
    const ushort* GetIndices() const;
    /// get billboard vertex components
    uint GetVertexComponents() const;
    /// get billboard vertex width
    SizeT GetVertexWidth() const;
    /// get uv coordinates for tree type
    const Math::float2& GetUV() const;
    /// get the billboard N3 shape node
    const Ptr<Models::ShapeNode>& GetBillboardShapeNode() const;
    /// get the billboard N3 model instance
    const Ptr<Models::ModelInstance>& GetBillboardInstance() const;

private:
    /// extract billboard geometry from nebula node
    void ExtractBillboardGeometry();
    /// setup the N2 shape node for rendering billboard of this tree lod
    void SetupBillboardShapeNode();

    static IndexT uniqueId;

    static const int InputVertexWidth = 7;
    //static const uint InputVertexComponents = (nMesh2::Coord | nMesh2::NormalUB4N | nMesh2::TangentUB4N | nMesh2::BinormalUB4N | nMesh2::Uv0S2);

    bool isValid;

    float minDist;
    float maxDist;
    Util::StringAtom nodePath;
    IndexT lodLevel;

    Ptr<Models::ShapeNode> billboardShapeNode;
    Ptr<Models::ModelInstance> billboardModelInstance;
    ushort numVertices;
    float* vertices;
    SizeT numIndices;
    ushort* indices;
    uint vertexComps;
    SizeT vertexWidth;
    bool isBillboard;
    Math::float2 uv; 
};

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeLOD::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeLOD::IsInRange(float dist) const
{
    return (dist >= this->minDist) && (dist < this->maxDist);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
TreeLOD::GetNodePath() const
{
    n_assert(this->nodePath.IsValid());
    return this->nodePath;
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
TreeLOD::GetLodLevel() const
{
    return this->lodLevel;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeLOD::IsBillboard() const
{
    return this->isBillboard;
}

//------------------------------------------------------------------------------
/**
*/
inline ushort
TreeLOD::GetNumVertices() const
{
    n_assert(this->isBillboard);
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline const float*
TreeLOD::GetVertices() const
{
    n_assert(this->isBillboard);
    return this->vertices;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
TreeLOD::GetNumIndices() const
{
    n_assert(this->isBillboard);
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline const ushort*
TreeLOD::GetIndices() const
{
    n_assert(this->isBillboard);
    return this->indices;
}

//------------------------------------------------------------------------------
/**
*/
inline uint
TreeLOD::GetVertexComponents() const
{
    n_assert(this->isBillboard);
    return this->vertexComps;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
TreeLOD::GetVertexWidth() const
{
    n_assert(this->isBillboard);
    return this->vertexWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Models::ShapeNode>&
TreeLOD::GetBillboardShapeNode() const
{
    n_assert(this->isBillboard);
    n_assert(this->billboardShapeNode.isvalid());
    return this->billboardShapeNode;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Models::ModelInstance>&
TreeLOD::GetBillboardInstance() const
{
    n_assert(this->isBillboard);
    n_assert(this->billboardModelInstance.isvalid());
    return this->billboardModelInstance;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float2&
TreeLOD::GetUV() const
{
    n_assert(this->isBillboard);
    return this->uv;
}

} // namespace Forest
//------------------------------------------------------------------------------
