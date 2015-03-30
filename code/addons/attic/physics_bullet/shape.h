#ifndef PHYSICS_SHAPE_H
#define PHYSICS_SHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::Shape
    
    
    (C) 2012 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "core/ptr.h"
//#include "physics/materialtable.h"
#include "physics/collidecategory.h"
#include "Physics/contactpoint.h"

//------------------------------------------------------------------------------
class btCollisionObject;
class btCollisionShape;

namespace Physics
{
class RigidBody;
class PhysicsEntity;
class FilterSet;

class Shape : public Core::RefCounted
{
    __DeclareClass(Shape);
public:
    /// shape types
    enum Type
    {
        InvalidShapeType = 0,
        Box,
        Sphere,
        Plane,
        Mesh,
        Capsule,
		Environment,
		Unknown,

        NumShapeTypes,
    };

    /// default constructor
    Shape();
    /// destructor
    virtual ~Shape();
    /// render debug visualization
	virtual void RenderDebug(const Math::matrix44& t){}

    /// get the shape type
    Type GetType() const;       
	

	btCollisionObject * GetShape();
	/// set the object's world space transform
	void SetTransform(const Math::matrix44& m);
	/// constructor    
	void SetInternalObject(Type t, btCollisionObject* shape);
	/// get the object's world space transform
	const Math::matrix44& GetTransform() const;

	void SetCollideCategory(CollideCategory coll);
	CollideCategory GetCollideCategory() const;
protected:
    friend class RigidBody;
    friend class Level;
    friend class Ray;
	friend class PhysicsServer;

    Type type;
    btCollisionObject * shape;	  
	Math::matrix44 transform;
	CollideCategory collideCategory;
};

inline
btCollisionObject*
Shape::GetShape()
{
	return shape;
}
inline
CollideCategory 
Shape::GetCollideCategory() const
{
	return this->collideCategory;
}

inline
void
Shape::SetCollideCategory(CollideCategory coll)
{
	this->collideCategory = coll;
}
}; // namespace Physics
//------------------------------------------------------------------------------
#endif
    