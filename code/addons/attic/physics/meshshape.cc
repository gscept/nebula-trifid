//------------------------------------------------------------------------------
//  physics/meshshape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/meshshape.h"
#include "coregraphics/legacy/nvx2streamreader.h"
#include "physics/meshcache.h"
#include "physics/physicsserver.h"
#include "debugrender/debugshaperenderer.h"

namespace Physics
{
__ImplementClass(Physics::MeshShape, 'PMSS', Physics::Shape);

using namespace Math;
using namespace Debug;

Opcode::SphereCollider MeshShape::opcSphereCollider;
dGeomID MeshShape::odeRayGeomId = 0;
dContactGeom MeshShape::rayCheckContactGeom;

//------------------------------------------------------------------------------
/**
*/
MeshShape::MeshShape() :
    Shape(Mesh),
    meshGroupIndex(0),
    odeTriMeshDataId(0),
	calculateMass(false)
{
    // create and initialize the static ray geom
    if (0 == MeshShape::odeRayGeomId)
    {
        MeshShape::odeRayGeomId = dCreateRay(0, 1.f); 
        dGeomRaySetParams(MeshShape::odeRayGeomId, 1, 1); 
        dGeomRaySetClosestHit(MeshShape::odeRayGeomId, 1);
    }

    // initialize the OPCODE sphere collider
    opcSphereCollider.SetFirstContact(false);       // report all contacts
    opcSphereCollider.SetTemporalCoherence(true);   // use temporal coherence
}

//------------------------------------------------------------------------------
/**
*/
MeshShape::~MeshShape()
{
    if (this->IsAttached())
    {
        this->Detach();
    }
}

//------------------------------------------------------------------------------
/**
    Create a mesh shape object, add it to ODE's collision space, and initialize
    the mass member.
*/
bool
MeshShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {  
        // fix my collide bits, we don't need to collide against other static and disabled entities
        this->SetCategoryBits(Static);
        this->SetCollideBits(0);

        if (!filename.IsEmpty())
        {
            // get shared mesh
            this->sharedMesh = MeshCache::Instance()->NewMesh(this->filename);
        }
        n_assert(0 != this->sharedMesh);

        // create an ODE TriMeshData object from the loaded vertices and indices
        this->odeTriMeshDataId = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSingle(this->odeTriMeshDataId,
                                    this->GetVertexBuffer(),
                                    this->GetVertexByteSize(),
                                    this->GetNumVertices(),
                                    this->GetIndexBuffer(),
                                    this->GetNumIndices(),
                                    3 * sizeof(int));
        this->odeTriMeshID = dCreateTriMesh(0, odeTriMeshDataId, 0, 0, 0);
        this->AttachGeom(this->odeTriMeshID, spaceId);
		if(calculateMass)
			dMassSetTrimesh(&(this->odeMass), Physics::MaterialTable::GetDensity(this->materialType), odeTriMeshID);

        // FIXME: apply shape mass here!
		if(dMassCheck(&odeMass) != 1)
		{
			odeMass.setBox(1,1,1,1);
		}

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    - 14-Jan-05 floh    memleak fixed, TriMeshData was not released
*/
void
MeshShape::Detach()
{
    n_assert(this->IsAttached());

    this->sharedMesh->Unload(); 

    dGeomTriMeshDataDestroy(this->odeTriMeshDataId);
    this->sharedMesh = 0;

    Shape::Detach();
}

//------------------------------------------------------------------------------
/**
    Does a sphere collision check. Returns the number of and the indices
    of all faces inside or intersecting the sphere.

    @param  s                   [in] the sphere in world space
    @param  outFaceIndices      [out] will be filled with pointer to face indices
    @return numFaces            number of collision faces
*/
int
MeshShape::DoSphereCollide(const sphere& s, uint*& outFaceIndices)
{
    // setup sphere
    IceMaths::Sphere opcSphere(IceMaths::Point(s.p.x(), s.p.y(), s.p.z()), s.r);

    // get pointer to our opc model
    Opcode::Model* opcModel = PhysicsServer::Instance()->GeomTriMeshGetOpcodeModel(this->odeTriMeshID);
    n_assert(opcModel);

    // setup identity matrix for sphere
    IceMaths::Matrix4x4 sphereTransform;
    sphereTransform.Identity();

    // get matrix for our model
    IceMaths::Matrix4x4* modelTransform = (IceMaths::Matrix4x4*) &(this->transform);

    // do sphere collision check
    bool success = opcSphereCollider.Collide(this->opcSphereCache, opcSphere, *opcModel, &sphereTransform, modelTransform);
    n_assert(success);
    int numFaces = opcSphereCollider.GetNbTouchedPrimitives();
    if (numFaces > 0)
    {
        outFaceIndices = (uint*) opcSphereCollider.GetTouchedPrimitives();
    }
    else
    {
        outFaceIndices = 0;
    }
    return numFaces;
}

//------------------------------------------------------------------------------
/**
    Do a closest-hit ray check on the shape.
*/
bool
MeshShape::DoRayCheck(const Math::line& l, Math::vector& contact)
{
    n_assert(0 < l.length());
    n_assert(0 != MeshShape::odeRayGeomId);

    const Math::vector& pos = l.start();
    Math::vector dir = l.vec();
    dir = vector::normalize(dir);

    // setup the ray
    dGeomRaySetLength(MeshShape::odeRayGeomId, l.length());
    dGeomRaySet(MeshShape::odeRayGeomId, pos.x(), pos.y(), pos.z(), dir.x(), dir.y(), dir.z());

    int result = dCollide(this->GetGeomId(), MeshShape::odeRayGeomId, 1, &MeshShape::rayCheckContactGeom, sizeof(MeshShape::rayCheckContactGeom));
    if(0 != result)
    {
        contact.set(MeshShape::rayCheckContactGeom.pos[0], MeshShape::rayCheckContactGeom.pos[1], MeshShape::rayCheckContactGeom.pos[2]);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render debug visualization of the mesh.  
*/
void
MeshShape::RenderDebug(const Math::matrix44& t)
{
    if (this->IsAttached())
    {        
        Math::matrix44 m = matrix44::multiply(this->GetTransform(), t);
        DebugShapeRenderer::Instance()->DrawIndexedPrimitives(m, CoreGraphics::PrimitiveTopology::TriangleList, 
                                                              this->GetNumIndices() / 3,
                                                              this->GetVertexBuffer(),
                                                              this->GetNumVertices(),
                                                              this->GetVertexByteSize()/4,
                                                              this->GetIndexBuffer(),
                                                              CoreGraphics::IndexType::Index32,                                                            
                                                              this->GetDebugVisualizationColor());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MeshShape::Destroy()
{
    if (MeshShape::odeRayGeomId != 0)
    {
        dGeomDestroy(MeshShape::odeRayGeomId);
    }
}

} // namespace Physics