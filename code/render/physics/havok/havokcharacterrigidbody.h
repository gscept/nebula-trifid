#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokCharacterPhantom
    
    Uses a rigidbody to represent the character, this is faster but gives less
	control.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file

*/
#include "havokcharacterbase.h"
#include <Physics2012/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>
//------------------------------------------------------------------------------

namespace Havok
{
class HavokCharacterRigidBody : public HavokCharacterBase
{
	__DeclareClass(HavokCharacterRigidBody);
public:
	/// constructor
	HavokCharacterRigidBody();
	/// destructor
	virtual ~HavokCharacterRigidBody();

	/// called before simulation step is taken
	virtual void OnStepBefore();

	/// set transform
	void SetTransform(const Math::matrix44& trans);
	/// get transform
	const Math::matrix44& GetTransform();

	/// get the character-rigidbody
	hkRefPtr<hkpRigidBody> GetRigidBody();
	/// get the character-rigidbody as a world object
	hkRefPtr<hkpWorldObject> GetWorldObject();

	/// if enabling collision callback then this creates and attaches a contact listener to this object
	void SetEnableCollisionCallback(bool enable);
	/// returns linear velocity
	Math::vector GetLinearVelocity();

	/// set collide category
	virtual void SetCollideCategory(Physics::CollideCategory coll);
	/// will deliberately throw an error! You must instead update the filter in the havok scene
	virtual void SetCollideFilter(uint mask);

	/// handle any changes concerning crouching
	virtual void HandleCrouching();

protected:
	/// attach to scene
	virtual void Attach(Physics::BaseScene * world);
	/// detach from scene
	virtual void Detach();

	/// summarize the input
	void GetInput(hkpCharacterInput& outInput);

	/// set up default character states
	void SetupCharacterStates();

	hkpCharacterRigidBodyCinfo charInfo;
	hkRefPtr<hkpCharacterRigidBody> characterRigidBody;
};

} 
// namespace Havok
//------------------------------------------------------------------------------