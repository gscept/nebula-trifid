#ifndef PHYSICS_CAPSULESHAPE_H
#define PHYSICS_CAPSULESHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::CapsuleShape
    
    A capsule shape (a cylinder with round caps).
    
    (C) 2005 RadonLabs GmbH
*/
#include "physics/shape.h"

//------------------------------------------------------------------------------
namespace Physics
{
class CapsuleShape : public Shape    
{
	__DeclareClass(CapsuleShape);
public:
    /// constructor
    CapsuleShape();
    /// destructor
    virtual ~CapsuleShape();
    /// render debug visualization
    virtual void RenderDebug(const Math::matrix44& t);
    /// attach the shape to the world
    virtual bool Attach(dSpaceID spaceId);
    /// set radius of cylinder
    void SetRadius(float r);
    /// get radius of cylinder
    float GetRadius() const;
    /// set length of cylinder (not counting the caps)
    void SetLength(float l);
    /// get length of cylinder
    float GetLength() const;

private:
    float radius;
    float length;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
CapsuleShape::SetRadius(float r)
{
    this->radius = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CapsuleShape::GetRadius() const
{
    return this->radius;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
CapsuleShape::SetLength(float l)
{
    this->length = l;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
CapsuleShape::GetLength() const
{
    return this->length;
}

}; // namespace Physics
//------------------------------------------------------------------------------
#endif

