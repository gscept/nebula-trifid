//------------------------------------------------------------------------------
//  physics/boxshape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/boxshape.h"
#include "debugrender/debugshaperenderer.h"
#include "physics/rigidbody.h"

namespace Physics
{
__ImplementClass(Physics::BoxShape, 'PSHA',Physics::Shape);

using namespace Math;
using namespace Debug;

//------------------------------------------------------------------------------
/**
*/
BoxShape::BoxShape() :
    Shape(Box),
    size(1.0f, 1.0f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BoxShape::~BoxShape()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a box object, add it to ODE's collision space, and initialize
    the mass member.
*/
bool
BoxShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {
        dGeomID box = dCreateBox(0, this->size.x(), this->size.y(), this->size.z());
        this->AttachGeom(box, spaceId);
        dMassSetBox(&(this->odeMass), Physics::MaterialTable::GetDensity(this->materialType), 
                    this->size.x(), this->size.y(), this->size.z());
        this->TransformMass();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the sphere shape.

    @param  t   transform matrix of my parent rigid body
*/
void
BoxShape::RenderDebug(const Math::matrix44& parentTransform)
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
        Math::matrix44 m = Math::matrix44::identity();
        m.scale(this->size * 0.5f);
        m = matrix44::multiply(m, this->GetTransform());
        m = matrix44::multiply(m, parentTransform);
        DebugShapeRenderer::Instance()->DrawBox(m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics