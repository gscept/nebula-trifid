#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::AnimatedPhysicsProperty

    A property that supports kinematic physics objects that are connected to
	animated joints from a graphicsproperty. It does not have rigid body
	mechanics (like mass), but only moves colliders around.
  
    (C) 2012 Johannes Hirche
*/
#include "properties/transformableproperty.h"
#include "physics/physicsbody.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "graphics/modelentity.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
	class AnimatedPhysicsProperty : public BaseGameFeature::TransformableProperty
{
	__DeclareClass(AnimatedPhysicsProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    AnimatedPhysicsProperty();
    /// destructor
    virtual ~AnimatedPhysicsProperty();

    /// setup callbacks for this property
    virtual void SetupCallbacks();    
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();

	/// called before rendering happens
	virtual void OnRender();

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

	/// update the root node transform
	virtual void UpdateTransform(const Math::matrix44 & transform){}

	/// connect joints and bodies
	virtual void AttachToJoints(const Util::Array<Util::String>& joints, const Util::Array<Util::String>& bodies);

	// connect joints and bodies that have the same name to each other
	virtual void AttachByNames();	

	bool enabled;	

	Util::Array<Ptr<Physics::PhysicsBody>> physicsEntities;
	Util::Array<Util::StringAtom> trackedJoints;
	Util::Dictionary<Util::StringAtom, Ptr<Physics::PhysicsBody>> bodyHash;
	Util::Dictionary<Util::StringAtom, Ptr<Physics::PhysicsBody>> jointHash;
	Util::Dictionary<Util::StringAtom, Math::matrix44> jointOffsets;
	Ptr<Graphics::ModelEntity> trackedModel;
	bool linked;

};

__RegisterClass(AnimatedPhysicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimatedPhysicsProperty::IsEnabled() const
{
	return this->enabled;    
}
}