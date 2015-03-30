#ifndef PHYSICS_SERVER_H
#define PHYSICS_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Physics::PhysicsServer

    The physics subsystem server object.

    (C) 2003 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "core/singleton.h"
#include "physics/physicsentity.h"
#include "physics/rigidbody.h"
#include "physics/contactpoint.h"
#include "physics/filterset.h"
#include "util/dictionary.h"
#include "timing/time.h"
#include "math/float2.h"
#include "math/line.h"


//------------------------------------------------------------------------------
class btDiscreteDynamicsWorld;
class btCollisionObject;

namespace Physics
{
class Level;
class MouseGripper;

class PhysicsServer : public Core::RefCounted
{
	__DeclareClass(PhysicsServer);
    __DeclareSingleton(PhysicsServer);

public:
	/// shape types
	enum RayTestType
	{
		All = 0,
		Closest,		

		NumRayTestTypes,
	};
    /// constructor
    PhysicsServer();
    /// destructor
    virtual ~PhysicsServer();

    /// initialize the physics subsystem
    virtual bool Open();
    /// close the physics subsystem
    virtual void Close();
    /// Is server open?
    bool IsOpen() const;
    /// perform simulation step(s)
    virtual void Trigger();
    /// set the current physics level object
    void SetLevel(Level* level);
    /// get the current physics level object
    Level* GetLevel() const;
    /// access to global embedded mouse gripper
    MouseGripper* GetMouseGripper() const;
    /// set the current simulation time
    void SetTime(Timing::Time t);
    /// get the current simulation time
    Timing::Time GetTime() const;
    /// set current point of interest
    void SetPointOfInterest(const Math::vector& p);
    /// get current point of interest
    const Math::vector& GetPointOfInterest() const;
    /// find a registered physics entity by its unique id
    PhysicsEntity* GetEntityByUniqueId(PhysicsEntity::Id uniqueId) const;

	//virtual PhysicsEntity* CreateEntity(Shape * shape, const float mass) const;
	
    virtual Shape* CreateBoxShape(const Math::matrix44& m, MaterialType matType, const Math::vector& size, bool isGhost = false) const;
    /// create a sphere shape object
    virtual Shape* CreateSphereShape(const Math::matrix44& m, MaterialType matType, float radius, bool isGhost = false) const;
    /// create a capsule shape object
    virtual Shape* CreateCapsuleShape(const Math::matrix44& m, MaterialType matType, float radius, float length, bool isGhost = false) const;
    /// create a mesh shape object
    //virtual Shape* CreateMeshShape(const Math::matrix44& m, MaterialType matType, const Util::String& meshFilename, int meshGroupIndex) const;

	//virtual Shape* CreateMeshShapeF(const Math::matrix44& m, MaterialType matType, const Util::String& meshFilename, int meshGroupIndex) const;
    /// create a ray object
    //virtual Ray* CreateRay(const Math::vector& orig, const Math::vector& vec) const;
    /// Do a ray check starting from position `pos' along ray `dir'.
    Util::Array<Ptr<ContactPoint> >RayCheck(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, RayTestType rayType);    
	/// do a stabbing test into the world with a ray bundle, return distance to intersection
	bool RayBundleCheck(const Math::vector& from, const Math::vector& to, const Math::vector& upVec, const Math::vector& leftVec, float bundleRadius, const FilterSet& excludeSet, float& outContactDist);    
    /// do a ray check through the mouse pointer and return closest contact
    Ptr<ContactPoint> GetClosestContactUnderMouse(const Math::line& worldMouseRay, const FilterSet& excludeSet);
    /// get closest contact along ray
    Ptr<ContactPoint> GetClosestContactAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet);
    /// apply an impulse along a ray into the world onto the first object which the ray hits
    bool ApplyImpulseAlongRay(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, float impulse);
    /// apply an area impule to the world
//    void ApplyAreaImpulse(AreaImpulse* impulse);
    /// return all entities within a spherical area
    int GetEntitiesInSphere(const Math::vector& pos, float radius, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result);
    /// return all entities within a box 
    int GetEntitiesInBox(const Math::vector& scale, const Math::matrix44& m, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result);
    /// render a debug visualization of the level
    virtual void RenderDebug();

	void AddFilterSet(const Util::String & name, FilterSet*);
	FilterSet * GetFilterSet(const Util::String &name);
    /// get a unique stamp value
    static uint GetUniqueStamp();
	/// get current bullet world
	btDiscreteDynamicsWorld * GetBulletWorld() const;

protected:
    /// register an entity with the server
    void RegisterEntity(PhysicsEntity* entity);
    /// unregister an entity from the server
    void UnregisterEntity(PhysicsEntity* entity);

	void GetEntitiesInShape(btCollisionObject * shape, const FilterSet& excludeSet, Util::Array<Ptr<PhysicsEntity> >& result );
    friend class Level;
    friend class RigidBody;
    friend class TerrainEntity;
    friend class Ray;
    friend class MouseGripper;
    friend class PhysicsEntity;

    static uint UniqueStamp;  
    bool isOpen;
    Timing::Time time;	
    Util::Array<ContactPoint> contactPoints;
    Ptr<Level> curLevel;
    //Ptr<MeshCache> meshCache;
    Util::Dictionary<uint, PhysicsEntity*> entityRegistry;
	Util::Dictionary<Util::String, FilterSet*> filterSets;
};

//------------------------------------------------------------------------------
/**
*/
inline
uint
PhysicsServer::GetUniqueStamp()
{
    return ++UniqueStamp;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
PhysicsServer::IsOpen() const
{
    return isOpen;
}

//------------------------------------------------------------------------------
/**
    Set the current time. Should be called before each call to Trigger().

    @param  t   the current time in seconds
*/
inline
void
PhysicsServer::SetTime(Timing::Time t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
    Get the current time.

    @return     current time
*/
inline
Timing::Time
PhysicsServer::GetTime() const
{
	return this->time;	
}

//------------------------------------------------------------------------------
/**
    Find a physics entity by its unique id. Can return 0!

    @param  uniqueId    unique id of entity to find
    @return             pointer to entity, or 0 if entity doesn't exist
*/
inline
PhysicsEntity*
PhysicsServer::GetEntityByUniqueId(PhysicsEntity::Id uniqueId) const
{
    PhysicsEntity* entity = 0;
    if (0 != uniqueId && this->entityRegistry.Contains(uniqueId))
    {
        entity = this->entityRegistry[uniqueId];
    }
    return entity;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif

