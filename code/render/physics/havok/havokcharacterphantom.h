#pragma once
//------------------------------------------------------------------------------
/**
    @class Havok::HavokCharacterPhantom
    
    Uses a phantom to represent the character, this is slower but gives more 
	control.
    
    (C) 2013-2014 Individual contributors, see AUTHORS file

*/
#include "havokcharacterbase.h"
//------------------------------------------------------------------------------

class hkpShapePhantom;
class hkpCharacterProxy;

namespace Havok
{
class HavokCharacterPhantom : public HavokCharacterBase
{
	__DeclareClass(HavokCharacterPhantom);
public:
	/// constructor
	HavokCharacterPhantom();
	/// destructor
	virtual ~HavokCharacterPhantom();

	/// called before simulation step is taken
	void OnStepBefore();

	/// set transform
	void SetTransform(const Math::matrix44& trans);
	/// get transform
	const Math::matrix44& GetTransform();

	/// get the phantom as a world object
	hkRefPtr<hkpWorldObject> GetWorldObject();
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

	hkpCharacterProxyCinfo charInfo;
	hkRefPtr<hkpShapePhantom> phantom;
	hkRefPtr<hkpCharacterProxy> characterProxy;		//< holds the phantom
}; 
} 
// namespace Havok
//------------------------------------------------------------------------------