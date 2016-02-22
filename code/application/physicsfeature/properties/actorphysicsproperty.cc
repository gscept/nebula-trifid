//------------------------------------------------------------------------------
//  properties/actorphysicsproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/actorphysicsproperty.h"
#include "core/factory.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "physics/physicsserver.h"
#include "physics/scene.h"
#include "basegametiming/gametimesource.h"
#include "math/polar.h"
#include "graphics/graphicsserver.h"
#include "graphics/cameraentity.h"
#include "managers/entitymanager.h"
#include "basegametiming/inputtimesource.h"
#include "navigation/navigationserver.h"
#include "debugrender/debugshaperenderer.h"

namespace PhysicsFeature
{
__ImplementClass(PhysicsFeature::ActorPhysicsProperty, 'APHP', PhysicsFeature::PhysicsProperty);

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
    gotoActive(false),
	disableProcessing(false),
    stuckTime(0),
    isStuck(false),
    unStuckTime(0),
	smoothingEnabled(false)
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
ActorPhysicsProperty::SetupCallbacks()
{
	if (this->IsSimulationHost())
	{
		this->entity->RegisterPropertyCallback(this, MoveBefore);
		this->entity->RegisterPropertyCallback(this, MoveAfter);
		this->entity->RegisterPropertyCallback(this, LoseActivity);
	}    
	this->entity->RegisterPropertyCallback(this, BeginFrame);
    this->entity->RegisterPropertyCallback(this, RenderDebug);    
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::OnActivate()
{
    PhysicsProperty::OnActivate();
	this->smoothingEnabled = this->entity->GetBool(Attr::AngularSmoothing);
    //Physics::PhysicsServer* physicsServer = Physics::PhysicsServer::Instance();
    // get physics entity
    /*Physics::PhysicsEntity* physicsEntity = this->GetPhysicsEntity();
	n_assert(0 != physicsEntity);*/

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
    Creates a Physics::CharEntity instead of a normal Physics::PhysicsEntity.
*/
void
ActorPhysicsProperty::EnablePhysics()
{
	if (this->IsSimulationHost())
	{
		n_assert(!this->IsEnabled());
		this->disableProcessing = false;

		// create a char physics entity
		this->charPhysicsEntity = Physics::Character::Create();
		this->charPhysicsEntity->SetUserData(this->GetEntity().upcast<Core::RefCounted>());

		//if (physicsMaterial.IsValid())
		//{
		//    this->charPhysicsEntity->SetOverwriteMaterialType(physicsMaterial);
		//}

		if (this->GetEntity()->HasAttr(Attr::CapsuleRadius))
		{
			this->charPhysicsEntity->SetRadius(this->GetEntity()->GetFloat(Attr::CapsuleRadius));
		}
		if (this->GetEntity()->HasAttr(Attr::CapsuleHeight))
		{
			this->charPhysicsEntity->SetHeight(this->GetEntity()->GetFloat(Attr::CapsuleHeight));
		}
		if (this->GetEntity()->HasAttr(Attr::CrouchingCapsuleHeight))
		{
			this->charPhysicsEntity->SetCrouchingHeight(this->GetEntity()->GetFloat(Attr::CrouchingCapsuleHeight));
		}
		else
		{
			this->charPhysicsEntity->SetCrouchingHeight(this->charPhysicsEntity->GetHeight());
		}

		if (this->GetEntity()->HasAttr(Attr::CharacterShape))
		{
			Util::String shapeString = this->GetEntity()->GetString(Attr::CharacterShape);
			shapeString.ToLower();

			if (shapeString.IsValid())
			{
				Physics::Character::CharacterShape shape;

				if ("capsule" == shapeString)
				{
					shape = Physics::Character::Capsule;
				}
				else if ("cylinder" == shapeString)
				{
					shape = Physics::Character::Cylinder;
				}
				else
				{
					n_error("Unrecognized shape '%s'!", shapeString.AsCharPtr());
				}

				this->charPhysicsEntity->SetShape(shape);
			}
		}

		//if (this->GetEntity()->HasAttr(Attr::CapsuleHover))
		//{
		//  this->charPhysicsEntity->SetHover(this->GetEntity()->GetFloat(Attr::CapsuleHover));
		//}

		this->charPhysicsEntity->SetTransform(this->GetEntity()->GetMatrix44(Attr::Transform));
		// attach physics entity to physics level
		const Ptr<Physics::Scene>& physicsLevel = Physics::PhysicsServer::Instance()->GetScene();
		n_assert(physicsLevel);
		physicsLevel->Attach(this->charPhysicsEntity.upcast<Physics::PhysicsObject>());

		this->charPhysicsEntity->SetMaxJumpHeight(this->GetEntity()->GetFloat(Attr::JumpHeight));
		this->charPhysicsEntity->SetJumpSpeed(this->GetEntity()->GetFloat(Attr::JumpSpeed));

		this->charPhysicsEntity->SetMovementSpeed(this->GetEntity()->GetFloat(Attr::MaxVelocity));
		this->charPhysicsEntity->SetMaxLinearAcceleration(this->GetEntity()->GetFloat(Attr::RelVelocity));

		if (this->GetEntity()->HasAttr(Attr::VelocityGain))
		{
			this->charPhysicsEntity->SetVelocityGain(this->GetEntity()->GetFloat(Attr::VelocityGain), this->GetEntity()->GetFloat(Attr::AirVelocityGain));
		}

		// make sure we are standing still
		this->Stop();

		// initialize feedback loops for motion smoothing
		Time time = GameTimeSource::Instance()->GetTime();
		matrix44 entityMatrix = this->GetEntity()->GetMatrix44(Attr::Transform);
		this->smoothedPosition.Reset(time, 0.001f, this->positionGain, entityMatrix.get_position());

		polar headingAngle(entityMatrix.get_zaxis());
		this->smoothedHeading.Reset(time, 0.001f, this->headingGain, headingAngle.rho);

		this->enabled = true;
		if (this->entity->HasAttr(Attr::CollisionFeedback))
		{
			this->charPhysicsEntity->SetEnableCollisionCallback(this->entity->GetBool(Attr::CollisionFeedback));
		}
		//   // add probe material to ground exclude set
		//Physics::CharEntity* physicsEntity = this->GetPhysicsEntity().cast<Physics::CharEntity>();
		//n_assert(0 != physicsEntity && physicsEntity->IsA(Physics::CharEntity::RTTI));
		//physicsEntity->GetGroundExcludeSet().AddMaterialType(Physics::MaterialTable::StringToMaterialType("Probe"));
	}
	else
	{
		Ptr<Physics::Collider> coll = Physics::Collider::Create();
		Math::matrix44 offset;
		float radius = this->GetEntity()->GetFloat(Attr::CapsuleRadius);
		float height = this->GetEntity()->GetFloat(Attr::CapsuleHeight);
		offset.translate(Math::vector(0, height*0.5f + 2.0f* radius, 0));
		Physics::ColliderDescription desc;
		desc.type = Physics::ColliderCapsule;
		desc.capsule.radius = radius;
		desc.capsule.height = height;
		desc.transform = offset;
		coll->AddFromDescription(desc);
		Physics::PhysicsCommon common;
		common.bodyFlags = Physics::Kinematic;
		common.collider = coll;
		common.mass = 80.0f;
		common.type = Physics::PhysicsBody::RTTI.GetFourCC();
		this->physicsEntity = Physics::PhysicsObject::CreateFromTemplate(common).cast<Physics::PhysicsBody>();
		Physics::PhysicsServer::Instance()->GetScene()->Attach(this->physicsEntity.cast<Physics::PhysicsObject>());

		this->physicsEntity->SetUserData(this->entity.cast<Core::RefCounted>());
		this->enabled = true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::DisablePhysics()
{
    n_assert(this->IsEnabled());
 
	if (this->IsSimulationHost())
	{
		// stop
		this->Stop();

		// remove from level
		const Ptr<Physics::Scene>& physicsLevel = Physics::PhysicsServer::Instance()->GetScene();
		n_assert(physicsLevel);
		physicsLevel->Detach(this->charPhysicsEntity.upcast<Physics::PhysicsObject>());

		// cleanup resource
		this->charPhysicsEntity = 0;
	}
    // call parent
    PhysicsProperty::DisablePhysics();
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::SetupAcceptedMessages()
{
	if (this->IsSimulationHost())
	{
		this->RegisterMessage(MoveDirection::Id);
		this->RegisterMessage(MoveFollow::Id);
		this->RegisterMessage(MoveGoto::Id);
		this->RegisterMessage(MoveStop::Id);
		this->RegisterMessage(SetTransform::Id);
		this->RegisterMessage(MoveTurn::Id);
		this->RegisterMessage(MoveSetVelocity::Id);
		this->RegisterMessage(MoveRotate::Id);
		this->RegisterMessage(MoveJump::Id);
		this->RegisterMessage(GetPhysicsObject::Id);
		this->RegisterMessage(PlayerDeath::Id);
		this->RegisterMessage(PlayerEnable::Id);
		this->RegisterMessage(Crouch::Id);
		PhysicsProperty::SetupAcceptedMessages();
	}
	else
	{		
		this->RegisterMessage(Crouch::Id);
		TransformableProperty::SetupAcceptedMessages();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ActorPhysicsProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	
	if (msg->CheckId(PlayerEnable::Id))
	{
		this->disableProcessing = !(msg.cast<PlayerEnable>())->GetEnabled();
	}
	if (!this->IsEnabled() || this->disableProcessing || !this->IsSimulationHost())
	{
		// disabled, don't listen to messages
		PhysicsProperty::HandleMessage(msg);
	}
	else if (msg->CheckId(MoveDirection::Id))
	{
		this->HandleMoveDirection((MoveDirection*)msg.get());
	}
	else if (msg->CheckId(MoveFollow::Id))
	{
		this->HandleMoveFollow((MoveFollow*)msg.get());
	}
	else if (msg->CheckId(MoveGoto::Id))
	{
		this->HandleMoveGoto((MoveGoto*)msg.get());
	}
	else if (msg->CheckId(MoveStop::Id))
	{
		this->Stop();
	}
	else if (msg->CheckId(SetTransform::Id))
	{
		this->HandleSetTransform((SetTransform*)msg.get());
	}
	else if (msg->CheckId(MoveTurn::Id))
	{
		this->HandleMoveTurn((MoveTurn*)msg.get());
	}
	else if (msg->CheckId(MoveRotate::Id))
	{
		this->HandleMoveRotate((MoveRotate*)msg.get());
	}
	else if (msg->CheckId(GetPhysicsObject::Id))
	{
		((PhysicsFeature::GetPhysicsObject*)msg.get())->SetObject(this->GetPhysicsEntity().upcast<Physics::PhysicsObject>());
		msg->SetHandled(true);
	}
	else if (msg->CheckId(MoveSetVelocity::Id))
	{
		this->GetEntity()->SetFloat(Attr::RelVelocity, ((MoveSetVelocity*)msg.get())->GetRelVelocity());
	}
	else if (msg->CheckId(MoveJump::Id))
	{
		charPhysicsEntity->Jump();
	}
	else if (msg->CheckId(Crouch::Id))
	{
		charPhysicsEntity->SetCrouching(msg.downcast<Crouch>()->GetEnable());
	}
	else if (msg->CheckId(PlayerDeath::Id))
	{
		this->Stop();
		this->disableProcessing = true;
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
        vector physicsEntityVelocity = this->charPhysicsEntity->GetLinearVelocity();

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

     //   if ((posError.length() > 0.001f) || (headingError > 0.001f))
        {
            // construct the new entity matrix
			matrix44 entityMatrix;
			if (this->smoothingEnabled)
			{
				entityMatrix = matrix44::rotationy(this->smoothedHeading.GetState());
				entityMatrix.translate(this->smoothedPosition.GetState());
			}
			else
			{
				entityMatrix = matrix44::rotationy(this->smoothedHeading.GetGoal());
				entityMatrix.translate(this->smoothedPosition.GetGoal());
			}
            // update game entity
            Ptr<UpdateTransform> msg = UpdateTransform::Create();
            msg->SetMatrix(entityMatrix);
            this->GetEntity()->SendSync(msg.upcast<Message>());
            this->GetEntity()->SetFloat4(Attr::VelocityVector, physicsEntityVelocity);
        }

		this->charPhysicsEntity->OnFrameAfter();
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
	this->gotoActive = false;
	this->charPhysicsEntity->SetMotionVector(vector(0.0f, 0.0f, 0.0f));
    this->GetEntity()->SetBool(Attr::Moving, false);
    this->GetEntity()->SetBool(Attr::Following, false);
    this->GetEntity()->SetFloat4(Attr::VelocityVector, vector(0.0f, 0.0f, 0.0f));
    this->gotoPath = 0;
    this->isStuck = false;
    this->unStuckTime = 0.0;
    this->stuckTime = 0.0;
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
	if (desiredVelocity.length() >  msg->GetMaxMovement())
	{
		desiredVelocity = Math::float4::normalize(desiredVelocity) *  msg->GetMaxMovement();
	}
    this->charPhysicsEntity->SetMotionVector(desiredVelocity);
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

	if (float4::equal3_all(dir, float4(0,0,0,0)))
	{
		return;
	}

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

	matrix44 newTransform = charPhysicsEntity->GetTransform();
	newTransform.set_xaxis(vector::cross3(newTransform.get_yaxis(),dir));
	newTransform.set_zaxis(vector::cross3(newTransform.get_xaxis(),newTransform.get_yaxis()));
	this->charPhysicsEntity->SetTransform(newTransform);
}

//------------------------------------------------------------------------------
/**
    Handle a MoveTurn message.
*/
void
ActorPhysicsProperty::HandleMoveRotate(MoveRotate* msg)
{
    n_assert(msg);
    
    float angle = msg->GetAngle();
	charPhysicsEntity->SetTransform(matrix44::multiply(matrix44::rotationy(angle),charPhysicsEntity->GetTransform()));
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
    //this->Stop();

    //make a navigation path from current to target position
    this->gotoDest = msg->GetPosition();

    
    const point& from = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
    const point& to = msg->GetPosition();

    // currently pathfinding is done directly, so no waiting for completed
    /*    if (this->gotoPath.isvalid())
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
    */
    {
       this->gotoPath = Navigation::NavigationServer::Instance()->MakePath(from, to);       
       this->curGotoSegment = 0;
    }
    if(this->gotoPath->CountSegments()>0)
	{    
		this->gotoActive = true;
		this->gotoTargetDist = msg->GetDistance();
		this->gotoTimeStamp = GameTimeSource::Instance()->GetTime();
		this->GetEntity()->SetBool(Attr::Moving, true);

		this->ContinueGoto();
	}
	else
	{
		this->SendStop();	
	}
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
	matrix44 entityMatrix = this->GetEntity()->GetMatrix44(Attr::Transform);
	this->smoothedPosition.Reset(time, 0.001f, this->positionGain, entityMatrix.get_position());

	polar headingAngle(entityMatrix.get_zaxis());
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
	
    point curPos = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
    const Util::Array<point>& gotoSegments = this->gotoPath->GetPoints();
    int numGotoSegments = gotoSegments.Size();
    for (;this->curGotoSegment < numGotoSegments-1; this->curGotoSegment++)
    {
        // skip waypoint, if actor stands between current and next waypoint
        if (this->curGotoSegment > 0)
        {
            point vecLastCur = gotoSegments[this->curGotoSegment] - gotoSegments[this->curGotoSegment-1];
            point vecMeCur = gotoSegments[this->curGotoSegment] - curPos;
            if (point::dot3(vecLastCur,vecMeCur) < 0.0f)
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
        float distMeNext = point(myPos - next).length();
        float distMeCur = point(myPos - cur).length();
        float distCurNext = point(cur - next).length();
        if (distMeNext <= distCurNext || distMeNext < distMeCur)
        {
            continue;
        }
        break;
    }    
}

//------------------------------------------------------------------------------
/**
Function was changed(11.07.07). Now AutoEvade()will be called by HandleMoveDirection()
to avoid replacement of non-static physic objekts. 
*/
void
ActorPhysicsProperty::AutoEvade(float4& targetVec)
{    
    /*
    //Check if path is valid
    if(this->gotoPath.isvalid())
    {
        this->withPath = true;
    }
    
    point curPos = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
    Array<point> gotoSegments;
    vector firstVec;

    //Only do this if we have a valid path
    if (withPath)
    {
        gotoSegments = (Array<point>)this->gotoPath->GetPoints();
        /// --- autoevade ---
        // compute a smoothed target vector out of all the next path nodes within 3 meters but minimum 2 nodes
        targetVec = point(0,0,0);
        firstVec = point(0,0,0);
        int pointIndex;
        for (pointIndex = this->curGotoSegment; 
             pointIndex < gotoSegments.Size() && 
             (point(gotoSegments[pointIndex] - curPos).length() < 3.0f ||
             pointIndex < this->curGotoSegment+1);
             pointIndex++)
        {
            point nextVec = gotoSegments[pointIndex] - curPos;
            nextVec.y() *= 2;
            if (pointIndex == this->curGotoSegment)
            {
                firstVec = nextVec;
            }
            else if(Math::point::dot3(firstVec,nextVec) < -0.25f)
            {
                // if angle between next vector and first vector is too great, 
                // break smoothing to avoid running against walls etc...
                break;
            }
            nextVec.y() = 0.0f;
            nextVec = Math::float4::normalize(nextVec);
            targetVec += nextVec;
        }
        targetVec = Math::float4::normalize(targetVec);        
        firstVec.y() = 0;
        firstVec = Math::float4::normalize(firstVec);        
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
                    if (float4::dot3(contactDir,firstVec) < -0.25f)
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
                    contactDir = float4::normalize(contactDir);
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
    
    //n_assert(this->gotoPath.isvalid());

	// goto path is not completed yet
	//if (!this->gotoPath->IsCompleted()) return;

    /// update the stuck flag
    this->DetectStuck();

    /// skip evaded goto segments
    this->SkipSegments();

    const Array<point>& gotoSegments = this->gotoPath->GetPoints();

    point curPos = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
    point curSegment = gotoSegments[this->curGotoSegment];

    //compute vector to segment
    float4 targetVec(curSegment - curPos);
    targetVec.y() = 0.0f;
    float dist = targetVec.length();
    targetVec=float4::normalize(targetVec);

    //stop if in range and every following segment too!
    float distToFinalTarget = float4(gotoSegments.Back()- curPos).length();
    if (distToFinalTarget < this->gotoTargetDist)
    {
        bool reached = true;
        IndexT i;
        for (i=this->curGotoSegment; i < gotoSegments.Size(); i++)
        {
            if (float4(gotoSegments[i] - gotoSegments.Back()).length() > this->gotoTargetDist)
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
    
    if(this->isStuck)
    {
        this->SendStop();
        return;
    }
    else if (targetVec.length() > TINY)
    {
		Ptr<MoveTurn> msg2 = MoveTurn::Create();
		msg2->SetDirection(targetVec);
		msg2->SetCameraRelative(false);
		__SendSync(this->entity, msg2);
        // just continue to go towards current segment position
        Ptr<MoveDirection> msg = MoveDirection::Create();
        msg->SetDirection(targetVec);
        msg->SetMaxMovement(dist);        
		__SendSync(this->entity, msg);		
    }
}

//------------------------------------------------------------------------------
/**
    Continue the current Follow action.
*/
void
ActorPhysicsProperty::ContinueFollow()
{    
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
        const vector targetPos = targetEntity->GetMatrix44(Attr::Transform).get_position();
        const vector curPos = this->GetEntity()->GetMatrix44(Attr::Transform).get_position();
        vector targetVec = targetPos - curPos;
        targetVec.y() = 0.0f; // ignore vertical dimension
        float targetDist = targetVec.length();

        // if we are close enough to our target, stop, and always face our target
        if (targetDist < this->followTargetDist)
        {
           // stop and look at our target
           this->SendStop();
           //this->charPhysicsEntity->SetDesiredLookat(targetVec);
        }
        // continue following target position if not stopped.
        else if ((this->gotoTimeStamp + 1.0) < GameTimeSource::Instance()->GetTime())
        {
            // check if new goto calculation is necessary
            if (this->gotoPath.isvalid())
            {		        
                // if destination is the same there is no need to find a new path
                float dist = (this->gotoPath->GetTarget() - targetPos).length();
                if (dist < this->gotoTargetDist)
                {
                    return;
                }

				// continue moving towards our target entity
				Ptr<MoveGoto> moveGoto = MoveGoto::Create();
				moveGoto->SetPosition(targetPos);
				moveGoto->SetDistance(this->followTargetDist);
				this->HandleMoveGoto(moveGoto);
                return;		        
            }
			// we have no path, create new one
			Ptr<MoveGoto> moveGoto = MoveGoto::Create();
			moveGoto->SetPosition(targetPos);
			moveGoto->SetDistance(this->followTargetDist);
			this->HandleMoveGoto(moveGoto);
        }
    }    
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Physics::Character>&
ActorPhysicsProperty::GetPhysicsEntity() const
{
	return this->charPhysicsEntity;
}

//------------------------------------------------------------------------------
/**
    Render a debug visualization of the current 3d navigation path.
*/
void
ActorPhysicsProperty::OnRenderDebug()
{
    
    if (this->gotoPath.isvalid())
    {
        const Array<point>& points = this->gotoPath->GetPoints();
        int i;
        for (i = 0; i < points.Size(); i++)
        {
            vector p(points[i]);
            matrix44 m;
            m.scale(vector(0.3f, 0.3f, 0.3f));
            m.translate(p + vector(0.0f, 0.1f, 0.0f));
            float4 color(1.0f, 0.0f, 0.0f, 0.5f);
            if (i == this->curGotoSegment) color.set(1.0f, 1.0f, 0.0f, 0.5f);
            else if (i < this->curGotoSegment) color.set(0.0f, 1.0f, 0.0f, 0.1f);
            Debug::DebugShapeRenderer::Instance()->DrawBox(m,color);            
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::OnLoseActivity()
{
    if (this->IsEnabled())
    {
    	this->charPhysicsEntity->SetMotionVector(vector(0.0f, 0.0f, 0.0f));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ActorPhysicsProperty::DetectStuck()
{
    if (!this->isStuck)
    {
        const float4& curPos = this->charPhysicsEntity->GetTransform().get_position();
        float4 diff = this->lastUpdatePos - curPos;
        if (0.0001f > diff.lengthsq3())
        {
            Timing::Time frameTime = BaseGameFeature::GameTimeSource::Instance()->GetFrameTime();
            Timing::Time newStuckTime = this->stuckTime + frameTime;

            // set stuck flag after 0.5 seconds
            if (newStuckTime > 0.5)
            {
                this->isStuck = true;
                this->unStuckTime = 0;
            }
            else
            {
                this->stuckTime = newStuckTime;
            }
        }
        else
        {
            this->stuckTime = 0;
        }

        this->lastUpdatePos = curPos;
    }
    else
    {
        if (this->unStuckTime > 0.5f)
        {
            this->isStuck = false;
            this->stuckTime = 0;
        }
        else
        {
            Timing::Time frameTime = BaseGameFeature::GameTimeSource::Instance()->GetFrameTime();
            this->unStuckTime += frameTime;
        }
    }
}
} // namespace Properties
