#ifndef PHYSICS_BOXSHAPE_H
#define PHYSICS_BOXSHAPE_H
//------------------------------------------------------------------------------
/**
    @class Physics::BoxShape

    A box shape in the Physics subsystem.
    
    (C) 2003 RadonLabs GmbH
*/
#include "physics/shape.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Physics
{
class BoxShape : public Shape
{
	__DeclareClass(BoxShape);
public:
    /// constructor
    BoxShape();
    /// destructor
    virtual ~BoxShape();
    /// render debug visualization
    virtual void RenderDebug(const Math::matrix44& t);
    /// attach the shape to the world
    virtual bool Attach(dSpaceID spaceId);
    /// set the size of the box
    void SetSize(const Math::vector& s);
    /// get the size of the box
    const Math::vector& GetSize() const;

private:
    Math::vector size;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
BoxShape::SetSize(const Math::vector& s)
{
    n_assert(!this->IsAttached());
    this->size = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
BoxShape::GetSize() const
{
    return this->size;
}

}; // namespace OdePhysics
//------------------------------------------------------------------------------
#endif    