//------------------------------------------------------------------------------
//  uiinputhandler.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ui/uiinputhandler.h"
#include "input/inputserver.h"
#include "graphics/graphicsinterface.h"
#include "uifeatureunit.h"


namespace UI
{
__ImplementClass(UI::UiInputHandler, 'SINP', Input::InputHandler);
using namespace Core;
using namespace Util;
//------------------------------------------------------------------------------
/**
*/
UiInputHandler::UiInputHandler() : handleInput(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
UiInputHandler::~UiInputHandler()
{
   // empty
}

//------------------------------------------------------------------------------
/**
    Begin capturing input to this input handler. This method must be
    overriden in a subclass, the derived method must call 
    ObtainMouseCapture(), ObtainKeyboardCapture(), or both, depending
    on what type input events you want to capture. An input handler
    which captures input gets all input events of the given type exclusively.
*/
void
UiInputHandler::BeginCapture()
{
	Input::InputServer::Instance()->ObtainMouseCapture(this);
	Input::InputServer::Instance()->ObtainKeyboardCapture(this);
}

//------------------------------------------------------------------------------
/**
    End capturing input to this input handler. Override this method
    in a subclass and release the captures obtained in BeginCapture().
*/
void
UiInputHandler::EndCapture()
{
	Input::InputServer::Instance()->ReleaseMouseCapture(this);
	Input::InputServer::Instance()->ReleaseKeyboardCapture(this);
}

//------------------------------------------------------------------------------
/**
*/
void
UiInputHandler::OnBeginFrame()
{
	//inputEvents.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
UiInputHandler::OnEndFrame()
{
    //UIServer::Instance()->ProcessInputEvents(this->inputEvents);
}

//------------------------------------------------------------------------------
/**
*/
bool
UiInputHandler::OnEvent(const Input::InputEvent& inputEvent)
{
	if (handleInput == false)
	{
		return false;
	}
    UiServer* uiServer = UiServer::Instance();
    switch (inputEvent.GetType())
    {
#ifndef _DEBUG
        case Input::InputEvent::AppObtainFocus:
        case Input::InputEvent::AppLoseFocus:
#endif
        case Input::InputEvent::Reset:
            this->OnReset();
            break;

		default:
            return uiServer->HandleInput(inputEvent);
			break;
    }        
    return false;
}

//------------------------------------------------------------------------------
/**
    OnReset is called when the app loses or gains focus (amongst other
    occasions). The input handler should reset its internal state
    to prevent keys from sticking down, etc...
*/
void
UiInputHandler::OnReset()
{
    // empty
}

void UiInputHandler::SetHandleInput(bool handleInput)
{
	this->handleInput = handleInput;
}

bool UiInputHandler::GetHandleInput()
{
	return handleInput;
}

} // namespace Input


