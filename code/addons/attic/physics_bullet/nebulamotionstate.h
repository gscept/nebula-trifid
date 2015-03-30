#pragma once
//------------------------------------------------------------------------------
/**
    @class NebulaMotionState

    (C) 2011 Johannes Hirche
*/

#include "LinearMath/btMotionState.h"
#include "Physics/physicsserver.h"

namespace Physics
{

class NebulaMotionState : public btMotionState
{
public:	
	NebulaMotionState(RigidBody&b): body(b) {}

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
		//if(body.isvalid())
		body.UpdateTransform(Math::matrix44::multiply(this->invInitialTransform,transform));
	}
    const Math::matrix44& GetTransform() const;	
	void SetTransform(const Math::matrix44& m )
	{
		this->transform = Math::matrix44::multiply(this->initialTransform,m);
	}
	void SetInitialTransform(const Math::matrix44 & m)
	{
		this->initialTransform = m;
		this->invInitialTransform = Math::matrix44::inverse(this->initialTransform);
	}
		
private:
    Math::matrix44  transform;	
	Math::matrix44	initialTransform;
	Math::matrix44	invInitialTransform;
	RigidBody& body;
};

//------------------------------------------------------------------------------
/**
*/
inline 
const Math::matrix44& 
NebulaMotionState::GetTransform() const
{
    return this->transform;
}

};