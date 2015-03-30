#pragma once
#include "physics/base/basestatic.h"


class btCollisionObject;
class btDynamicsWorld;

namespace Bullet
{
	
class Collider;
class BulletCollider;
class Scene;
	

class BulletStatic : public Physics::BaseStatic
{

	__DeclareClass(BulletStatic);

public:
	/// constructor
	BulletStatic();
	/// destructor
	virtual ~BulletStatic();

	void SetCollideCategory(Physics::CollideCategory coll);
	void SetCollideFilter(uint mask);
	virtual void SetTransform(const Math::matrix44 & trans);

	

protected:
	friend class Scene;

	void Attach(Physics::BaseScene * inWorld);
	void Detach();

	btDynamicsWorld * world;
	btCollisionObject * collObj;

};

}

