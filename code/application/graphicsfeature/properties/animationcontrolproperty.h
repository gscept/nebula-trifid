#pragma once
//------------------------------------------------------------------------------
/**
    @class GraphicsFeature::AnimationControlProperty
    
    Graphics property for animated characters.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "game/property.h"
#include "physics/physicsobject.h"

//------------------------------------------------------------------------------
namespace GraphicsFeature
{
class AnimationControlProperty : public Game::Property
{
    __DeclareClass(AnimationControlProperty);
	__SetupExternalAttributes();
public:
    
	/// Constructor
	AnimationControlProperty();
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
	/// called from Entity::ActivateProperties()
	virtual void OnActivate();
	/// called from Entity::DeactivateProperties()
	virtual void OnDeactivate();
private:
    /// start walk
    void Walk(const Ptr<Messaging::Message>& msg);
    /// stop walk
    void Stop();
	/// start idling
	void Idle();
	/// jump
	void Jump();
	/// death
	void Death();
	/// use
	void Use();

    static const IndexT MovementTrackIndex = 0;
	bool walking;
	bool strafing;
	bool death;
	Ptr<Physics::PhysicsObject> physObj;
};
__RegisterClass(AnimationControlProperty);

} // namespace GraphicsFeature
//------------------------------------------------------------------------------
    