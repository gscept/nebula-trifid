#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::PhysicsProperty

    A physics property adds basic physical behaviour to a game entity. 
    The default behaviour is that of a passive physics object which will
    just passively roll and bounce around. Implement more advanced behaviour
    in subclasses.

    The physics property maintains the attributes:

    Attr::Transform
    Attr::Velocity
  
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "basegamefeature/properties/transformableproperty.h"
#include "physics/physicsbody.h"
#include "physicsfeature/physicsattr/physicsattributes.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
	class PhysicsProperty : public BaseGameFeature::TransformableProperty
{
	__DeclareClass(PhysicsProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    PhysicsProperty();
    /// destructor
    virtual ~PhysicsProperty();

    /// setup callbacks for this property
    virtual void SetupCallbacks();    
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

	/// called on begin of frame
	virtual void OnBeginFrame();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

    /// called after movement has happened
    virtual void OnMoveAfter();
    /// get a pointer to the physics entity
    virtual Ptr<Physics::PhysicsBody> GetPhysicsBody() const;

    /// enable/disable physics will create/destroy the underlying physics object
    void SetEnabled(bool enabled);
    /// is physics enabled
    bool IsEnabled() const;

	/// temporarily enable/disable physics handling
	void SetSleeping(bool sleep);

	/// is sleeping
	bool IsSleeping() const;

//	virtual void doCollideEntity(Physics::PhysicsEntity* collidee, Ptr<Physics::Contact> contact);
//	virtual void doCollideOther(Physics::Shape* collidee, Ptr<Physics::Contact> contact);


protected:
    /// enable and activate the physics, overload in subclass
    virtual void EnablePhysics();
    /// disable and cleanup the physics, overload in subclass
    virtual void DisablePhysics();

	/// determine if we are the one in charge for physics
	virtual bool IsSimulationHost();
	/// transfer physics transform to game entity
	virtual void UpdateFromPhysics();

    /// apply a global impulse vector at the next time step at a global position
    void ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multByMass = false);

    bool enabled;	

    Ptr<Physics::PhysicsBody> physicsEntity;
};
__RegisterClass(PhysicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
bool
PhysicsProperty::IsEnabled() const
{
    return this->enabled;    
}

}; // namespace Properties
//------------------------------------------------------------------------------