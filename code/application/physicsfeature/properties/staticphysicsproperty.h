#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::StaticPhysicsProperty

    (C) 2016 Individual contributors, see AUTHORS file
*/
#include "basegamefeature/properties/transformableproperty.h"
#include "physics/physicsbody.h"
#include "physicsfeature/physicsattr/physicsattributes.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class StaticPhysicsProperty : public BaseGameFeature::TransformableProperty
{
	__DeclareClass(StaticPhysicsProperty);
	__SetupExternalAttributes();
public:
    /// constructor
	StaticPhysicsProperty();
    /// destructor
    virtual ~StaticPhysicsProperty();
    
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

	/// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    
    /// enable/disable physics will create/destroy the underlying physics object
    void SetEnabled(bool enabled);
    /// is physics enabled
    bool IsEnabled() const;

protected:
    /// enable and activate the physics, overload in subclass
    virtual void EnablePhysics();
    /// disable and cleanup the physics, overload in subclass
    virtual void DisablePhysics();    

    Ptr<Physics::StaticObject> physicsEntity;
};
__RegisterClass(StaticPhysicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
bool
StaticPhysicsProperty::IsEnabled() const
{
	return this->physicsEntity.isvalid();
}

}; // namespace Properties
//------------------------------------------------------------------------------