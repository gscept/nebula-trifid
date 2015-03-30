#ifndef PROPERTIES_PHYSICSPROPERTY_H
#define PROPERTIES_PHYSICSPROPERTY_H
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
*/
#include "properties/transformableproperty.h"
#include "physics/physicsentity.h"
#include "physicsattr/physicsattributes.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class PhysicsProperty : public BaseGameFeature::TransformableProperty
{
	__DeclareClass(PhysicsProperty);
public:
    /// constructor
    PhysicsProperty();
    /// destructor
    virtual ~PhysicsProperty();

    /// setup callbacks for this property
    virtual void SetupCallbacks();
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

    /// called after movement has happened
    virtual void OnMoveAfter();
    /// get a pointer to the physics entity
    virtual Ptr<Physics::PhysicsEntity> GetPhysicsEntity() const;

    /// enable/disable physics
    void SetEnabled(bool enabled);
    /// is physics enabled
    bool IsEnabled() const;

protected:
    /// enable and activate the physics, overload in subclass
    virtual void EnablePhysics();
    /// disable and cleanup the physics, overload in subclass
    virtual void DisablePhysics();

    /// apply a global impulse vector at the next time step at a global position
    void ApplyImpulseAtPos(const Math::vector& impulse, const Math::vector& pos, bool multByMass = false);

    bool enabled;

    Ptr<Physics::PhysicsEntity> physicsEntity;
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
#endif