//------------------------------------------------------------------------------
//  physics/ProbeSphere.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/probesphere.h"
#include "physics/materialtable.h"
#include "physics/physicsentity.h"
#include "physics/contactpoint.h"
#include "physics/filterset.h"
#include "physics/physicsserver.h"
#include "debugrender/debugshaperenderer.h"

namespace Physics
{
__ImplementClass(Physics::ProbeSphere, 'PPSP', Physics::SphereShape);

using namespace Math;
using namespace Debug;

//------------------------------------------------------------------------------
/**
*/
ProbeSphere::ProbeSphere() :
	staticCollision(false),
	dynamicCollision(false)
{
}

//------------------------------------------------------------------------------
/**
*/
ProbeSphere::~ProbeSphere()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ProbeSphere::Collide()
{
	this->contactPoints.Clear();
	this->collidedEntityIDs.Clear();
	this->dynamicCollision = false;
	this->staticCollision = false;
    this->staticDepth = 0;
    this->staticCollideDir.set(0,0,0);

	// ignore other probes
	FilterSet excludeSet;
    MaterialType probeMaterial = Physics::MaterialTable::StringToMaterialType("Probe");
    MaterialType triggerMaterial = Physics::MaterialTable::StringToMaterialType("Trigger");
	excludeSet.AddMaterialType(probeMaterial);
    excludeSet.AddMaterialType(triggerMaterial);

    // ignore certain physics entity
	if (this->toIgnore)
	{
	    excludeSet.AddEntityId(this->toIgnore->GetUniqueId());
	}
 
	Shape::Collide(excludeSet, this->contactPoints);

	// store entities
	IndexT i;
	for (i = 0; i < this->contactPoints.Size(); i++)
	{
		if (0 != this->contactPoints[i].GetPhysicsEntity())
		{
			this->dynamicCollision = true;
			
			Physics::PhysicsEntity* contactEntity = this->contactPoints[i].GetPhysicsEntity();

			if (contactEntity)
			{
				this->collidedEntityIDs.Append(contactEntity->GetUserData());				
			}
		}
		else
		{
            if (this->contactPoints[i].GetDepth() > this->staticDepth)
            {
                this->staticDepth = this->contactPoints[i].GetDepth();
                this->staticCollideDir = this->contactPoints[i].GetPosition() - this->GetTransform().get_position();
                this->staticCollideDir = vector::normalize(this->staticCollideDir);
            }
			this->staticCollision = true;
		}
	}
}

//------------------------------------------------------------------------------
/**
    Overwrite this in a subclass
*/
bool
ProbeSphere::OnCollide(Shape* collidee)
{
	// don't collide with other probes
    if (collidee->GetMaterialType() == Physics::MaterialTable::StringToMaterialType("Probe"))
    {
        return false;
    }
	
	if (collidee->GetMaterialType() == Physics::MaterialTable::StringToMaterialType("Character"))
	{
		return false;
	}

    SphereShape::OnCollide(collidee);

    return false;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the sphere shape.

    @param  t   transform matrix of my parent rigid body
*/
void
ProbeSphere::RenderDebug(const Math::matrix44& parentTransform)
{
    if (this->IsAttached())
    {
        // compute resulting model matrix
		float r = this->GetRadius();
        Math::matrix44 m = Math::matrix44::identity();
		m.scale(Math::vector(r, r, r));
        m = matrix44::multiply(m, this->GetTransform());
        m = matrix44::multiply(m, parentTransform);
        Math::float4 color;
		if (!this->contactPoints.IsEmpty())
		{
			color = Math::float4(1.0f, 1.0f, 0.0f, 0.1f);
		}
		else
		{
			color = Math::float4(1.0f, 1.0f, 1.0f, 0.1f);
		}
        DebugShapeRenderer::Instance()->DrawSphere(m, color);
    }
}

//------------------------------------------------------------------------------
/**
*/
float
ProbeSphere::DistanceToGround(const Math::vector& pos, float upOffset, float downOffset)
{
    Math::vector from(pos.x(), pos.y() + upOffset, pos.z());
    Math::vector dir(0.0f, -(upOffset + downOffset), 0.0f);

    FilterSet excludeSet;
    MaterialType probeMaterial = Physics::MaterialTable::StringToMaterialType("Probe");
	excludeSet.AddMaterialType(probeMaterial);
    Physics::PhysicsServer::Instance()->RayCheck(from, dir, excludeSet);   
    Util::Array<Physics::ContactPoint> contactPoints = Physics::PhysicsServer::Instance()->GetContactPoints();
    IndexT i;
	for (i = 0; i < contactPoints.Size(); i++)
	{
        if (!contactPoints[i].GetPhysicsEntity())
        {
            return Math::vector(contactPoints[i].GetPosition() - from).length() - upOffset;
        }
    }

    return downOffset * 1000;;
}

} // namespace Physics