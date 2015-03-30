#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::UiInputHandler
    
    Handles user input sent to the UI system
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "input/inputhandler.h"

//------------------------------------------------------------------------------
namespace UI
{
class UiInputHandler : public Input::InputHandler
{
    __DeclareClass(UiInputHandler);
public:
    /// constructor
    UiInputHandler();
    /// destructor
    virtual ~UiInputHandler();
        
    /// capture input to this event handler
    virtual void BeginCapture();
    /// end input capturing to this event handler
    virtual void EndCapture();

protected:
    friend class Base::InputServerBase;
    
    /// called on InputServer::BeginFrame()
    virtual void OnBeginFrame();
    /// called on InputServer::EndFrame();
    virtual void OnEndFrame();
    /// called when input handler obtains capture
    //virtual void OnObtainCapture();
    /// called when input handler looses capture
    //virtual void OnReleaseCapture();
    /// called when an input event should be processed
	virtual bool OnEvent(const Input::InputEvent& inputEvent);
    /// called when the handler should reset itself
    virtual void OnReset();

private:
	Util::Array<Input::InputEvent> inputEvents;	
};

} // namespace UI
//------------------------------------------------------------------------------

