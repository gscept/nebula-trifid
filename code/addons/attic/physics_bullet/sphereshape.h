#ifndef PHYSICS_SPHERESHAPE_H
#define PHYSICS_SPHERESHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::SphereShape

    A sphere shape in the Physics subsystem.
    
    (C) 2003 RadonLabs GmbH
*/
#include "physics/shape.h"

//------------------------------------------------------------------------------
namespace Physics
{
class SphereShape : public Shape
{
	__DeclareClass(SphereShape);
public:
    /// constructor
    SphereShape();
    /// destructor
    virtual ~SphereShape();
    /// render debug visualization
    virtual void RenderDebug(const Math::matrix44& t);
    /// attach the shape to the world
    virtual bool Attach(dSpaceID spaceId);
    /// set radius of the sphere
    void SetRadius(float r);
    /// get radius of the sphere
    float GetRadius() const;

private:
    float radius;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
SphereShape::SetRadius(float r)
{
    n_assert(!this->IsAttached());
    this->radius = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
SphereShape::GetRadius() const
{
    return this->radius;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif
