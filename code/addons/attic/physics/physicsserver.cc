//------------------------------------------------------------------------------
//  physics/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/physicsserver.h"
#include "physics/level.h"
#include "physics/composite.h"
#include "physics/rigidbody.h"
#include "physics/boxshape.h"
#include "physics/sphereshape.h"
#include "physics/capsuleshape.h"
#include "physics/meshshape.h"
#include "physics/ray.h"
#include "physics/hingejoint.h"
#include "physics/universaljoint.h"
#include "physics/sliderjoint.h"
#include "physics/balljoint.h"
#include "physics/hinge2joint.h"
#include "physics/amotor.h"
#include "physics/meshcache.h"
#include "io/ioserver.h"
#include "util/string.h"
#include "coregraphics/shaperenderer.h"
#include "physics/areaimpulse.h"
#include "core/factory.h"
#include "src/collision_kernel.h"
#include "ode/ode.h"
#include "ode/collision_trimesh.h"
#define TRIMESH_INTERNAL
#include "src/collision_trimesh_internal.h"

namespace Physics
{
__ImplementClass(Physics::PhysicsServer, 'PSRV', Core::RefCounted);
__ImplementSingleton(Physics::PhysicsServer);

using namespace Math;

uint PhysicsServer::UniqueStamp = 0;

//------------------------------------------------------------------------------
/**
*/
PhysicsServer::PhysicsServer() :
    isOpen(false),
    time(0.0),
    contactPoints(256, 256)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
PhysicsServer::~PhysicsServer()
{
    n_assert(!this->isOpen);
    n_assert(this->curLevel == 0);
    
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
dWorldID
PhysicsServer::GetOdeWorldId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeWorldId();
}

//------------------------------------------------------------------------------
/**
*/
dSpaceID
PhysicsServer::GetOdeStaticSpaceId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeStaticSpaceId();
}

//------------------------------------------------------------------------------
/**
*/
dSpaceID
PhysicsServer::GetOdeDynamicSpaceId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeDynamicSpaceId();
}

//------------------------------------------------------------------------------
/**
*/
dSpaceID
PhysicsServer::GetOdeCommonSpaceId() const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetOdeCommonSpaceId();
}

//------------------------------------------------------------------------------
/**
*/
dSpaceID
PhysicsServer::GetScratchSpaceSpaceID(unsigned int id) const
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->GetScratchSpaceSpaceID(id);
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
PhysicsServer::CreateScratchSpace()
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->CreateScratchSpace();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysicsServer::RemoveScratchSpace(unsigned int id)
{
    n_assert(this->curLevel.isvalid());
    return this->curLevel->RemoveScratchSpace(id);
}

//------------------------------------------------------------------------------
/**
    Set the current physics level. The refcount of the level will be 
    incremented, the refcount of the previous level will be decremented
    (if exists). A 0 pointer is valid and will just release the
    previous level.

    @param  level   pointer to a Physics::Level object
*/
void
PhysicsServer::SetLevel(Level* level)
{
    n_assert(this->isOpen);
    if (this->curLevel != 0)
    {
        this->curLevel->OnDeactivate();
        this->curLevel = 0;
    }
    if (level)
    {
        this->curLevel = level;
        this->curLevel->OnActivate();
    }
}

//------------------------------------------------------------------------------
/**
    Return pointer to current level.

    @return     pointer to a Physics::Level object
*/
Level*
PhysicsServer::GetLevel() const
{
    return this->curLevel.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Initialize the physics subsystem.

    - 25-May-05 jo   Don't create default physics level.

    @return     true if physics subsystem initialized successfully
*/
bool
PhysicsServer::Open()
{
    n_assert(!this->isOpen);

    // setup the material table
    MaterialTable::Setup();

    // setup mesh cache
    this->meshCache = MeshCache::Create();

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the physics subsystem.
*/
void
PhysicsServer::Close()
{
    n_assert(this->isOpen);
    this->SetLevel(0);
    this->meshCache = 0;
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Perform one or more simulation steps. The number of simulation steps
    performed depends on the time of the last call to Trigger().
*/
void
PhysicsServer::Trigger()
{
    n_assert(this->isOpen);
    if (this->curLevel != 0)
    {
        // trigger the level
        this->curLevel->SetTime(this->time);
        this->curLevel->Trigger();
    }
}

//------------------------------------------------------------------------------
/**
    Set the current point of interest for the physics subsystem. This can
    be for instance the position of the game entity which has the input focus.
    Only the area around this point of interest should be simulated.
*/
void
PhysicsServer::SetPointOfInterest(const Math::vector& p)
{
    n_assert(this->curLevel != 0);
    this->curLevel->SetPointOfInterest(p);
}

//------------------------------------------------------------------------------
/**
    Get the current point of interest.
*/
const Math::vector&
PhysicsServer::GetPointOfInterest() const
{
    n_assert(this->curLevel != 0);
    return this->curLevel->GetPointOfInterest();
}

//------------------------------------------------------------------------------
/**
    Do a ray check starting from position `pos' along direction `dir'.
    Make resulting intersection points available in `GetIntersectionPoints()'.
*/
bool
PhysicsServer::RayCheck(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet)
{
    const static Math::matrix44 identity = Math::matrix44::identity();
    this->contactPoints.Clear();
    this->ray.SetOrigin(pos);
    this->ray.SetVector(dir);
    this->ray.SetExcludeFilterSet(excludeSet);
    this->ray.DoRayCheckAllContacts(identity, this->contactPoints);
    return this->contactPoints.Size() > 0;
}

//------------------------------------------------------------------------------
/**
    Create a new Composite object.

    @return     pointer to a new Composite object
*/
Composite*
PhysicsServer::CreateComposite() const
{
    return Composite::Create();
}

//------------------------------------------------------------------------------
/**
    Create a new Ragdoll object. Ragdolls are a specialization of 
    class Composite.

    @return     pointer to a new Composite object
*/
Ragdoll*
PhysicsServer::CreateRagdoll() const
{
    return 0;// TODO :Ragdoll::Create();
}

//------------------------------------------------------------------------------
/**
    Create a new RigidBody object. This method is normally
    overwritten by API specific subclasses of Physics::PhysicsServer.

    @return     pointer to a new RigidBody object
*/
RigidBody*
PhysicsServer::CreateRigidBody() const
{
    return RigidBody::Create();
}

//------------------------------------------------------------------------------
/**
    Create a HingeJoint object.
*/
HingeJoint*
PhysicsServer::CreateHingeJoint() const
{
    return HingeJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create a Hinge2Joint object.
*/
Hinge2Joint*
PhysicsServer::CreateHinge2Joint() const
{
    return Hinge2Joint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an UniversalJoint object.
*/
UniversalJoint*
PhysicsServer::CreateUniversalJoint() const
{
    return UniversalJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an SliderJoint object.
*/
SliderJoint*
PhysicsServer::CreateSliderJoint() const
{
    return SliderJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an BallJoint object.
*/
BallJoint*
PhysicsServer::CreateBallJoint() const
{
    return BallJoint::Create();
}

//------------------------------------------------------------------------------
/**
    Create an AMotor object.
*/
AMotor*
PhysicsServer::CreateAMotor() const
{
    return AMotor::Create();
}

//------------------------------------------------------------------------------
/**
    Create a new fully initialized box shape object.

    @param  m           locale transformation of shape
    @param  matType     MaterialType of the shape
    @param  size        size of the box
    @return             pointer to new box shape object
*/
BoxShape*
PhysicsServer::CreateBoxShape(const Math::matrix44& m, MaterialType matType, const Math::vector& size) const
{
	// first remove scaling from transformation matrix
	matrix44 pure = matrix44::identity();
	pure.set_position(m.get_position());
	pure.set_xaxis(float4::normalize(m.get_xaxis()));
	pure.set_yaxis(float4::normalize(m.get_yaxis()));
	pure.set_zaxis(float4::normalize(m.get_zaxis()));

    BoxShape* boxShape = BoxShape::Create();
	boxShape->SetTransform(pure);
    boxShape->SetMaterialType(matType);
    boxShape->SetSize(size);
    return boxShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized sphere shape object.

    @param  m           locale transformation of shape
    @param  matType     MaterialType of the shape
    @param  radius      radius of sphere
    @return             pointer to new sphere shape object    
*/
SphereShape*
PhysicsServer::CreateSphereShape(const Math::matrix44& m, MaterialType matType, float radius) const
{
    SphereShape* sphereShape = SphereShape::Create();
    sphereShape->SetTransform(m);
    sphereShape->SetMaterialType(matType);
    sphereShape->SetRadius(radius);
    return sphereShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized capsule shape object.

    @param  m           locale transform of shape
    @param  matType     MaterialType of shape
    @param  radius      radius of capsule
    @param  length      lenght of capsule (not counting the caps)
    @return             pointer to a new CapsuleShape object
*/
CapsuleShape*
PhysicsServer::CreateCapsuleShape(const Math::matrix44& m, MaterialType matType, float radius, float length) const
{
    CapsuleShape* capsuleShape = CapsuleShape::Create();
    capsuleShape->SetTransform(m);
    capsuleShape->SetMaterialType(matType);
    capsuleShape->SetRadius(radius);
    capsuleShape->SetLength(length);
    return capsuleShape;
}

//------------------------------------------------------------------------------
/**
    Create a fully initialized mesh shape object.

    @param  m               locale transformation of shape
    @param  matType         MaterialType of the shape
    @param  meshFilename    filename of mesh resource file
    @param  meshGroupIndex  the mesh group index in the mesh file (set to 0 if unsure)
    @return                 pointer to new mesh shape object
*/
MeshShape*
PhysicsServer::CreateMeshShape(const Math::matrix44& m, MaterialType matType, const Util::String& meshFilename, int meshGroupIndex) const
{
    MeshShape* meshShape = MeshShape::Create();
    meshShape->SetTransform(m);
    meshShape->SetMaterialType(matType);
    meshShape->SetFilename(meshFilename);
    meshShape->SetMeshGroupIndex(meshGroupIndex);
    return meshShape;
}

//------------------------------------------------------------------------------
/**
    Create a ray object.

    @param  orig    origin of ray in local space
    @param  vec     direction/length vector in local space
    @return         pointer to new ray object
*/
Physics::Ray*
PhysicsServer::CreateRay(const Math::vector& orig, const Math::vector& vec) const
{
    Ray* ray = Ray::Create();
    ray->SetOrigin(orig);
    ray->SetVector(vec);
    return ray;
}

//------------------------------------------------------------------------------
/**
    Shoots a 3d ray into the world and returns the closest contact.

    @param  pos         starting position of ray
    @param  dir         direction and length of ray
    @param  exludeSet   filter set defining objects to exclude
    @return             pointer to closest ContactPoint, or 0 if no contact detected
*/
const ContactPoint*
PhysicsServer::GetClosestContactAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet)
{
    // do the actual ray check (returns all contacts)
    this->RayCheck(pos, dir, excludeSet);
    
    // find closest contact
    const Util::Array<ContactPoint>& contacts = this->GetContactPoints();
    int closestContactIndex = -1;
    float closestDistance = dir.length();;
    int i;
    int numContacts = contacts.Size();
    Math::vector distVec;
    for (i = 0; i < numContacts; i++)
    {
        const ContactPoint& curContact = contacts[i];
        distVec = curContact.GetPosition() - pos;
        float dist = distVec.length();
        if (dist < closestDistance)
        {
            closestContactIndex = i;
            closestDistance = dist;
        }
    }
    if (closestContactIndex != -1)
    {
        return &contacts[closestContactIndex];
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    Shoots a 3d ray through the current mouse position and returns the
    closest contact, or a null pointer if no contact.
    NOTE: This gets the current view matrix from the Nebula gfx server.
    This means the check could be one frame of, if the "previous" view matrix
    is used.

    @param  mousePos        the current 2d mouse position (or any other 2d screen pos)
    @param  length          length of ray through mouse
    @param  excludeSet      filter set defining which objects to exclude
    @return                 pointer to closest ContactPoint or 0 if no contact detected
*/
const ContactPoint*
PhysicsServer::GetClosestContactUnderMouse(const Math::line& worldMouseRay, const FilterSet& excludeSet)
{
    return this->GetClosestContactAlongRay(worldMouseRay.start(), worldMouseRay.vec(), excludeSet);
}

//------------------------------------------------------------------------------
/**
    Apply an impulse on the first rigid body which lies along the defined ray.
*/
bool
PhysicsServer::ApplyImpulseAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, float impulse)
{
    const ContactPoint* contactPoint = this->GetClosestContactAlongRay(pos, dir, excludeSet);
    if (contactPoint)
    {
        RigidBody* rigidBody = contactPoint->GetRigidBody();
        if (rigidBody)
        {
            Math::vector normDir = dir;
            normDir = vector::normalize(normDir);
            rigidBody->ApplyImpulseAtPos(contactPoint->GetPosition(), normDir * impulse);
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Apply an area impulse to the world.
*/
void
PhysicsServer::ApplyAreaImpulse(AreaImpulse* impulse)
{
    n_assert(impulse);
    impulse->Apply();
}

//------------------------------------------------------------------------------
/**
    Renders the debug visualization of the level.
*/
void
PhysicsServer::RenderDebug()
{
    if (this->curLevel != 0)
    {
        this->curLevel->RenderDebug();
    }    
}

//------------------------------------------------------------------------------
/**
    This method returns all physics entities touching the given spherical 
    area. The method creates a sphere shape and calls its collide
    method, so it's quite fast. Note that entities will be appended to the
    array, so usually you should make sure to pass an empty array. This method
    will also overwrite the internal contactPoints array which can be 
    queried after the method has returned, but note that there will only
    be one contact per physics shape.

    @param  pos         center of the sphere
    @param  radius      radius of the sphere
    @param  excludeSet  what contacts should be ignored?
    @param  result      array which will be filled with entity pointers
    @return             number of entities touching the sphere
*/
int
PhysicsServer::GetEntitiesInSphere(const Math::vector& pos, float radius, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result)
{
    n_assert(radius >= 0.0f);
    n_assert(this->GetLevel());
    int oldResultSize = result.Size();

    // create a sphere shape and perform collision check
    Math::matrix44 m = Math::matrix44::identity();
    m.translate(pos);
    Ptr<SphereShape> sphereShape = this->CreateSphereShape(m, MaterialTable::StringToMaterialType("Wood"), radius);
    sphereShape->Attach(this->GetLevel()->GetOdeDynamicSpaceId());
    this->contactPoints.Reset();
    sphereShape->Collide(excludeSet, this->contactPoints);
    sphereShape->Detach();

    // convert contacts to unique entities
    uint stamp = this->GetUniqueStamp();
    int i;
    int numContacts = this->contactPoints.Size();
    for (i = 0; i < numContacts; i++)
    {
        PhysicsEntity* entity = this->contactPoints[i].GetPhysicsEntity();
        if (entity && (entity->GetStamp() != stamp))
        {
            entity->SetStamp(stamp);
            result.Append(entity);
        }
    }
    return result.Size() - oldResultSize;
}
//------------------------------------------------------------------------------
/**
    This method returns all physics entities touching the given box shaped 
    area. The method creates a box shape and calls its collide
    method, so it's quite fast. Note that entities will be appended to the
    array, so usually you should make sure to pass an empty array. This method
    will also overwrite the internal contactPoints array which can be 
    queried after the method has returned, but note that there will only
    be one contact per physics shape.

    @param  pos         center of box
    @param  scale       box scaling
    @param  excludeSet  what contacts should be ignored?
    @param  result      array which will be filled with entity pointers
    @return             number of entities touching the box
*/
int
PhysicsServer::GetEntitiesInBox(const Math::vector& scale, const Math::matrix44& m, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result)
{
    n_assert(scale.x() >= 0 && scale.y() >= 0 && scale.z() >= 0);
    n_assert(this->GetLevel());
    int oldResultSize = result.Size();

    // create a box shape and perform collision check
    Math::matrix44 mNew = Math::matrix44::identity();
    mNew.set_xaxis(m.get_xaxis() * (1/scale.x()));
    mNew.set_yaxis(m.get_yaxis() * (1/scale.y()));
    mNew.set_zaxis(m.get_zaxis() * (1/scale.z()));
    mNew.set_position(m.get_position());

    Ptr<BoxShape> boxShape = this->CreateBoxShape(mNew, MaterialTable::StringToMaterialType("Wood"), scale);
    boxShape->Attach(this->GetLevel()->GetOdeDynamicSpaceId());
    this->contactPoints.Reset();
    boxShape->Collide(excludeSet, this->contactPoints);
    boxShape->Detach();

    // convert contacts to unique entities
    uint stamp = this->GetUniqueStamp();
    int i;
    int numContacts = this->contactPoints.Size();
    for (i = 0; i < numContacts; i++)
    {
        PhysicsEntity* entity = this->contactPoints[i].GetPhysicsEntity();
        if (entity && (entity->GetStamp() != stamp))
        {
            entity->SetStamp(stamp);
            result.Append(entity);
        }
    }
    return result.Size() - oldResultSize;
}

//------------------------------------------------------------------------------
/**
    This method computes a ray in 3d space thru the mouse position.
    It uses the projection and view matrix from the cam of the view set.

    @param  mousePos    2d screen position of mouse
    @param  scale       length of ray cast into the 3d world
*/
//Math::line 
//PhysicsServer::ComputeWorldMouseRay(const float2& mousePos, float length)
//{
//    n_assert(Graphics::GraphicsServer::HasInstance());
//    n_assert(this->graphicsView.isvalid());
//    const Ptr<Graphics::CameraEntity>& cam = this->graphicsView->GetCameraEntity();
//    n_assert(cam.isvalid());
//    const matrix44& view = matrix44::inverse(cam->GetViewTransform());
//    matrix44 invProj = matrix44::inverse(cam->GetProjTransform());
//
//    return Graphics::GraphicsServer::Instance()->ComputeWorldMouseRay(mousePos, length, view, invProj, cam->GetNearClipPlane());
//}

//------------------------------------------------------------------------------
/**
*/
Opcode::Model* 
PhysicsServer::GeomTriMeshGetOpcodeModel(const dGeomID& g)
{
	dUASSERT(g && g->type == dTriMeshClass, "argument not a trimesh");
	dxTriMesh* Geom = (dxTriMesh*)g;
    dUASSERT(Geom->Data, "Data element not initialized");
    return &(Geom->Data->BVTree);
}

} // namespace Physics