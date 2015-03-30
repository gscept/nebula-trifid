#ifndef Physics_LEVEL_H
#define Physics_LEVEL_H
//------------------------------------------------------------------------------
/**
    @class Physics::Level

    The Physics level contains all the physics entities.

    Has a "point of interest" property which should be set to the point
    where the action happens (for instance where the player controlled
    character is at the moment). This is useful for huge levels where
    physics should only happen in an area around the player.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
#include "util/array.h"
#include "util/hashtable.h"
#include "timing/time.h"
//#include "kernel/nprofiler.h"
#include "util/blob.h"
#include "math/vector.h"

class btDiscreteDynamicsWorld;
class btCollisionObject;
class btDefaultCollisionConfiguration;
class btBroadphaseInterface;
class btConstraintSolver;
class btCollisionDispatcher;
class DebugDrawer;
//------------------------------------------------------------------------------
namespace Physics
{
class PhysicsEntity;
class Shape;
class Ray;

class Level : public Core::RefCounted 
{
    __DeclareClass(Level);
public:
    /// constructor
    Level();
    /// destructor
    virtual ~Level();
    /// called when the level is attached to the physics server
    virtual void OnActivate();
    /// called when the level is detached from the physics server
    virtual void OnDeactivate();
    /// perform one (or more) simulation steps depending on current time
    virtual void Trigger();
    /// attach a physics entity to the level
    void AttachEntity(PhysicsEntity* entity);
    /// remove a physics entity from the level
    void RemoveEntity(PhysicsEntity* entity);
    /// get number of entities
    int GetNumEntities() const;
    /// get pointer to entity at index
    PhysicsEntity* GetEntityAt(int index) const;
    /// directly attach a shape to the level
    void AttachShape(Shape* shape);
    /// remove a shape from the level
    void RemoveShape(Shape* shape);
    /// get number of shapes attached to level
    int GetNumShapes() const;
    /// get shape at index
    Shape* GetShapeAt(int index) const;
    /// set the current simulation time
    void SetTime(Timing::Time t);
    /// get the current simulation time
    Timing::Time GetTime() const;
    /// set step size
    void SetStepSize(Timing::Time t);
    /// get step size
    Timing::Time GetStepSize() const;
    /// get current step count
    int GetStepCount() const;
    /// set point of interest
    void SetPointOfInterest(const Math::vector& v);
    /// get current point of interest
    const Math::vector& GetPointOfInterest() const;
    /// render debug visualization
    void RenderDebug();
    /// get the ODE world id
	btDiscreteDynamicsWorld * GetBulletWorld() const;
        /// set gravity vector
    void SetGravity(const Math::vector& v);
    /// get gravity vector
    const Math::vector& GetGravity() const;

	///for level editor
	void SetDisablePhysics(bool i_bDisable){m_bDisablePhysics = i_bDisable;}
	bool GetDisabledPhysics(){return m_bDisablePhysics;}

protected:
   
	//for level editor
	bool m_bDisablePhysics;
    Timing::Time time;
    Timing::Time stepSize;
    Util::Array<Ptr<PhysicsEntity> > entityArray;
    Util::Array<Ptr<Shape> > shapeArray;
    Ptr<Shape> collideShape;
    Math::vector pointOfInterest;
    Math::vector gravity;

	struct _physics
	{
		_physics() : collisionConfiguration(NULL), broadphase(NULL),
			constraintSolver(NULL), 
			dispatcher(NULL), dynamicsWorld(NULL)            
		{}
		btDefaultCollisionConfiguration *collisionConfiguration;
		btBroadphaseInterface *broadphase;
		btConstraintSolver *constraintSolver;
		btCollisionDispatcher* dispatcher;
		btDiscreteDynamicsWorld *dynamicsWorld;
	} physics;

    enum
    {
        MaxContacts = 16,
    };
    Timing::Time simTimeStamp;
    int simSteps;
    
    Util::HashTable<Util::Blob, Timing::Time> collisionSounds;

/*    PROFILER_DECLARE(profFrameBefore);
    PROFILER_DECLARE(profFrameAfter);
    PROFILER_DECLARE(profStepBefore);
    PROFILER_DECLARE(profStepAfter);
    PROFILER_DECLARE(profCollide);
    PROFILER_DECLARE(profStep);
    PROFILER_DECLARE(profJointGroupEmpty);*/

    #ifdef __NEBULA_STATS__
    int statsNumSpaceCollideCalled;              // number of times dSpaceCollide has been invoked
    int statsNumNearCallbackCalled;              // number of times the near callback has been invoked
    int statsNumCollideCalled;                   // number of times the collide function has been invoked
    int statsNumCollided;                        // number of times two shapes have collided
    int statsNumSpaces;
    int statsNumShapes;
    int statsNumSteps;
    #endif
	DebugDrawer *debugDrawer;
};

//------------------------------------------------------------------------------
/**
    Get the current gravity.

    @return     current gravity
*/
inline
const Math::vector&
Level::GetGravity() const
{
    return this->gravity;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
Level::SetPointOfInterest(const Math::vector& v)
{
    this->pointOfInterest = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
Level::GetPointOfInterest() const
{
    return this->pointOfInterest;
}

//------------------------------------------------------------------------------
/**
    Get number of entities attached to level.
*/
inline
int
Level::GetNumEntities() const
{
    return this->entityArray.Size();
}

//------------------------------------------------------------------------------
/**
    Sets the current simulation time stamp.

    @param  t   the current simulation time stamp
*/
inline
void
Level::SetTime(Timing::Time t)
{
    this->time = t;
}

//------------------------------------------------------------------------------
/**
    Returns current simlulation time stamp.

    @return     the current simulation time stamp
*/
inline
Timing::Time
Level::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
    Set the simulation step size.

    @param  s   the step size in seconds
*/
inline
void
Level::SetStepSize(Timing::Time t)
{
    this->stepSize = t;
}

//------------------------------------------------------------------------------
/**
    Get the simulation step size.

    @return     step size in seconds
*/
inline
Timing::Time
Level::GetStepSize() const
{
    return this->stepSize;
}

//------------------------------------------------------------------------------
/**
    Get the current simulation step count.

    @return     current step number
*/
inline
int
Level::GetStepCount() const
{
    return this->simSteps;
}

inline
btDiscreteDynamicsWorld * Level::GetBulletWorld() const
{
	return this->physics.dynamicsWorld;
}

}; // namespace Physics

//------------------------------------------------------------------------------
#endif
    
    