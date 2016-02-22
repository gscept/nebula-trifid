#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXStatic

    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "physics/base/basestatic.h"
#include "physics/collider.h"
#include "math/matrix44.h"

namespace physx
{
    class PxRigidStatic;
    class PxScene;
}

namespace PhysX
{


class PhysXStatic : public Physics::BaseStatic
{
__DeclareClass(PhysXStatic);

public:
    ///
    PhysXStatic();
    ///
    ~PhysXStatic();
       
	
protected:	
	friend class PhysicsObject;
    ///
	virtual void SetupFromTemplate(const Physics::PhysicsCommon & templ);
    ///
    void Attach(Physics::BaseScene * world);
    ///
    void Detach();

    physx::PxRigidStatic * body;
    physx::PxScene *scene;
};
}