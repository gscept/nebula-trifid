//------------------------------------------------------------------------------
//  properties/actorphysicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/actorphysicsproperty.h"
#include "core/factory.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "physicsattr/physicsattributes.h"
#include "physics/physicsserver.h"
#include "physics/level.h"
#include "basegametiming/gametimesource.h"
#include "math/polar.h"
#include "graphics/graphicsserver.h"
#include "graphics/cameraentity.h"
#include "managers/entitymanager.h"
#include "basegametiming/inputtimesource.h"
//#include "navigation/server.h"
#include "basegameprotocol.h"
#include "physicsprotocol.h"

namespace Attr
{
    DefineFloat(CapsuleRadius, 'CAPR', ReadWrite);
    DefineFloat(CapsuleHeight, 'CAHE', ReadWrite);
    DefineFloat(CapsuleHover, 'CAHO', ReadWrite);
};

namespace PhysicsFeature
{
__ImplementPropertyClass(PhysicsFeature::ActorPhysicsProperty, 'APHP', PhysicsProperty);

using namespace Game;
using namespace Messaging;
using namespace Timing;
using namespace Math;
using namespace Util;
using namespace BaseGameFeature;

const float ActorPhysicsProperty::AutoEvadeProbeAboveGround = 0.4f;

//Radius was changed from 1.0 to 0.5! To avoid problems while trying to enter buildings.
const float ActorPhysicsProperty::AutoEvadeProbeRadius = 0.5f;

//------------------------------------------------------------------------------
/**
*/
ActorPhysicsProperty::ActorPhysicsProperty() :
    followTargetDist(4.0f),
    gotoTargetDist(0.1f),
    gotoSegmentDist(0.2f),
    curGotoSegment(0),
    gotoTimeStamp(0.0),
    headingGain(-6.0f),
    positionGain(-25.0f),
    autoEvadeEnabled(false),
    autoEvadeStaticsEnabled(false),
    withPath(false),
    gotoActive(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ActorPhysicsProperty::~ActorPhysicsProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::SetupDefaultAttributes()
{
    PhysicsProperty::SetupDefaultAttributes();
    SetupAttr(Attr::VelocityVector);
    SetupAttr(Attr::RelVelocity);
    SetupAttr(Attr::MaxVelocity);
    SetupAttr(Attr::Following);
    SetupAttr(Attr::Moving);
    SetupAttr(Attr::TargetEntityId);
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, MoveBefore);
    this->entity->RegisterPropertyCallback(this, MoveAfter);
    this->entity->RegisterPropertyCallback(this, RenderDebug);
    this->entity->RegisterPropertyCallback(this, LoseActivity);
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::OnActivate()
{
    PhysicsProperty::OnActivate();

    //Physics::PhysicsServer* physicsServer = Physics::PhysicsServer::Instance();
    // get physics entity
    Physics::PhysicsEntity* physicsEntity = this->GetPhysicsEntity();
	n_assert(0 != physicsEntity);

    // create and attach auto evade probe
    /*
    this->autoEvadeProbe = Physics::ProbeSphere::Create();
    n_assert(this->autoEvadeProbe.isvalid());
    matrix44 transform = this->GetEntity()->GetMatrix44(Attr::Transform);
    float4 pos = transform.get_position();
    pos.y() += AutoEvadeProbeAboveGround + AutoEvadeProbeRadius;
    transform.set_position(pos);
	this->autoEvadeProbe->SetTransform(transform);
	this->autoEvadeProbe->SetPhysicsEntityToIgnore(physicsEntity);
	this->autoEvadeProbe->SetRadius(AutoEvadeProbeRadius);
	this->autoEvadeProbe->SetMaterialType(Physics::MaterialTable::StringToMaterialType("Probe"));
	physicsServer->GetLevel()->AttachShape(this->autoEvadeProbe);
    */
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::SetAutoEvadeEnable(bool b)
{
    /*
    if (b && !this->autoEvadeEnabled)
    {
        Physics::PhysicsServer* physicsServer = Physics::PhysicsServer::Instance();
	    Physics::PhysicsEntity* physicsEntity = this->GetPhysicsEntity();
	    n_assert(0 != physicsEntity);

        // create and attach probe
        this->autoEvadeProbe = Physics::ProbeSphere::Create();
        n_assert(this->autoEvadeProbe.isvalid());
        matrix44 transform = this->GetEntity()->GetMatrix44(Attr::Transform);
        float4 pos = transform.get_position();
        pos.y() += AutoEvadeProbeAboveGround + AutoEvadeProbeRadius;
        transform.set_position(pos);
	    this->autoEvadeProbe->SetTransform(transform);
	    this->autoEvadeProbe->SetPhysicsEntityToIgnore(physicsEntity);
	    this->autoEvadeProbe->SetRadius(AutoEvadeProbeRadius);
	    this->autoEvadeProbe->SetMaterialType(Physics::MaterialTable::StringToMaterialType("Probe"));
	    physicsServer->GetLevel()->AttachShape(this->autoEvadeProbe);
    }
    else if (!b && this->autoEvadeEnabled)
    {
        // detach probes
	    Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
	    n_assert(0 != physicsLevel);
        physicsLevel->RemoveShape(this->autoEvadeProbe);
    }

    this->autoEvadeEnabled = b;
    */
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::OnDeactivate()
{
    this->SetAutoEvadeEnable(false);
    PhysicsProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Physics::PhysicsEntity>
ActorPhysicsProperty::CreatePhysicsEntity() const
{
    return (Physics::PhysicsEntity*)Physics::CharEntity::Create();
}

//------------------------------------------------------------------------------
/**
    Creates a Physics::CharEntity instead of a normal Physics::PhysicsEntity.
*/
void
ActorPhysicsProperty::EnablePhysics()
{
    n_assert(!this->IsEnabled());
    
    // create a char physics entity
	this->charPhysicsEntity = CreatePhysicsEntity().cast<Physics::CharEntity>();
    this->charPhysicsEntity->SetUserData(this->GetEntity()->GetUniqueId());

    if (physicsMaterial.IsValid())
    {
        this->charPhysicsEntity->SetOverwriteMaterialType(physicsMaterial);
    }

    if (this->GetEntity()->HasAttr(Attr::Physics))
    {
        this->charPhysicsEntity->SetResourceName(this->GetEntity()->GetString(Attr::Physics));
    }

    if (this->GetEntity()->HasAttr(Attr::CapsuleRadius))
    {
        this->charPhysicsEntity->SetRadius(this->GetEntity()->GetFloat(Attr::CapsuleRadius));
    }
    if (this->GetEntity()->HasAttr(Attr::CapsuleHeight))
    {
        this->charPhysicsEntity->SetHeight(this->GetEntity()->GetFloat(Attr::CapsuleHeight));
    }
    if (this->GetEntity()->HasAttr(Attr::CapsuleHover))
    {
        this->charPhysicsEntity->SetHover(this->GetEntity()->GetFloat(Attr::CapsuleHover));
    }
   
    this->charPhysicsEntity->SetTransform(this->GetEntity()->GetMatrix44(Attr::Transform));
    // attach physics entity to physics level
    Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
    n_assert(physicsLevel);
    physicsLevel->AttachEntity(this->charPhysicsEntity);

    // make sure we are standing still
    this->Stop();

    // initialize feedback loops for motion smoothing
    Time time = GameTimeSource::Instance()->GetTime();
    matrix44 entityMatrix = this->GetEntity()->GetMatrix44(Attr::Transform);
    this->smoothedPosition.Reset(time, 0.001f, this->positionGain, entityMatrix.get_position());

    polar headingAngle(entityMatrix.get_zaxis());
    this->smoothedHeading.Reset(time, 0.001f, this->headingGain, headingAngle.rho);

    // add probe material to ground exclude set
	Physics::CharEntity* physicsEntity = this->GetPhysicsEntity().cast<Physics::CharEntity>();
	n_assert(0 != physicsEntity && physicsEntity->IsA(Physics::CharEntity::RTTI));
	physicsEntity->GetGroundExcludeSet().AddMaterialType(Physics::MaterialTable::StringToMaterialType("Probe"));

    // call parrent
    PhysicsProperty::EnablePhysics();
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::DisablePhysics()
{
    n_assert(this->IsEnabled());
    
    // stop
    this->Stop();

    // remove from level
    Physics::Level* physicsLevel = Physics::PhysicsServer::Instance()->GetLevel();
    n_assert(physicsLevel);
    physicsLevel->RemoveEntity(this->charPhysicsEntity);

    // cleanup resource
    this->charPhysicsEntity = 0;

    // call parrent
    PhysicsProperty::DisablePhysics();
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(MoveDirection::Id);
    this->RegisterMessage(MoveFollow::Id);
    this->RegisterMessage(MoveGoto::Id);
    this->RegisterMessage(MoveStop::Id);
    this->RegisterMessage(SetTransform::Id);
    this->RegisterMessage(MoveTurn::Id);
    this->RegisterMessage(MoveSetVelocity::Id);
    this->RegisterMessage(MoveRotate::Id);
    this->RegisterMessage(GetPhysicsEntity::Id);
    PhysicsProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void 
ActorPhysicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    if (!this->IsEnabled())
    {
        // disabled, don't listen to messages
        PhysicsProperty::HandleMessage(msg);
    }
    else if (msg->CheckId(MoveDirection::Id))
    {
        this->HandleMoveDirection((MoveDirection*) msg.get());
    }
    else if (msg->CheckId(MoveFollow::Id))
    {
        this->HandleMoveFollow((MoveFollow*) msg.get());
    }
    else if (msg->CheckId(MoveGoto::Id))
    {
        this->HandleMoveGoto((MoveGoto*) msg.get());
    }
    else if (msg->CheckId(MoveStop::Id))
    {
        this->Stop();
    }
    else if (msg->CheckId(SetTransform::Id))
    {
        this->HandleSetTransform((SetTransform*) msg.get());
    }
    else if (msg->CheckId(MoveTurn::Id))
    {
        this->HandleMoveTurn((MoveTurn*) msg.get());
    }
    else if (msg->CheckId(MoveRotate::Id))
    {
        this->HandleMoveRotate((MoveRotate*) msg.get());
    }
    else if (msg->CheckId(GetPhysicsEntity::Id))
    {
        ((PhysicsFeature::GetPhysicsEntity*)msg.get())->SetEntity(this->GetPhysicsEntity());
    }
    else if (msg->CheckId(MoveSetVelocity::Id))
    {
        this->GetEntity()->SetFloat(Attr::RelVelocity, ((MoveSetVelocity*)msg.get())->GetRelVelocity());
    }
    else
    {
        PhysicsProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
    The OnMoveBefore() method handles all pending messages, and other 
    per-frame-stuff that must happen before the physics subsystem is
    triggered.
*/
void
ActorPhysicsProperty::OnMoveBefore()
{
    if (this->IsEnabled())
    {
        if (this->IsGotoActive())
        {
            this->ContinueGoto();
        }
        if (this->GetEntity()->GetBool(Attr::Following))
        {
            this->ContinueFollow();
        }
    }
}

//------------------------------------------------------------------------------
/**
    The OnMoveAfter() method transfers the current physics entity transform to 
    the game entity.
*/
void
ActorPhysicsProperty::OnMoveAfter()
{
    if (this->IsEnabled())
    {
        matrix44 physicsEntityTransform = this->charPhysicsEntity->GetTransform();
        vector physicsEntityVelocity = this->charPhysicsEntity->GetVelocity();

        // feed the feedback loops
        polar headingAngles(physicsEntityTransform.get_zaxis());
        this->smoothedPosition.SetGoal(physicsEntityTransform.get_position());   
        this->smoothedHeading.SetGoal(headingAngles.rho);

        // if current state and goal are already close to each other,
        // we don't send a position update
        // NOTE: this check must be made before the feedback look update!
        vector posError = this->smoothedPosition.GetState() - this->smoothedPosition.GetGoal();
        float headingError = n_abs(this->smoothedHeading.GetState() - this->smoothedHeading.GetGoal());

        // always evaluate the feedback loops to keep them in valid time delta
        Time time = GameTimeSource::Instance()->GetTime();
        this->smoothedPosition.Update(time);
        this->smoothedHeading.Update(time);

        // only send update transform if anything changed
        if ((posError.length() > 0.001f) || (headingError > 0.001f))
        {
            // construct the new entity matrix
            matrix44 entityMatrix = matrix44::rotationy(this->smoothedHeading.GetState());
            entityMatrix.translate(this->smoothedPosition.GetState());

            // update game entity
            Ptr<UpdateTransform> msg = UpdateTransform::Create();
            msg->SetMatrix(entityMatrix);
            this->GetEntity()->SendSync(msg.upcast<Message>());
            this->GetEntity()->SetFloat4(Attr::VelocityVector, physicsEntityVelocity);
        }
    }

    /*
    if (this->autoEvadeEnabled)
    {
        // update autoevadeprobe
        matrix44 transform = this->GetEntity()->GetMatrix44(Attr::Transform);
        float4 pos = transform.get_position();
        pos.y() += AutoEvadeProbeAboveGround + AutoEvadeProbeRadius;
        transform.set_position(pos);
        this->autoEvadeProbe->SetTransform(transform);
    }
    */
}

//------------------------------------------------------------------------------
/**
    This simply sends a synchronous stop message to myself. This uses
    a message so that everybody else who might be interested in the information
    that I have stopped can listen to the message.
*/
void
ActorPhysicsProperty::SendStop()
{
    Ptr<MoveStop> msg = MoveStop::Create();
    this->GetEntity()->SendSync(msg.upcast<Message>());
}

//------------------------------------------------------------------------------
/**
    Immediately stop the entity.

    26-Jan-06   floh    bugfix: also cancelled MoveFollow
    14-Feb-06   nico    bugfix: now really cancelled MoveFollow ;)
*/
void
ActorPhysicsProperty::Stop()
{
	// release existing goto path in progress
    /*
	if (this->gotoPath.isvalid() && !this->gotoPath->IsCompleted())
	{
	    Navigation::Server::Instance()->ReleasePath(this->gotoPath);
	}
    this->gotoPath = 0;
    */

	this->charPhysicsEntity->SetDesiredVelocity(vector(0.0f, 0.0f, 0.0f));
    this->GetEntity()->SetBool(Attr::Moving, false);
    this->GetEntity()->SetBool(Attr::Following, false);
    this->GetEntity()->SetFloat4(Attr::VelocityVector, vector(0.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------
/**
    Handle a MoveDirection message.
*/
void
ActorPhysicsProperty::HandleMoveDirection(MoveDirection* msg)
{
    n_assert(msg);

    vector dir = msg->GetDirection();

    // convert camera relative vector into absolute vector if necessary
    if (msg->GetCameraRelative())
    {
        const Ptr<Graphics::View>& curView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
        Graphics::CameraEntity* camera = curView->GetCameraEntity();
        n_assert(camera);
        matrix44 camTransform = camera->GetTransform();
        camTransform.set_position(float4(0.0f, 0.0f, 0.0f, 1.0f));
        dir = matrix44::transform(dir, camTransform);
    }
    dir.y() = 0.0f;
    dir = vector::normalize(dir);

    /*
    this->AutoEvade(dir);
    dir = vector::normalize(dir);
    */
        
    vector desiredVelocity = dir * this->GetEntity()->GetFloat(Attr::RelVelocity) * this->GetEntity()->GetFloat(Attr::MaxVelocity);
    this->charPhysicsEntity->SetDesiredVelocity(desiredVelocity, msg->GetMaxMovement());
    this->charPhysicsEntity->SetDesiredLookat(dir);
    this->GetEntity()->SetBool(Attr::Moving, true);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveTurn message.
*/
void
ActorPhysicsProperty::HandleMoveTurn(MoveTurn* msg)
{
    n_assert(msg);
    vector dir = msg->GetDirection();
    if (msg->GetCameraRelative())
    {
        const Ptr<Graphics::View>& curView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
        Graphics::CameraEntity* camera = curView->GetCameraEntity();
        n_assert(camera);
        matrix44 camTransform = camera->GetTransform();
        camTransform.set_position(float4(0.0f, 0.0f, 0.0f, 1.0f));
        dir = matrix44::transform(dir, camTransform);
    }
    dir.y() = 0.0f;
    dir = vector::normalize(dir);
    this->charPhysicsEntity->SetDesiredLookat(dir);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveTurn message.
*/
void
ActorPhysicsProperty::HandleMoveRotate(MoveRotate* msg)
{
    n_assert(msg);
    
    Time frameTime = InputTimeSource::Instance()->GetFrameTime();
    float angle = msg->GetAngle() * (float)frameTime;
    vector direction = this->charPhysicsEntity->GetDesiredLookat();    
    direction = matrix44::transform(direction, matrix44::rotationy(angle));
    this->charPhysicsEntity->SetDesiredLookat(direction);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveGoto message.
*/
void
ActorPhysicsProperty::HandleMoveGoto(MoveGoto* msg)
{
    n_assert(msg);
	
    // cleanup current movement
    this->Stop();

    //make a navigation path from current to target position
    this->gotoDest = msg->GetPosition();

    /*
    const point& from = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
    const point& to = msg->GetPosition();
    if (this->gotoPath.isvalid())
    {
        if (this->gotoPath->IsCompleted())
        {
            this->gotoPath = Navigation::Server::Instance()->MakePath(from, to);
            n_assert(!this->gotoPath->IsCompleted() || this->gotoPath->CountSegments() > 0);
            this->curGotoSegment = 0;
        }
        else
        {
            Navigation::Server::Instance()->UpdatePath(this->gotoPath, to);
        }
    }
    else
    {
        n_assert(!this->gotoPath.isvalid());
        this->gotoPath = Navigation::Server::Instance()->MakePath(from, to);
        n_assert(!this->gotoPath->IsCompleted() || this->gotoPath->CountSegments() > 0);
       this->curGotoSegment = 0;
    }
    */
    
    this->gotoActive = true;
    this->gotoTargetDist = msg->GetDistance();
    this->gotoTimeStamp = GameTimeSource::Instance()->GetTime();
    this->GetEntity()->SetBool(Attr::Moving, true);

    this->ContinueGoto();
}

//------------------------------------------------------------------------------
/**
    Handle a SetTransform message.
*/
void
ActorPhysicsProperty::HandleSetTransform(SetTransform* msg)
{
    n_assert(msg);
    this->charPhysicsEntity->SetTransform(msg->GetMatrix());   

    // reset the feedback loops 
    Time time = GameTimeSource::Instance()->GetTime();
    this->smoothedPosition.Reset(time, 0.001f, this->positionGain, msg->GetMatrix().get_position());
    polar headingAngle(msg->GetMatrix().get_zaxis());
    this->smoothedHeading.Reset(time, 0.001f, this->headingGain, headingAngle.rho);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveFollow message.
*/
void
ActorPhysicsProperty::HandleMoveFollow(MoveFollow* msg)
{
    n_assert(msg);
    this->GetEntity()->SetInt(Attr::TargetEntityId, msg->GetTargetEntityId());
    if (EntityManager::Instance()->ExistsEntityByUniqueId(this->GetEntity()->GetInt(Attr::TargetEntityId)))
    {
        Entity* targetEntity = EntityManager::Instance()->GetEntityByUniqueId(this->GetEntity()->GetInt(Attr::TargetEntityId));
        n_assert(targetEntity);
        this->followTargetDist = msg->GetDistance();
        this->GetEntity()->SetBool(Attr::Following, true);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::SkipSegments()
{
    /*
    point curPos = this->GetEntity()->GetMatrix44(Attr::Transform).pos_component();
    const Util::Array<point>& gotoSegments = this->gotoPath->GetPoints();
    int numGotoSegments = gotoSegments.Size();
    for (;this->curGotoSegment < numGotoSegments-1; this->curGotoSegment++)
    {
        // skip waypoint, if actor stands between current and next waypoint
        if (this->curGotoSegment > 0)
        {
            vector vecLastCur = gotoSegments[this->curGotoSegment] - gotoSegments[this->curGotoSegment-1];
            vector vecMeCur = gotoSegments[this->curGotoSegment] - curPos;
            if (vecLastCur.dot(vecMeCur) < 0.0f)
            {
                continue;
            }
        }
        
        point cur = gotoSegments[this->curGotoSegment];
        point next = gotoSegments[this->curGotoSegment+1];
        point myPos = curPos;
        cur.y() *= 2;
        next.y() *= 2;
        myPos.y() *= 2;
        float distMeNext = vector(myPos - next).lengeth();
        float distMeCur = vector(myPos - cur).lengeth();
        float distCurNext = vector(cur - next).lengeth();
        if (distMeNext <= distCurNext || distMeNext < distMeCur)
        {
            continue;
        }
        break;
    }
    */
}

//------------------------------------------------------------------------------
/**
Function was changed(11.07.07). Now AutoEvade()will be called by HandleMoveDirection()
to avoid replacement of non-static physic objekts. 
*/
void
ActorPhysicsProperty::AutoEvade(vector& targetVec)
{
    /*
    //Check if path is valid
    if(this->gotoPath.isvalid())
    {
        this->withPath = true;
    }
    
    vector curPos = this->GetEntity()->GetMatrix44(Attr::Transform).pos_component();
    Array<vector> gotoSegments;
    vector firstVec;

    //Only do this if we have a valid path
    if (withPath)
    {
        gotoSegments = (Array<vector>)this->gotoPath->GetPoints();
        /// --- autoevade ---
        // compute a smoothed target vector out of all the next path nodes within 3 meters but minimum 2 nodes
        targetVec = vector(0,0,0);
        firstVec = vector(0,0,0);
        int pointIndex;
        for (pointIndex = this->curGotoSegment; 
             pointIndex < gotoSegments.Size() && 
             (vector(gotoSegments[pointIndex] - curPos).length() < 3.0f ||
             pointIndex < this->curGotoSegment+1);
             pointIndex++)
        {
            vector nextVec = gotoSegments[pointIndex] - curPos;
            nextVec.y() *= 2;
            if (pointIndex == this->curGotoSegment)
            {
                firstVec = nextVec;
            }
            else if(firstVec.dot(nextVec) < -0.25f)
            {
                // if angle between next vector and first vector is too great, 
                // break smoothing to avoid running against walls etc...
                break;
            }
            nextVec.y() = 0.0f;
            nextVec.norm();
            targetVec += nextVec;
        }
        targetVec.norm();
        firstVec.y() = 0;
        firstVec.norm();
    }

    // test collision of autoevadeprobe
    this->autoEvadeProbe->Collide();
    const Array<Physics::ContactPoint>& contactPoints = this->autoEvadeProbe->GetContactPoints();
    if (contactPoints.Size() > 0)
    {
        vector evadeVec(0,0,0);

        // a vector 90° to the targetVec
        vector targetVecCross = targetVec;
        targetVecCross.rotate(vector(0,1,0), 0.5f*N_PI);
        int i;
        for (i=0; i < contactPoints.Size(); i++)
        {
            bool useFirstVec = false;
            vector contactDir(0,0,0);
            float depth = 0;

            // collisions with static obstacles will be ignored if
            // there are also collisions with dynamic entities
            // or if flag is turned to false
            if (0 != contactPoints[i].GetEntity() || 
                (this->autoEvadeStaticsEnabled && !this->autoEvadeProbe->HasDynamicCollisions()))
            {
                point colliderPos = contactPoints[i].GetPosition();
                Physics::PhysicsEntity* physicsEntity = contactPoints[i].GetEntity();
                Ptr<Game::Entity> collidedEntity = 0;
                if (physicsEntity)
                {
                    collidedEntity = EntityManager::Instance()->GetEntityByUniqueId(physicsEntity->GetUserData());
                }
                point myPos = curPos;
                colliderPos.y() = 0;
                myPos.y() = 0;
                
                //Only do this if we have a valid path
                if(withPath)
                {
                    //if target point lies near to collision point stop movement
                    if (this->curGotoSegment == this->gotoPath->CountSegments() - 1)
                    {
                        point curTarget = gotoSegments[this->curGotoSegment];
                        curTarget.y() = 0;
                        if (AutoEvadeProbeRadius-0.3f > vector(colliderPos - curTarget).length())
                        {
                            // end point reached
                            this->SendStop();
                            targetVec.set(0,0,0);
                            return;
                        }
                    }
                }

                // direction to collision
                contactDir = colliderPos - myPos;
   
                // depth is a factor to make near collisions more important     
                depth = pow(n_clamp(1 + (AutoEvadeProbeRadius - vector(myPos - colliderPos).length())/AutoEvadeProbeRadius, 0, 3), 3);
              
                // Only do this if we have a valid path
                if(withPath)
                {
                    if (contactDir.dot(firstVec) < -0.25f)
                    {
                        useFirstVec = true;
                    }
                }
            }

            // ignore collisions that are too far
            if (depth > 0)
            {
                if (useFirstVec)
                {
                    evadeVec += firstVec * depth;
                }
                else
                {
                    contactDir.y() = 0;
                    contactDir.norm();
                    // evade direction is 90° to the collision direction
                    vector evadeDir = contactDir;
                    evadeDir.rotate(vector(0,1,0), 0.5f*N_PI);
                    contactDir.norm();
                    const vector lookingAt = -this->GetEntity()->GetMatrix44(Attr::Transform).z_component();
                    
                    Physics::PhysicsEntity* physicsEntity = contactPoints[i].GetEntity();
                    Ptr<Game::Entity> collidedEntity = 0;
                    if (physicsEntity)
                    {
                        collidedEntity = EntityManager::Instance()->GetEntityByUniqueId(physicsEntity->GetUserData());
                    }

                    // decide to turn left or right around obstacle
                    float leftRight;
                    if (collidedEntity.isvalid() && collidedEntity->HasProperty(ActorPhysicsProperty::RTTI))
                    {
                        if (collidedEntity->HasAttr(Attr::Moving) &&
                            collidedEntity->GetBool(Attr::Moving))
                        {
                            if (this->GetEntity()->GetMatrix44(Attr::Transform).z_component().dot(collidedEntity->GetMatrix44(Attr::Transform).z_component()) > 0.7)
                            {
                                break;
                            }
                        }
                        point opponentPos = collidedEntity->GetMatrix44(Attr::Transform).pos_component();
                        point myPos = this->GetEntity()->GetMatrix44(Attr::Transform).pos_component();
                        opponentPos.y() = 0;
                        myPos.y() = 0;
                        vector dir = opponentPos - myPos;
                        dir.norm();

                        if (lookingAt.dot(dir) > 0.7f &&
                            targetVec.dot(collidedEntity->GetMatrix44(Attr::Transform).z_component()) > -0.5f)
                        {
                            if (targetVecCross.dot(collidedEntity->GetMatrix44(Attr::Transform).z_component()) > 0) leftRight = 1;
                            else leftRight = -1;
                        }
                        else
                        {
                            if (lookingAt.dot(evadeDir) > 0) leftRight = 1;
                            else leftRight = -1;
                        }
                    }
                    else if (lookingAt.dot(contactDir) > 0.8f)
                    {
                        if (targetVecCross.dot(contactDir) > 0) leftRight = -1;
                        else leftRight = 1;
                    }
                    else
                    {
                        // if looking aside the contactPoint keep following the evade direction
                        if (lookingAt.dot(evadeDir) > 0) leftRight = 1;
                        else leftRight = -1;
                    }
                    
                    // the more frontal an obstacle is the more important
                    // obstacles behind will be ignored
                    float correctionFactor = n_clamp(targetVec.dot(contactDir), 0, 1.0f);
                    evadeVec += evadeDir * leftRight * correctionFactor * depth;
                }                
            }
        }
        targetVec = evadeVec + targetVec * n_clamp(1 - evadeVec.length(), 0, 1);
    }
    */
}

//------------------------------------------------------------------------------
/**
    Continue the current Goto action.
*/
void
ActorPhysicsProperty::ContinueGoto()
{
    /*
    //n_assert(this->gotoPath.isvalid());

	// goto path is not completed yet
	//if (!this->gotoPath->IsCompleted()) return;

    /// skip evaded goto segments
    //this->SkipSegments();

    //const Array<point>& gotoSegments = this->gotoPath->GetPoints();

    point curPos = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
    point curSegment = this->gotoDest;

    //FIXME: create a unsubstantial Array only for testing
    Array<point> gotoSegments;
    gotoSegments.Append(curPos+curPos);

    //compute vector to segment
    vector targetVec(curSegment - curPos);
    targetVec.y() = 0.0f;
    float dist = targetVec.length();
    targetVec=vector::normalize(targetVec);

    //stop if in range and every following segment too!
    float distToFinalTarget = vector(gotoSegments.Back()- curPos).length();
    if (distToFinalTarget < this->gotoTargetDist)
    {
        bool reached = true;
        IndexT i;
        for (i=this->curGotoSegment; i < gotoSegments.Size(); i++)
        {
            if (vector(gotoSegments[i] - gotoSegments.Back()).length() > this->gotoTargetDist)
            {
                reached = false;
            }
        }
        if (reached)
        {
            // end point reached
            this->SendStop();
            return;
        }
    }
    if (this->curGotoSegment == gotoSegments.Size()-1)
    {
        if (dist < this->gotoTargetDist)
        {
            // end point reached
            this->SendStop();
            return;
        }
    }
    else if (dist < this->gotoSegmentDist)
    {
        // current segment position reached
        this->curGotoSegment++;
        return;
    }

    // evade dynamic obstacles
    if (this->autoEvadeEnabled)
    {
        this->AutoEvade(targetVec);
    }

    if (targetVec.length() > TINY)
    {
        // just continue to go towards current segment position
        Ptr<MoveDirection> msg = MoveDirection::Create();
        msg->SetDirection(targetVec);
        msg->SetMaxMovement(dist);
        this->GetEntity()->SendSync(msg.upcast<Messaging::Message>());
    }
    */
}

//------------------------------------------------------------------------------
/**
    Continue the current Follow action.
*/
void
ActorPhysicsProperty::ContinueFollow()
{
    /*
    n_assert(this->GetEntity()->GetBool(Attr::Following));
 
    if (!EntityManager::Instance()->ExistsEntityByUniqueId(this->GetEntity()->GetInt(Attr::TargetEntityId)))
    {
        // our target entity has gone...
        this->SendStop();
    }
    else
    {
        Game::Entity* targetEntity = EntityManager::Instance()->GetEntityByUniqueId(this->GetEntity()->GetInt(Attr::TargetEntityId));
        n_assert(targetEntity);

        // compute positions in world coordinates
        const vector targetPos = targetEntity->GetMatrix44(Attr::Transform).pos_component();
        const vector curPos = this->GetEntity()->GetMatrix44(Attr::Transform).pos_component();
        vector targetVec = targetPos - curPos;
        targetVec.y() = 0.0f; // ignore vertical dimension
        float targetDist = targetVec.length();

        // if we are close enough to our target, stop, and always face our target
        if (targetDist < this->followTargetDist)
        {
           // stop and look at our target
           this->SendStop();
           this->charPhysicsEntity->SetDesiredLookat(targetVec);
        }
        // continue following target position if not stopped.
        else if ((this->gotoTimeStamp + 1.0) < GameTimeSource::Instance()->GetTime())
        {
            // check if new goto calculation is necessary
	        if (this->gotoPath.isvalid())
	        {
		        if (this->gotoPath->IsCompleted())
		        {
			        // if destination is the same there is no need to find a new path
			        float dist = (this->gotoPath->GetTarget() - targetPos).length();
			        if (dist < this->gotoTargetDist) return;
		        }
		        else
		        {
			        // just update path calculation
			        Navigation::Server::Instance()->UpdatePath(this->gotoPath, targetPos);
			        return;
		        }
	        }

            // continue moving towards our target entity
            Ptr<MoveGoto> moveGoto = MoveGoto::Create();
            moveGoto->SetPosition(targetPos);
            this->HandleMoveGoto(moveGoto);
        }
    }
    */
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Physics::PhysicsEntity>
ActorPhysicsProperty::GetPhysicsEntity() const
{
	return this->charPhysicsEntity.cast<Physics::PhysicsEntity>();
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the current 3d navigation path.
*/
void
ActorPhysicsProperty::OnRenderDebug()
{
    
    /*if (this->gotoPath.isvalid())
    {
        const Array<vector>& points = this->gotoPath->GetPoints();
        int i;
        for (i = 0; i < points.Size(); i++)
        {
            matrix44 m;
            m.scale(vector(0.1f, 0.1f, 0.1f));
            m.translate(points[i] + vector(0.0f, 0.1f, 0.0f));
            float4 color(1.0f, 0.0f, 0.0f, 0.5f);
            if (i == this->curGotoSegment) color.set(1.0f, 1.0f, 0.0f, 0.5f);
            else if (i < this->curGotoSegment) color.set(0.0f, 1.0f, 0.0f, 0.1f);
            gfxServer->DrawShape(nGfxServer2::Box, m, color);
        }
    }*/
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::OnLoseActivity()
{
    if (this->IsEnabled())
    {
    	this->charPhysicsEntity->SetDesiredVelocity(vector(0.0f, 0.0f, 0.0f));
    }
}

} // namespace Properties
