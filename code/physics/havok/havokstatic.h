#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokStatic
	
	Sets up a static compound shape, this is intended to be used for larger
	static shapes such as landscapes as buildings.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

#include "physics/base/basestatic.h"
#include "physics/model/templates.h"
#include "havokbody.h"

class hkpWorld;

namespace Physics
{
	class PhysicsCommon;
}

namespace Havok
{

class HavokStatic : public Physics::BaseStatic
{
	__DeclareClass(HavokStatic);
public:
	/// default constructor
	HavokStatic();
	/// destructor
	~HavokStatic();

	/// render the debug shapes
	virtual void RenderDebug();

	/// if enabling collision callback then this creates and attaches a contact listener to this object
	void SetEnableCollisionCallback(bool enable);

	/// set collide category
	virtual void SetCollideCategory(Physics::CollideCategory coll);
	/// will deliberately throw an error! You must instead update the filter in the havok scene
	virtual void SetCollideFilter(uint mask);

	// ----- Havok-specific methods ----- 

	/// get the rigidbody pointer
	hkRefPtr<hkpRigidBody> GetRigidBody();

protected:
	friend class Physics::PhysicsObject;

	/// attach to scene
	void Attach(Physics::BaseScene * world);
	/// detach from scene
	void Detach();

	/// setup static physics object from a template
	void SetupFromTemplate(const Physics::PhysicsCommon& templ);

	hkRefPtr<hkpRigidBody> rigidBody;
	hkRefPtr<hkpWorld> world;
};

//------------------------------------------------------------------------------
/**
*/
inline hkRefPtr<hkpRigidBody> 
HavokStatic::GetRigidBody()
{
	return this->rigidBody;
}

}