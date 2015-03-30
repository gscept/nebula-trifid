//------------------------------------------------------------------------------
//  physics/capsuleshape.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/capsuleshape.h"
#include "debugrender/debugshaperenderer.h"
#include "math/vector.h"

namespace Physics
{
__ImplementClass(Physics::CapsuleShape, 'PCSH', Physics::Shape);

using namespace Math;
using namespace Debug;

//------------------------------------------------------------------------------
/**
*/
CapsuleShape::CapsuleShape() :
    Shape(Capsule),
    radius(1.0f),
    length(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CapsuleShape::~CapsuleShape()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create a capsule object, add it to ODE's collide space, and initialize
    the mass member.
*/
bool
CapsuleShape::Attach(dSpaceID spaceId)
{
    if (Shape::Attach(spaceId))
    {
        dGeomID capsule = dCreateCCylinder(0, this->radius, this->length);
        this->AttachGeom(capsule, spaceId);
        dMassSetCappedCylinder(&(this->odeMass), Physics::MaterialTable::GetDensity(this->materialType),
                               3, this->radius, this->length);
        this->TransformMass();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the capsule shape.
*/
void
CapsuleShape::RenderDebug(const Math::matrix44& parentTransform)
{
    if (this->IsAttached())
    {
        DebugShapeRenderer* shapeRenderer = DebugShapeRenderer::Instance();
        Math::vector capScale(this->radius, this->radius, this->radius);
        Math::float4 color = this->GetDebugVisualizationColor();

        Math::matrix44 cap0Transform = Math::matrix44::identity();
        cap0Transform.scale(capScale);
        cap0Transform.translate(Math::vector(0.0f, 0.0f, this->length * 0.5f));
        cap0Transform = matrix44::multiply(cap0Transform , this->GetTransform());
        cap0Transform = matrix44::multiply(cap0Transform , parentTransform);
        shapeRenderer->DrawSphere(cap0Transform, color);

        Math::matrix44 cap1Transform = Math::matrix44::identity();
        cap1Transform.scale(capScale);
        cap1Transform.translate(Math::vector(0.0f, 0.0f, -this->length * 0.5f));
        cap1Transform = matrix44::multiply(cap1Transform , this->GetTransform());
        cap1Transform = matrix44::multiply(cap1Transform , parentTransform);
        shapeRenderer->DrawSphere(cap1Transform, color);

        Math::matrix44 cylTransform = Math::matrix44::identity();
        cylTransform.scale(Math::vector(this->radius, this->radius, this->length));
        cylTransform = matrix44::multiply(cylTransform, this->GetTransform());
        cylTransform = matrix44::multiply(cylTransform, parentTransform);
        shapeRenderer->DrawCylinder(cylTransform, color);
    }
}

}; // namespace Physics