//------------------------------------------------------------------------------
//  physicsfeature/properties/pointnclickinputproperty.cc
//  (C) 2005 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "pointnclickinputproperty.h"
#include "managers/focusmanager.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "basegamefeature/basegameprotocol.h"
#include "managers/focusmanager.h"
#include "managers/envquerymanager.h"
#include "input/inputserver.h"
#include "input/mouse.h"
#include "basegametiming/inputtimesource.h"


namespace PhysicsFeature
{
__ImplementClass(PhysicsFeature::PointNClickInputProperty, 'pcip', GraphicsFeature::InputProperty);

using namespace BaseGameFeature;
using namespace GraphicsFeature;
using namespace Input;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
PointNClickInputProperty::PointNClickInputProperty() :
    moveGotoTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PointNClickInputProperty::~PointNClickInputProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
PointNClickInputProperty::SetupCallbacks()
{
    this->entity->RegisterPropertyCallback(this, BeginFrame);
}

//------------------------------------------------------------------------------
/**
*/
void
PointNClickInputProperty::OnBeginFrame()
{
    // only do something if we have the input focus
    if (FocusManager::Instance()->GetInputFocusEntity() == GetEntity())
    {
        Ptr<Mouse> mouse = InputServer::Instance()->GetDefaultMouse();                
        if (mouse->ButtonDown(MouseButton::LeftButton))
        {
            this->OnLmbDown();
        }
        else if (mouse->ButtonPressed(MouseButton::LeftButton))
        {
            this->OnLmbPressed();
        }
        if (mouse->ButtonPressed(MouseButton::MiddleButton))
        {
            this->OnMmbPressed();
        }
        if (mouse->ButtonDown(MouseButton::RightButton))
        {
            mouse->BeginCapture();
            this->OnRmbDown();
        }
        if (mouse->ButtonUp(MouseButton::RightButton))
        {
            mouse->EndCapture();
        }
        if (mouse->WheelForward())
        {
            this->OnCameraZoomIn();
        }
        if (mouse->WheelBackward())
        {
            this->OnCameraZoomOut();
        }
    }
    InputProperty::OnBeginFrame();
}

//------------------------------------------------------------------------------
/**
    This sends a move goto message to the intersection point with the world.
*/
void
PointNClickInputProperty::SendMoveGoto()
{
    EnvQueryManager* envQueryMgr = EnvQueryManager::Instance();
    if (envQueryMgr->HasMouseIntersection())
    {
        // send a MoveGoto to the mouse intersection point
        const point& mousePos = envQueryMgr->GetMousePos3d();
		Ptr<MoveGoto> msg = MoveGoto::Create();
        msg->SetPosition(mousePos);
           __SendSync(this->entity,msg);

        // record the current time for the HandleLMBPressed() method
        this->moveGotoTime = InputTimeSource::Instance()->GetTime();
    }
}

//------------------------------------------------------------------------------
/**
    Handle the default action. This sends usually a MoveGoto message
    to our entity.
*/
void
PointNClickInputProperty::OnLmbDown()
{
    this->SendMoveGoto();
}

//------------------------------------------------------------------------------
/**
    This handles the case when the left mouse button remains pressed.
    In this case, a MoveGoto message is sent every half a second or so, so
    that the entity follows the mouse.
*/
void
PointNClickInputProperty::OnLmbPressed()
{
    Timing::Time curTime = InputTimeSource::Instance()->GetTime();
    if ((curTime - this->moveGotoTime) > 0.25f)
    {
        this->SendMoveGoto();
    }
}

//------------------------------------------------------------------------------
/**
    Handle the camera orbit movement.
*/
void
PointNClickInputProperty::OnMmbPressed()
{
    // get horizontal and vertical mouse movement
    const float2& mouseMove = InputServer::Instance()->GetDefaultMouse()->GetMovement();    

    // create CameraOrbit message
	Ptr<GraphicsFeature::CameraOrbit> msg = GraphicsFeature::CameraOrbit::Create();
    msg->SetHorizontalRotation(mouseMove.x());
    msg->SetVerticalRotation(mouseMove.y());
    __SendSync(this->entity,msg);
}

//------------------------------------------------------------------------------
/**
    Handle a right mouse button click. This cancels the current movement.
*/
void
PointNClickInputProperty::OnRmbDown()
{
    // create a MoveStop message
	Ptr<MoveStop> msg = MoveStop::Create();
    __SendSync(this->entity,msg);
}

//------------------------------------------------------------------------------
/**
    This sends a CameraDistance message if the user zooms in with the
    mouse wheel.
*/
void
PointNClickInputProperty::OnCameraZoomIn()
{
	Ptr<GraphicsFeature::CameraDistance> msg = GraphicsFeature::CameraDistance::Create();
    msg->SetRelativeDistanceChange(-1.0f);
	__SendSync(this->entity,msg);
}

//------------------------------------------------------------------------------
/**
    This sends a CameraDistance message if the user zooms out with the
    mouse wheel.
*/
void
PointNClickInputProperty::OnCameraZoomOut()
{
	Ptr<GraphicsFeature::CameraDistance> msg = GraphicsFeature::CameraDistance::Create();
    msg->SetRelativeDistanceChange(+1.0f);
    __SendSync(this->entity,msg);
}

//------------------------------------------------------------------------------
/**
*/
void
PointNClickInputProperty::OnActivate()
{
	// enable mouse tracking, otherwise nothing will happen
	EnvQueryManager::Instance()->SetMouseTracking(true);
	InputProperty::OnActivate();
}

}; // namespace Properties