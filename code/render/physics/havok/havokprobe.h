#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokProbe
	    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/baseprobe.h"
#include "physics/model/templates.h"

class hkpSimpleShapePhantom;
class hkpWorld;

namespace Physics
{
	class PhysicsCommon;
}

namespace Havok
{
class HavokProbe : public Physics::BaseProbe
{
	__DeclareClass(HavokProbe);
public:
	/// default constructor
	HavokProbe();
	/// destructor
	~HavokProbe();

	/// do debug rendering
	void RenderDebug();
	/// called before simulation step is taken
	void OnStepBefore();

	/// get an array of overlapping objects
	virtual Util::Array<Ptr<Core::RefCounted>> GetOverlappingObjects();

	/// set collide category
	virtual void SetCollideCategory(Physics::CollideCategory coll);
	/// will deliberately throw an error! You must instead update the filter in the havok scene
	virtual void SetCollideFilter(uint mask);

	virtual void SetTransform(const Math::matrix44 & trans);

protected:
	/// attach to scene
	virtual void Attach(Physics::BaseScene * world);
	/// detach from scene
	virtual void Detach();

	bool hasOverlappingObjects;

	hkRefPtr<hkpShape> shape;	//< need to make a separate shape (other than the colliders') that have the correct dimensions, since havok bodies aren't scaled
	Math::float4 halfSize;
	hkRefPtr<hkpSimpleShapePhantom> phantom;
	hkRefPtr<hkpWorld> world;
};

}