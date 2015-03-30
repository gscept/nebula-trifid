#pragma once
//------------------------------------------------------------------------------
/**
    @class NebulaMotionState

    (C) 2012 Johannes Hirche
*/

#include "LinearMath/btMotionState.h"
#include "physics/physicsserver.h"

namespace Bullet
{

class NebulaMotionState : public btMotionState
{
public:	
	NebulaMotionState(BulletBody*b): body(b) {}

	~NebulaMotionState()
	{		
	}
	virtual void getWorldTransform(btTransform& worldTrans) const
	{
		worldTrans = Neb2BtM44Transform(this->transform);
	}
	
	virtual void setWorldTransform(const btTransform& worldTransform)
	{
        this->transform = Bt2NebTransform(worldTransform);		
		body->UpdateTransform( this->transform);
	}
    		
private:
    Math::matrix44  transform;		
	BulletBody* body;
};

};
