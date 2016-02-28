#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::ActorPhysicsProperty  

    ActorPhysicsProperty adds "actor physics" to an entity. This is 
    mainly a capsule which is always kept upright and responds "immediately"
    to move messages. Attach the ActorPhysicsProperty instead of a 
    PhysicsProperty to an entity.
    
    ActorPhysicsProperty implements the following messages:
    
    MoveGoto
    MoveFollow
    MoveDirection
    MoveStop
    MoveTurn
    MoveRotate
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file	
*/
#include "physicsfeature/properties/physicsproperty.h"
#include "physicsfeature/physicsprotocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "game/entity.h"
#include "physics/character.h"
#include "math/pfeedbackloop.h"
#include "math/angularpfeedbackloop.h"
#include "navigation/path3d.h"
//#include "physics/probesphere.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
class ActorPhysicsProperty : public PhysicsProperty
{
	__DeclareClass(ActorPhysicsProperty);
	__SetupExternalAttributes();
public:
    /// constructor
    ActorPhysicsProperty();
    /// destructor
    virtual ~ActorPhysicsProperty();

    /// setup callbacks for this property, call by entity in OnActivate()
    virtual void SetupCallbacks();
    /// called from Entity::ActivateProperties()
    virtual void OnActivate();
    /// called from Entity::DeactivateProperties()
    virtual void OnDeactivate();
	
    /// called before movement has happened
    virtual void OnMoveBefore();
    /// called after movement has happened
    virtual void OnMoveAfter();
    /// called on debug visualization
    virtual void OnRenderDebug();
    /// called on losing activity (no longer triggered)
    virtual void OnLoseActivity();
    
    /// get a pointer to the physics entity
    virtual const Ptr<Physics::Character>& GetPhysicsEntity() const;
    
    /// override to register accepted messages
    virtual void SetupAcceptedMessages();
    /// handle a single message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

    /// en-/disable autoevade function (default: true)
    void SetAutoEvadeEnable(bool b);
    /// en-/disable autoevade function for static models (default: true)
    void SetAutoEvadeStaticsEnabled(bool b);

protected:
    /// enable and activate the physics
    virtual void EnablePhysics();
    /// disable and cleanup the physics
    virtual void DisablePhysics();

    /// immediately stop the entity
    void Stop();
    /// send a synchronous MoveStop message to self
    void SendStop();
    /// handle a MoveDirection message
    void HandleMoveDirection(BaseGameFeature::MoveDirection* msg);
    /// handle a MoveGoto message
    void HandleMoveGoto(BaseGameFeature::MoveGoto* msg);
    /// handle a SetTransform message
    void HandleSetTransform(BaseGameFeature::SetTransform* msg);
    /// handle a MoveTurn message
    void HandleMoveTurn(BaseGameFeature::MoveTurn* msg);
    /// handle a MoveRotate message
    void HandleMoveRotate(BaseGameFeature::MoveRotate* msg);
    /// handle a MoveFollow message
    void HandleMoveFollow(BaseGameFeature::MoveFollow* msg);
    /// return true if Goto is currently active
    bool IsGotoActive() const;
    /// continue a Goto
    void ContinueGoto();
    /// continue a Follow
    void ContinueFollow();
    /// skip segments
    void SkipSegments();
    /// perform auto evade
    void AutoEvade(Math::float4& targetVec);
    /// check if stuck
    void DetectStuck();

    Ptr<Physics::Character> charPhysicsEntity;

    Math::PFeedbackLoop<Math::vector> smoothedPosition;
    Math::AngularPFeedbackLoop smoothedHeading;

    float followTargetDist;
    float gotoTargetDist;   
    float gotoSegmentDist;
    Ptr<Navigation::Path3D> gotoPath;
    int curGotoSegment;
    Timing::Time gotoTimeStamp;
    Timing::Time stuckTime;
    Timing::Time unStuckTime;
    float headingGain;
    float positionGain;
	bool smoothingEnabled;

    static const float AutoEvadeProbeAboveGround;   ///< dist of auto evade probe to entity's pos
    static const float AutoEvadeProbeRadius;        ///< radius of auto evade probe
    //Ptr<Physics::ProbeSphere> autoEvadeProbe;
	bool disableProcessing;
    bool autoEvadeEnabled;
    bool autoEvadeStaticsEnabled;
    bool withPath;
    bool gotoActive;
    bool isStuck;
    Math::float4 lastUpdatePos;
    Math::point gotoDest;
    Util::String physicsMaterial;	
};
__RegisterClass(ActorPhysicsProperty);

//------------------------------------------------------------------------------
/**
*/
inline
bool
ActorPhysicsProperty::IsGotoActive() const
{
    return this->gotoActive;//this->gotoPath.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ActorPhysicsProperty::SetAutoEvadeStaticsEnabled(bool b)
{
    this->autoEvadeStaticsEnabled = b;
}

}; // namespace Properties
//------------------------------------------------------------------------------