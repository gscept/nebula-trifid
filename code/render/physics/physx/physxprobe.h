#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysX::PhysXProbe
    
    (C) 2016 Individual contributors, see AUTHORS file
*/

#include "physics/base/baseprobe.h"
#include "physics/collider.h"
#include "math/matrix44.h"

namespace physx
{ 
	class PxActor;
}

namespace PhysX
{

class PhysXProbe : public Physics::BaseProbe
{
	__DeclareClass(PhysXProbe);

public:
    ///
    PhysXProbe();
    ///
    ~PhysXProbe();

    ///
    virtual Util::Array<Ptr<Core::RefCounted>> GetOverlappingObjects();
	/// convenience function
	void Init(const Ptr<Physics::Collider> & coll, const Math::matrix44 & trans);

	///
	void ClearOverlap();
	///
	void AddOverlap(physx::PxActor * other);
protected:	
    ///
    void Attach(Physics::BaseScene * world);
    ///
    void Detach();
	Util::Array<Ptr<Core::RefCounted>> overlap;
};
}