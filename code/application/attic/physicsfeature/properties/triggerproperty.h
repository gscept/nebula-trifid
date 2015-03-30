#ifndef PHYSICSFEATURE_TRIGGERPROPERTY_H
#define PHYSICSFEATURE_TRIGGERPROPERTY_H
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::TriggerProperty

    Property for a defined trigger area, a trigger checks if the accepted entity is in its area 
    and executes actions (on this entities, if action requested it)

    (C) 2008 Radon Labs GmbH
*/
#include "properties/transformableproperty.h"
#include "physics/shape.h"
#include "math/vector.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class TriggerProperty : public BaseGameFeature::TransformableProperty
{
	__DeclareClass(TriggerProperty);

public:
    /// constructor
    TriggerProperty();
    /// destructor
    virtual ~TriggerProperty();

    /// setup callbacks for this property
    virtual void SetupCallbacks();
    /// setup default entity attributes
    virtual void SetupDefaultAttributes();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();    
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
    /// called from within Entity::Load() after attributes are loaded
    virtual void OnLoad();
    /// called when all is saved
    virtual void OnSave();
    /// called on begin of frame
    virtual void OnBeginFrame();
    /// setup accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

    /// Render a debug visualization of the trigger if needed
    virtual void OnRenderDebug();
    /// Render a debug visualization of the trigger
    virtual void RenderDebugVisualization();

protected:
	/// this message is called every Attr::TriggerPhase from timestamp
	virtual void TriggerAction();

    /// get postion of the trigger
	Math::point GetTriggerPosition();
    /// get Scaling of the trigger
    Math::vector GetTriggerScale();
    /// get Trigger transform   
    const Math::matrix44 GetTriggerTransform();
    /// filter current entities in Trigger, this will refresh enterdEntities, insideEntities and leftEntities arrays
    void FilterEntities();
    /// will update the currEntitiesInTrigger
    void SetCurrentEntitiesInTrigger();
    /// refresh entitiesLastFrameInTrigger
    void SetEntitiesLastFrameInTrigger();
    /// clear entity arrays
    void ClearEntities();
    /// create static collision
    void CreateCollisionShape();
    /// destroy static collision
    void DestroyCollisionShape();

    /// set trigger status to active or inactive
    void SetActiveStatus(bool status);
    /// check if active flag is set
    bool IsTriggerActive();

    // trigger is currently evaluated
    bool triggerActive;
    // actions will be executed once
    bool isOneWayTrigger;
    // shape of trigger may be a box or a sphere
	Util::String shape;
    // scaling of the trigger
	Math::vector triggerScale;

    // trigger name
	Util::String triggerName;

	// Entities accepted by Filter, currently in trigger;
    // needed to check if an entity was in trigger or has just entered it
    Util::Array<Ptr<Game::Entity> > entitiesLastFrameInTrigger;
    
    // list of entity inside, entered, left, may be empty
    Util::Array<Ptr<Game::Entity> > enterdEntities;
    Util::Array<Ptr<Game::Entity> > insideEntities;
    Util::Array<Ptr<Game::Entity> > leftEntities;

    Util::Array<Ptr<Game::Entity> > currEntitiesInTrigger;
    
	Timing::Time triggerPhaseTime;
    Ptr<Physics::Shape> collisionShape;
};

__RegisterClass(TriggerProperty);

}; // namespace PhysicsFeature
//------------------------------------------------------------------------------
#endif

