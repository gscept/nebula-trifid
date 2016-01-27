//------------------------------------------------------------------------------
//  properties/mayacamerappoperty.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsfeature/properties/mayacameraproperty.h"
#include "managers/focusmanager.h"
#include "graphics/cameraentity.h"
#include "game/entity.h"
#include "basegamefeature/basegameprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/gamepad.h"
#include "input/inputserver.h"
#include "basegametiming/inputtimesource.h"
#include "graphicsfeature/properties/inputproperty.h"
#include "debugrender/debugshaperenderer.h"
#include "graphics/display.h"
#include "coregraphics/displaymode.h"

namespace GraphicsFeature
{

using namespace Input;
using namespace Math;
using namespace BaseGameFeature;

__ImplementClass(GraphicsFeature::MayaCameraProperty, 'MCAM', GraphicsFeature::CameraProperty);

//------------------------------------------------------------------------------
/**
*/
MayaCameraProperty::MayaCameraProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MayaCameraProperty::~MayaCameraProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
MayaCameraProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, Render);
}

//------------------------------------------------------------------------------
/**
*/
void
MayaCameraProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(UpdateTransform::Id);
    this->RegisterMessage(InputFocus::Id);
	CameraProperty::SetupAcceptedMessages();
}
    
//------------------------------------------------------------------------------
/**
*/
void
MayaCameraProperty::OnObtainInputFocus()
{
    this->entity->SetBool(Attr::InputFocus, true);
}

//------------------------------------------------------------------------------
/**
*/
void
MayaCameraProperty::OnLoseInputFocus()
{     
    this->entity->SetBool(Attr::InputFocus, false);
}

//------------------------------------------------------------------------------
/**
*/
void
MayaCameraProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(UpdateTransform::Id))
    {        
		this->cameraEntity->SetTransform(((UpdateTransform*)msg.get())->GetMatrix());
    }
    else if (msg->CheckId(InputFocus::Id))
    {
        InputFocus* focusMsg = msg.downcast<InputFocus>();
        if (focusMsg->GetObtainFocus())
        {
            this->OnObtainInputFocus();
        }
        else
        {
            this->OnLoseInputFocus();
        }
    }
    else
    {
        CameraProperty::HandleMessage(msg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MayaCameraProperty::OnActivate()
{
	CameraProperty::OnActivate();

	// initialize the Maya camera object
	matrix44 m = this->GetEntity()->GetMatrix44(Attr::Transform);

	float4 coi = float4(0,0,0,1);
	float4 pos = float4(10,10,10,1);

	this->entity->SetFloat4(Attr::MayaCameraCenterOfInterest, coi);

	m.set_position(pos);
	this->GetEntity()->SetMatrix44(Attr::Transform, m);

	// setup the camera util object
	this->mayaCameraUtil.Setup(	coi, 
		pos, 
		this->entity->GetFloat4(Attr::MayaCameraDefaultUpVec) );

	// initialize interpolation vars
	this->focusReached		 = true;
	this->eyePositionReached = true;
}


//------------------------------------------------------------------------------
/**
*/
void
MayaCameraProperty::OnRender()
{
    // handle input even if we have only camera focus
    if (FocusManager::Instance()->GetInputFocusEntity() == GetEntity() && this->focusReached && this->eyePositionReached)
    {
        // feed the Maya camera object with input
		InputServer* inputServer = InputServer::Instance();
        const Ptr<Keyboard>& keyboard = inputServer->GetDefaultKeyboard();
        const Ptr<Mouse>& mouse = inputServer->GetDefaultMouse();
        const Ptr<GamePad>& gamePad = inputServer->GetDefaultGamePad(0);

        // standard input handling: manipulate camera
        this->mayaCameraUtil.SetOrbitButton(mouse->ButtonPressed(MouseButton::LeftButton));
        this->mayaCameraUtil.SetPanButton(mouse->ButtonPressed(MouseButton::MiddleButton));
        this->mayaCameraUtil.SetZoomButton(mouse->ButtonPressed(MouseButton::RightButton));
        this->mayaCameraUtil.SetZoomInButton(mouse->WheelForward());
        this->mayaCameraUtil.SetZoomOutButton(mouse->WheelBackward());
		if( keyboard->KeyPressed(Input::Key::LeftMenu) )
		{
			this->mayaCameraUtil.SetMouseMovement(mouse->GetMovement());
		}

        // process gamepad input
        float zoomIn = 0.0f;
        float zoomOut = 0.0f;
        float2 panning(0.0f, 0.0f);
        float2 orbiting(0.0f, 0.0f);
        if (gamePad->IsConnected())
        {
            const float gamePadZoomSpeed = 50.0f;
            const float gamePadOrbitSpeed = 10.0f;
            const float gamePadPanSpeed = 10.0f;
            if (gamePad->ButtonDown(GamePad::AButton))
            {
                this->mayaCameraUtil.Reset();
            }
            float frameTime = (float) InputTimeSource::Instance()->GetFrameTime();
            zoomIn       += gamePad->GetAxisValue(GamePad::RightTriggerAxis) * frameTime * gamePadZoomSpeed;
            zoomOut      += gamePad->GetAxisValue(GamePad::LeftTriggerAxis) * frameTime * gamePadZoomSpeed;
            panning.x()  += gamePad->GetAxisValue(GamePad::RightThumbXAxis) * frameTime * gamePadPanSpeed;
            panning.y()  += gamePad->GetAxisValue(GamePad::RightThumbYAxis) * frameTime * gamePadPanSpeed;
            orbiting.x() += gamePad->GetAxisValue(GamePad::LeftThumbXAxis) * frameTime * gamePadOrbitSpeed;
            orbiting.y() += gamePad->GetAxisValue(GamePad::LeftThumbYAxis) * frameTime * gamePadOrbitSpeed;
        }

        // handle keyboard input
        if (keyboard->KeyDown(Key::Space))
        {
            this->mayaCameraUtil.Reset();
        }
        if (keyboard->KeyPressed(Key::Right))
        {
            panning.x() -= 0.1f;
        }
        if (keyboard->KeyPressed(Key::Left))
        {
            panning.x() += 0.1f;
        }
        if (keyboard->KeyPressed(Key::Up))
        {
            panning.y() += 0.1f;
        }
        if (keyboard->KeyPressed(Key::Down))
        {
            panning.y() -= 0.1f;
        }

		// update panning, orbiting, zooming speeds
		this->mayaCameraUtil.SetPanning(panning);
		this->mayaCameraUtil.SetOrbiting(orbiting);
		this->mayaCameraUtil.SetZoomIn(zoomIn);
		this->mayaCameraUtil.SetZoomOut(zoomOut);

		point coi = this->mayaCameraUtil.GetCenterOfInterest();

		this->entity->SetFloat4(Attr::MayaCameraCenterOfInterest, coi);	
    }

	// update
	this->mayaCameraUtil.Update();

    if (FocusManager::Instance()->GetCameraFocusEntity() == GetEntity())
    {
		// only use the internal matrix if we are not animated
		this->entity->SetMatrix44(Attr::Transform, this->mayaCameraUtil.GetCameraTransform());	
	}

	// interpolate camera to it's focus destination (switch to feedbackloop?)
	if( !this->focusReached )
	{
		Math::float4 currentPosition = this->entity->GetFloat4(Attr::MayaCameraCenterOfInterest);
		Math::float4 lookVector = (float4)focusDestination-currentPosition;
		lookVector.set_w(0.0f);
		Math::scalar length = lookVector.length();
		if( length < 0.01 )
		{
			this->focusReached = true;
			currentPosition = focusDestination;
		}
		else
		{
			float updateTime = (float)InputTimeSource::Instance()->GetFrameTime()*20.0f;
			currentPosition.set_x( currentPosition.x() + (focusDestination.x()-currentPosition.x())*updateTime );
			currentPosition.set_y( currentPosition.y() + (focusDestination.y()-currentPosition.y())*updateTime );
			currentPosition.set_z( currentPosition.z() + (focusDestination.z()-currentPosition.z())*updateTime );
		}
		this->entity->SetFloat4(Attr::MayaCameraCenterOfInterest, currentPosition);
		this->mayaCameraUtil.SetCenterOfInterest(currentPosition);
	}

	// interpolate cameras distance to center of interest (switch to feedbackloop?)
	if( !this->eyePositionReached )
	{
		Math::scalar currentPosition = mayaCameraUtil.GetViewDistance();
		Math::scalar length = abs(currentPosition-this->eyeDestination);
		if( length < 0.01 )
		{
			this->eyePositionReached = true;
			currentPosition = eyeDestination;
		}
		else
		{
			float updateTime = (float)InputTimeSource::Instance()->GetFrameTime()*20.0f;
			currentPosition = currentPosition + (eyeDestination-currentPosition)*updateTime;
		}
		this->mayaCameraUtil.SetViewDistance( currentPosition );
	}

    // important: call parent class at the end to apply any further effects
    CameraProperty::OnRender();
}

//------------------------------------------------------------------------------
/**
*/
void 
MayaCameraProperty::OnDeactivate()
{
    // clear input focus, if we are the input focus object
    if (this->HasFocus())
    {
        FocusManager::Instance()->SetInputFocusEntity(0);
    }
    CameraProperty::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void 
MayaCameraProperty::SetCameraFocus(const Math::point& centerOfInterest, Math::scalar distance)
{
	// Let the old focus be reached first
	if(!eyePositionReached || !focusReached)
		return;

	// Set focus destination
	this->focusDestination = centerOfInterest;
	this->focusReached = false;

	// Check if the distance is valid
	if(distance < 1.0f)
		return;

	// Set looking from destination
	this->eyeDestination = distance;
	this->eyePositionReached = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
MayaCameraProperty::ResetCamera()
{
	// Reset camera util
	this->mayaCameraUtil.Reset();

	// reset interpolation vars
	this->focusReached		 = true;
	this->eyePositionReached = true;
}

}; // namespace Properties