#ifndef PHYSICS_PHYSICSMESH_H
#define PHYSICS_PHYSICSMESH_H
//------------------------------------------------------------------------------
/**
    @class Physics::PhysicsMesh
    
    Holds the geometry data for a collide mesh. Meshes are usually cached in 
    the MeshCache, so that identical meshes are only loaded once.
    
    (C) 2006 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "coregraphics/primitivegroup.h"

//------------------------------------------------------------------------------
namespace Physics
{
class PhysicsMesh : public Core::RefCounted
{
    __DeclareClass(PhysicsMesh);
public:
    /// constructor
    PhysicsMesh();
    /// destructor
    virtual ~PhysicsMesh();
    /// set filename of mesh
    void SetFilename(const Util::String& n);
    /// get filename of mesh
    const Util::String& GetFilename() const;
    /// load the mesh data
    bool Load();
    /// unload the mesh data
    void Unload();
    /// return true if mesh data is loaded
    bool IsLoaded() const;
    /// get number of mesh groups
    int GetNumGroups() const;
    /// mesh group at index
    const CoreGraphics::PrimitiveGroup& GetGroupAt(int index) const;
    /// get overall number of vertices
    int GetNumVertices() const;
    /// get overall number of indices
    int GetNumIndices() const;
    /// get vertex byte size
    int GetVertexByteSize() const;
    /// get pointer to complete vertex buffer
    float* GetVertexPointer() const;
    /// get pointer to index buffer
    int* GetIndexPointer() const;
    /// get number of vertices in group
    int GetGroupNumVertices(int groupIndex) const;
    /// get number of indices in group
    int GetGroupNumIndices(int groupIndex) const;
    /// get pointer to first vertex in group
    float* GetGroupVertexPointer(int groupIndex) const;
    /// get pointer to first index in group
    int* GetGroupIndexPointer(int groupIndex) const;

private:
    /// update the group bounding boxes (slow!)
    void UpdateGroupBoundingBoxes();

    Util::String filename;
    SizeT numVertices;
    SizeT numIndices;
    SizeT vertexByteSize;
    SizeT vertexNumFloats;
    float* vertexData;
    int* indexData;
    Util::Array<CoreGraphics::PrimitiveGroup> meshGroups;    
    bool isLoaded;
};

//------------------------------------------------------------------------------
/**
*/
inline void
PhysicsMesh::SetFilename(const Util::String& n)
{
    this->filename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
PhysicsMesh::GetFilename() const
{
    return this->filename;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
PhysicsMesh::IsLoaded() const
{
    return this->isLoaded;
}

//------------------------------------------------------------------------------
/**
*/
inline int
PhysicsMesh::GetNumGroups() const
{
    n_assert(this->isLoaded);
    return this->meshGroups.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const CoreGraphics::PrimitiveGroup& 
PhysicsMesh::GetGroupAt(int index) const
{
    n_assert(this->isLoaded);
    return this->meshGroups[index];
}

//------------------------------------------------------------------------------
/**
*/
inline int
PhysicsMesh::GetNumVertices() const
{
    n_assert(this->isLoaded);
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline int
PhysicsMesh::GetNumIndices() const
{
    n_assert(this->isLoaded);
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline int
PhysicsMesh::GetVertexByteSize() const
{
    n_assert(this->isLoaded);
    return this->vertexByteSize;
}

//------------------------------------------------------------------------------
/**
*/
inline float*
PhysicsMesh::GetVertexPointer() const
{
    n_assert(this->isLoaded);
    n_assert(0 != this->vertexData);
    return this->vertexData;
}

//------------------------------------------------------------------------------
/**
*/
inline int*
PhysicsMesh::GetIndexPointer() const
{
    n_assert(this->isLoaded);
    n_assert(0 != this->indexData);
    return this->indexData;
}

}
//------------------------------------------------------------------------------
#endif
