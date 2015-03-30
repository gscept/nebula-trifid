//------------------------------------------------------------------------------
//  physics/sphereshape.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/sphereshape.h"
#include "physics/materialtable.h"
#include "debugrender/debugshaperenderer.h"

namespace Physics
{
__ImplementClass(Physics::SphereShape, 'PSPS', Physics::Shape);

using namespace Debug;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
SphereShape::SphereShape() :
    Shape(Sphere),
    radius(1.0f)
{
}

//------------------------------------------------------------------------------
/**
*/
SphereShape::~SphereShape()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a sphere object, add it to ODE's collision space, and initialize
    the mass member.
*/
bool
SphereShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {
        dGeomID sphere = dCreateSphere(0, radius);
        this->AttachGeom(sphere, spaceId);
        dMassSetSphere(&(this->odeMass), Physics::MaterialTable::GetDensity(this->materialType), this->radius);
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
SphereShape::RenderDebug(const Math::matrix44& parentTransform)
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
        Math::matrix44 m = Math::matrix44::identity();
        m.scale(Math::float4(this->radius, this->radius, this->radius, 0));
        m = matrix44::multiply(m, this->GetTransform());
        m = matrix44::multiply(m, parentTransform);
        DebugShapeRenderer::Instance()->DrawSphere(m, this->GetDebugVisualizationColor());
    }
}

} // namespace Physics