//------------------------------------------------------------------------------
//  properties/chasecameraproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/chasecameraproperty.h"
#include "basegametiming/inputtimesource.h"
#include "managers/focusmanager.h"
#include "game/entity.h"
#include "graphics/stage.h"
#include "graphics/cameraentity.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "math/quaternion.h"
#include "io/console.h"
#include "physics/physicsserver.h"
#include "physicsfeature/physicsprotocol.h"

using namespace Math;

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::ChaseCameraProperty, 'PCHC', GraphicsFeature::CameraProperty);

using namespace Game;
using namespace Messaging;
using namespace BaseGameFeature;
using namespace Math;
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
ChaseCameraProperty::ChaseCameraProperty() :
    cameraDistance(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ChaseCameraProperty::~ChaseCameraProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This adds the default attributes to the property.
*/
void
ChaseCameraProperty::SetupDefaultAttributes()
{
    CameraProperty::SetupDefaultAttributes();    
}

//------------------------------------------------------------------------------
/**
    This method is called once when the property is attached to the entity.
*/
void
ChaseCameraProperty::OnActivate()
{
    CameraProperty::OnActivate();

    // setup the collide exclude set
#if 0
    this->collideExcludeSet.Clear();
    this->collideExcludeSet.AddMaterialType(Physics::MaterialTable::StringToMaterialType("Probe"));
    this->collideExcludeSet.AddMaterialType(Physics::MaterialTable::StringToMaterialType("Character"));
#endif
    this->collideExcludeSet.SetCollideBits(Physics::Characters | Physics::SensorTrigger);
    // reset the camera
    this->HandleCameraReset();
}

//------------------------------------------------------------------------------
/**
    This method is called when the entity obtains the camera focus.
*/
void
ChaseCameraProperty::OnObtainCameraFocus()
{
    // initialize the feedback loops with the current camera values so
    // that we get a smooth interpolation to the new position    
    const matrix44 m = this->entity->GetMatrix44(Attr::Transform);

    Timing::Time time = InputTimeSource::Instance()->GetTime();
    this->cameraPos.Reset(time, 0.0001f, this->entity->GetFloat(Attr::CameraLinearGain), m.get_position() - (m.get_zaxis() * 5.0f));
    this->cameraLookat.Reset(time, 0.0001f, this->entity->GetFloat(Attr::CameraAngularGain), m.get_position() - (m.get_zaxis() * 10.0f));
    this->UpdateCamera(false);
    CameraProperty::OnObtainCameraFocus();

	// add the physicsobject-id to the exclude set if the entity has one
	if (this->entity->AcceptsMessage(PhysicsFeature::GetPhysicsObject::Id))
	{
		Ptr<PhysicsFeature::GetPhysicsObject> message = PhysicsFeature::GetPhysicsObject::Create();
		this->entity->SendSync(message.cast<Messaging::Message>());

		uint id = message->GetObject()->GetUniqueId();

		this->collideExcludeSet.AddRigidBodyId(id);
	}

}

//------------------------------------------------------------------------------
/**
    This method is called before the camera is "rendered".
*/
void
ChaseCameraProperty::OnRender()
{
    // only do something if we have the camera focus
    if (FocusManager::Instance()->GetCameraFocusEntity() == this->entity)
    {
        // update the camera
        this->UpdateCamera(true);
		this->UpdateAudioListenerPosition();       		
		PostEffect::PostEffectManager::Instance()->SetPointOfInterest(this->entity->GetMatrix44(Attr::Transform).get_position());
		// this is an odd effect, it depends on the position of the (rotating) camera, should be player?
		//PostEffect::PostEffectManager::Instance()->SetPointOfInterest(this->cameraPos.GetState());
    }
   
	// important: call parent class
	//FIXME: clashes with transform attribute, making whole cam ineffective
    //CameraProperty::OnRender();
}

//------------------------------------------------------------------------------
/**
*/
void
ChaseCameraProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(CameraOrbit::Id);
    this->RegisterMessage(CameraReset::Id);
    this->RegisterMessage(CameraDistance::Id);
    CameraProperty::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
    This method handles pending messages for the property. It is called
    by the OnRender() method.
*/
void
ChaseCameraProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(CameraOrbit::Id))
    {
        CameraOrbit* cameraOrbitMsg = msg.downcast<CameraOrbit>();
        this->HandleCameraOrbit(cameraOrbitMsg->GetHorizontalRotation(), cameraOrbitMsg->GetVerticalRotation());
    }
    else if (msg->CheckId(CameraReset::Id))
    {
        this->HandleCameraReset();
    }
    else if (msg->CheckId(CameraDistance::Id))
    {
        CameraDistance* cameraDistanceMsg = msg.downcast<CameraDistance>();
        this->HandleCameraDistanceChange(cameraDistanceMsg->GetRelativeDistanceChange());
    }
    CameraProperty::HandleMessage(msg);
}

//------------------------------------------------------------------------------
/**
    Handle a camera distance change.
*/
void
ChaseCameraProperty::HandleCameraDistanceChange(float d)
{
    float minDist  = this->entity->GetFloat(Attr::CameraMinDistance);
    float maxDist  = this->entity->GetFloat(Attr::CameraMaxDistance);
    float zoomStep = this->entity->GetFloat(Attr::CameraDistanceStep);
    this->cameraDistance = n_clamp(this->cameraDistance + (d * zoomStep), minDist, maxDist);
}

//------------------------------------------------------------------------------
/**
    Handle a camera reset.
*/
void
ChaseCameraProperty::HandleCameraReset()
{
    float curTheta = this->entity->GetFloat(Attr::CameraDefaultTheta);     
    float curRho = this->entity->GetFloat(Attr::CameraDefaultRho);     
    const matrix44 m = this->entity->GetMatrix44(Attr::Transform);
    this->cameraAngles.set(m.get_zaxis());
    this->cameraAngles.theta = curTheta;
    this->cameraAngles.rho = curRho;
    this->cameraDistance = this->entity->GetFloat(Attr::CameraDistance);
}

//------------------------------------------------------------------------------
/**
    Handle a camera orbit.
*/
void
ChaseCameraProperty::HandleCameraOrbit(float dRho, float dTheta)
{
    float angularVelocity = this->entity->GetFloat(Attr::CameraAngularVelocity);
    float lowStop = n_deg2rad(this->entity->GetFloat(Attr::CameraLowStop));
    float hiStop  = n_deg2rad(this->entity->GetFloat(Attr::CameraHighStop));
    
    float frameTime = (float) InputTimeSource::Instance()->GetFrameTime();;
    this->cameraAngles.rho += dRho * angularVelocity * frameTime;
    this->cameraAngles.theta += -dTheta * angularVelocity * frameTime;
    this->cameraAngles.theta = n_clamp(this->cameraAngles.theta, lowStop, hiStop);
}

//------------------------------------------------------------------------------
/**
    Do a ray check between 'from' and 'to' and return a replacement
    point for 'to'.
*/
point
ChaseCameraProperty::DoCollideCheck(const point& from, const point& to)
{		
	static const vector up(0.0f, 1.0f, 0.0f);
    matrix44 m = matrix44::lookatrh(from, to, up);
    float outContactDist = 1.0f;
  	
    Physics::PhysicsServer::Instance()->GetScene()->RayBundleCheck(from, to-from, up, m.get_xaxis(), 0.25f, this->collideExcludeSet,outContactDist);
	
    vector vec = vector::normalize(to - from);    
    point newTo = from + vec * outContactDist;
    return newTo;
}

//------------------------------------------------------------------------------
/**
    Update the camera position and orientation from the current
    orbit polar angles.
*/
void
ChaseCameraProperty::UpdateCamera(bool interpolate)
{
    // compute the lookat point in global space
    matrix44 m44 = this->entity->GetMatrix44(Attr::Transform);    
    const point pos = m44.get_position();
    matrix44 rot = m44;
    rot.set_position(point(0,0,0));
    vector lookatPoint = (pos + matrix44::transform(this->entity->GetFloat4(Attr::CameraOffset), rot));    

    matrix44 m = matrix44::translation(0, 0, this->cameraDistance);
    m = matrix44::multiply(m, matrix44::rotationx(this->cameraAngles.theta));
    m = matrix44::multiply(m, matrix44::rotationy(this->cameraAngles.rho));
    float4 newpos = m.get_position();    

    newpos += lookatPoint;    
	newpos = this->DoCollideCheck(lookatPoint, newpos);
    if (!interpolate)
    {
        Timing::Time time = InputTimeSource::Instance()->GetTime();
        this->cameraPos.Reset(time, 0.0001f, this->entity->GetFloat(Attr::CameraLinearGain), newpos);
        this->cameraLookat.Reset(time, 0.0001f, this->entity->GetFloat(Attr::CameraAngularGain), lookatPoint);
    }

    // feed and update the feedback loops
    Timing::Time time = InputTimeSource::Instance()->GetTime();
    this->cameraPos.SetGoal(newpos);
    this->cameraLookat.SetGoal(lookatPoint);
    this->cameraPos.Update(time);
    this->cameraLookat.Update(time);

    // construct the new camera matrix
    matrix44 cameraMatrix = matrix44::lookatrh(this->cameraPos.GetState(), this->cameraLookat.GetState(), vector::upvec());

	// do not commit this
	if ( false)
	{
		matrix44 m = matrix44::translation(0.0f, 0.0f, this->cameraDistance);
		m = matrix44::multiply(m, matrix44::rotationx(this->cameraAngles.theta - (N_PI * 0.5f)));
		m = matrix44::multiply(m, matrix44::rotationy(this->cameraAngles.rho));
		m = matrix44::multiply(m, matrix44::translation(this->cameraLookat.GetState()));

		cameraMatrix = m;
	}

    // update the graphics subsystem camera
    this->cameraEntity->SetTransform(cameraMatrix);
}

} // namespace GraphicsFeature