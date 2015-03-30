#pragma once

#include "physics/base/physicsserverbase.h"

namespace Bullet
{

class BulletPhysicsServer : public Physics::BasePhysicsServer
{
	__DeclareClass(BulletPhysicsServer);
public:
	BulletPhysicsServer(){}
	~BulletPhysicsServer(){}

	void HandleCollisions();
protected:	
	
private:
};

}