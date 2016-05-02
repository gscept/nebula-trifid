//------------------------------------------------------------------------------
//  properties/inputproperty.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "properties/inputproperty.h"
#include "managers/focusmanager.h"
#include "game/entity.h"
#include "input/inputserver.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/gamepad.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "io/console.h"

namespace GraphicsFeature
{
__ImplementClass(GraphicsFeature::InputProperty, 'INPR', Game::Property);

using namespace Math;
using namespace Game;
using namespace Input;
using namespace BaseGameFeature;

//------------------------------------------------------------------------------
/**
*/
InputProperty::InputProperty():
    lastFrameSendMovement(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
InputProperty::~InputProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::OnActivate()
{
    Property::OnActivate();
    if (this->entity->GetBool(Attr::InputFocus))
    {
        FocusManager::Instance()->SetInputFocusEntity(this->entity);		
    }
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::OnDeactivate()
{
    // clear input focus, if we are the input focus object
    if (this->HasFocus())
    {
        FocusManager::Instance()->SetInputFocusEntity(0);
    }
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(InputFocus::Id);    
    Game::Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg != 0);
    
    if (msg->CheckId(InputFocus::Id))
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
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::OnObtainInputFocus()
{
    this->entity->SetBool(Attr::InputFocus, true);
}

//------------------------------------------------------------------------------
/**
*/
void
InputProperty::OnLoseInputFocus()
{
    this->entity->SetBool(Attr::InputFocus, false);
}

//------------------------------------------------------------------------------
/**
    This method returns true if our entity has the input focus. This 
    implementation makes sure that 2 properties cannot report that they
    have the input focus by accident.
*/
bool
InputProperty::HasFocus() const
{
    return this->entity->GetBool(Attr::InputFocus);
}

//------------------------------------------------------------------------------
/**
    Handle general input.
*/
void
InputProperty::OnBeginFrame()
{
    // only do something if we have the input focus
    if (FocusManager::Instance()->GetInputFocusEntity() == this->entity)
    {
        InputServer* inputServer = InputServer::Instance();
        const Ptr<Keyboard>& kbd = inputServer->GetDefaultKeyboard();
        const Ptr<Mouse>& mouse  = inputServer->GetDefaultMouse();
        const Ptr<GamePad>& gpad = inputServer->GetDefaultGamePad(0);

        if (kbd->KeyDown(Key::Space))
        {
            // send camera reset msg
            Ptr<CameraReset> camReset = CameraReset::Create();
            this->entity->SendSync(camReset.cast<Messaging::Message>());
        }                
        if (mouse->ButtonPressed(MouseButton::RightButton) || this->entity->GetBool(Attr::InputMouseGrab))
        {
            float2 mouseMove = mouse->GetMovement();     

            // create CameraOrbit message
            Ptr<CameraOrbit> msg = CameraOrbit::Create();
            msg->SetHorizontalRotation(mouseMove.x());
            msg->SetVerticalRotation(mouseMove.y());
            this->entity->SendSync(msg.cast<Messaging::Message>());     
			Ptr<MoveTurn> msg2 = MoveTurn::Create();
			msg2->SetDirection(vector(0,0,-1));
			msg2->SetCameraRelative(true);
			this->entity->SendSync(msg2.cast<Messaging::Message>());     

        }
        if (mouse->WheelForward())
        {
            this->OnCameraZoomIn();
        }
        if (mouse->WheelBackward())
        {
            this->OnCameraZoomOut();
        }

        bool movement = false;
        vector moveVec(0.0f, 0.0f, 0.0f);
		if (kbd->KeyPressed(Input::Key::W) || kbd->KeyPressed(Input::Key::Up))
        {
            moveVec += vector(0.0f, 0.0f, -1.0f);
            movement = true;
        }
		if (kbd->KeyPressed(Input::Key::S) || kbd->KeyPressed(Input::Key::Down))
        {
            moveVec += vector(0.0f, 0.0f, 1.0f);
            movement = true;
        }
		if (kbd->KeyPressed(Input::Key::A) || kbd->KeyPressed(Input::Key::Left))
        {
            moveVec += vector(-1.0f, 0.0f, 0.0f);
            movement = true;
        }        
		if (kbd->KeyPressed(Input::Key::D) || kbd->KeyPressed(Input::Key::Right))
		{
			moveVec += vector(1.0f, 0.0f, 0.0f);
			movement = true;
		}
		if (kbd->KeyPressed(Input::Key::Z))
		{
			moveVec += vector(0.0f, -1.0f, 0.0f);
			movement = true;
		}
		if (kbd->KeyPressed(Input::Key::Q))
		{
			moveVec += vector(0.0f, 1.0f, 0.0f);
			movement = true;
		}
		if(kbd->KeyPressed(Input::Key::E))
		{
			Ptr<MoveJump> msg = MoveJump::Create();
			this->entity->SendSync(msg.cast<Messaging::Message>()); 
			this->lastFrameSendMovement = true;
			movement = true;
		}
		if(kbd->KeyPressed(Input::Key::X))
		{
			Ptr<PlayerUse> msg = PlayerUse::Create();			
			this->entity->SendSync(msg.cast<Messaging::Message>()); 						
		}
        if(kbd->KeyDown(Input::Key::C))
        {
            Ptr<Crouch> msg = Crouch::Create();
            msg->SetEnable(true);
            __SendSync(this->entity,msg);
        }
        if(kbd->KeyUp(Input::Key::C))
        {
            Ptr<Crouch> msg = Crouch::Create();
            msg->SetEnable(false);
            __SendSync(this->entity,msg);
        }
        if (gpad->IsConnected())
        {
            if (gpad->ButtonDown(GamePad::StartButton))
            {
                // send camera reset msg
                Ptr<CameraReset> camReset = CameraReset::Create();
                this->entity->SendSync(camReset.cast<Messaging::Message>());
            }                
            if ((gpad->GetAxisValue(GamePad::LeftThumbXAxis) != 0) || (gpad->GetAxisValue(GamePad::LeftThumbYAxis) != 0))
            {
                // create CameraOrbit message
                Ptr<CameraOrbit> msg = CameraOrbit::Create();
                msg->SetHorizontalRotation(gpad->GetAxisValue(GamePad::LeftThumbXAxis) * 0.1f);
                msg->SetVerticalRotation(gpad->GetAxisValue(GamePad::LeftThumbYAxis) * 0.1f);
                this->entity->SendSync(msg.cast<Messaging::Message>());            
            }
            if (gpad->GetAxisValue(GamePad::LeftTriggerAxis) != 0)
            {
                this->OnCameraZoomIn();
            }
            if (gpad->GetAxisValue(GamePad::RightTriggerAxis) != 0)
            {
                this->OnCameraZoomOut();
            }
            if ((gpad->GetAxisValue(GamePad::RightThumbXAxis) != 0) || (gpad->GetAxisValue(GamePad::RightThumbYAxis) != 0))
            {
                moveVec += vector(gpad->GetAxisValue(GamePad::RightThumbXAxis), 0.0f, gpad->GetAxisValue(GamePad::RightThumbYAxis));
                movement = true;
            }       
        }

        // handle stop
        if (movement)
        {
            Ptr<MoveDirection> msg = MoveDirection::Create();
            msg->SetDirection(float4::normalize(moveVec));			
            msg->SetCameraRelative(this->entity->GetBool(Attr::CameraRelativeMove));
            this->entity->SendSync(msg.cast<Messaging::Message>());   

            this->lastFrameSendMovement = true;
        }
        else if (this->lastFrameSendMovement)
        {
            Ptr<MoveStop> msg = MoveStop::Create();
            this->entity->SendSync(msg.cast<Messaging::Message>());

            this->lastFrameSendMovement = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    This sends a CameraDistance message if the user zooms in with the
    mouse wheel.
*/
void
InputProperty::OnCameraZoomIn()
{
	Ptr<CameraDistance> msg = CameraDistance::Create();
    msg->SetRelativeDistanceChange(-1.0f);
	this->entity->SendSync(msg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
    This sends a CameraDistance message if the user zooms out with the
    mouse wheel.
*/
void
InputProperty::OnCameraZoomOut()
{
	Ptr<CameraDistance> msg = CameraDistance::Create();
    msg->SetRelativeDistanceChange(+1.0f);
    this->entity->SendSync(msg.cast<Messaging::Message>());
}

void InputProperty::OnStart()
{
	if (this->entity->GetBool(Attr::InputFocus))
	{		
		// orientate connected entity according to camera 
		Ptr<MoveTurn> msg2 = MoveTurn::Create();
		msg2->SetDirection(vector(0, 0, -1));
		msg2->SetCameraRelative(true);
		this->entity->SendSync(msg2.cast<Messaging::Message>());
	}
}

} // namespace GraphicsFeature
