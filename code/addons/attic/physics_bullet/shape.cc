//------------------------------------------------------------------------------
//  Physics/shape.cc
//  (C) Johannes Hirche, LTU Skelleftea
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "Physics/rigidbody.h"
#include "Physics/shape.h"
#include "Physics/conversion.h"
namespace Physics
{
__ImplementClass(Physics::Shape, 'SHA2', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Shape::Shape() :
    type(InvalidShapeType),
    shape(0),
	collideCategory(Default)
{

}

//------------------------------------------------------------------------------
/**
*/
Shape::~Shape()
{
	if(shape)
	{
		btCollisionShape * internalShape = shape->getCollisionShape();
		if(internalShape)
			n_delete(internalShape);
		n_delete(shape);
		shape = 0;
	}
}

void
Shape::SetInternalObject(Type t, btCollisionObject * inShape)
{
	n_assert(type == InvalidShapeType);
	shape = inShape;
	type = t;
}

void
Shape::SetTransform(const Math::matrix44& m)
{
    this->transform = m;

    if (this->shape)
    {
		this->shape->setWorldTransform(Neb2BtM44Transform(m));		
    }
}

//------------------------------------------------------------------------------
/**
    Get the body's current transformation in global space.

    @param  m   [out] will be filled with the resulting matrix
*/
const Math::matrix44&
Shape::GetTransform() const
{
    return this->transform;
}

} // namespace Physics