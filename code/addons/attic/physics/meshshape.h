#ifndef PHYSICS_MESHSHAPE_H
#define PHYSICS_MESHSHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::MeshShape

    An shape containing a triangle mesh.
    
    (C) 2003 RadonLabs GmbH
*/
#include "physics/shape.h"
#include "util/string.h"
#include "math/sphere_.h"
#include "math/line.h"
#include "physics/physicsmesh.h"
#define BAN_OPCODE_AUTOLINK
#include "opcode/opcode.h"

//------------------------------------------------------------------------------
namespace Physics
{
class MeshShape : public Shape
{
    __DeclareClass(MeshShape);
public:
    /// constructor
    MeshShape();
    /// destructor
    virtual ~MeshShape();
    /// render debug visualization
    virtual void RenderDebug(const Math::matrix44& t);
    /// attach the shape to the world
    virtual bool Attach(dSpaceID spaceId);
    /// detach the shape from the world
    virtual void Detach();
    /// set the mesh filename
    void SetFilename(const Util::String& name);
    /// get the mesh filename
    const Util::String& GetFilename() const;
    /// set mesh group index
    void SetMeshGroupIndex(int i);
    /// get mesh group index
    int GetMeshGroupIndex() const;
    /// access to vertex buffer
    const float* GetVertexBuffer() const;
    /// access to index buffer
    const int* GetIndexBuffer() const;
	/// access to mesh
	Physics::PhysicsMesh* GetMesh() const; 
    /// set mesh directly
	void SetMesh(Physics::PhysicsMesh* mesh); 
    /// get number of vertices
    int GetNumVertices() const;
    /// get vertex width
    int GetVertexByteSize() const;
    /// get number of indices
    int GetNumIndices() const;
    /// do a sphere collide check, returns number of and indices of contained faces
    int DoSphereCollide(const Math::sphere& s, uint*& outFaceIndices);
    /// do a first-hit raycheck
    bool DoRayCheck(const Math::line& l, Math::vector& contact);

    /// cleanup the ode Ray Geom
    static void Destroy();

	void CalculateMass(bool);

protected:
    friend class Ray;

    /// get ODE TriMeshDataID
    dTriMeshDataID GetOdeTriMeshDataId() const;
    /// get ODE TriMeshID
    dGeomID GetOdeTriMeshID() const;

    Util::String filename;
    int meshGroupIndex;
    Ptr<Physics::PhysicsMesh> sharedMesh;
    dTriMeshDataID odeTriMeshDataId;
    dGeomID odeTriMeshID;

    static dGeomID odeRayGeomId;
    static dContactGeom rayCheckContactGeom;

    static Opcode::SphereCollider opcSphereCollider;    // an OPCODE sphere collider
    Opcode::SphereCache opcSphereCache;                 // an OPCODE sphere cache
	bool calculateMass;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
MeshShape::SetMeshGroupIndex(int i)
{
    this->meshGroupIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
MeshShape::GetMeshGroupIndex() const
{
    return this->meshGroupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
Physics::PhysicsMesh*
MeshShape::GetMesh() const
{
	return this->sharedMesh;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
MeshShape::SetMesh(Physics::PhysicsMesh* mesh)
{
    n_assert(0 != mesh);
    this->sharedMesh = mesh;
}

//------------------------------------------------------------------------------
/**
*/
inline
dTriMeshDataID
MeshShape::GetOdeTriMeshDataId() const
{
    return this->odeTriMeshDataId;
}

//------------------------------------------------------------------------------
/**
*/
inline
dGeomID
MeshShape::GetOdeTriMeshID() const
{
    return this->odeTriMeshID;
}

//------------------------------------------------------------------------------
/**
*/
inline
const float*
MeshShape::GetVertexBuffer() const
{
    return this->sharedMesh->GetVertexPointer();
}

//------------------------------------------------------------------------------
/**
*/
inline
const int*
MeshShape::GetIndexBuffer() const
{
    return this->sharedMesh->GetGroupIndexPointer(this->meshGroupIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
MeshShape::GetNumVertices() const
{
    return this->sharedMesh->GetNumVertices();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
MeshShape::GetVertexByteSize() const
{
    return this->sharedMesh->GetVertexByteSize();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
MeshShape::GetNumIndices() const
{
    return this->sharedMesh->GetGroupNumIndices(this->meshGroupIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
MeshShape::SetFilename(const Util::String& name)
{
    this->filename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
MeshShape::GetFilename() const
{
    return this->filename;
}

inline 
void MeshShape::CalculateMass(bool inCalc)
{
	calculateMass = inCalc;
}

}; // namespace OdePhysics
//------------------------------------------------------------------------------
#endif

    