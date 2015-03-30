#pragma once
//------------------------------------------------------------------------------
/**
	@class ScriptingFeature::ScriptingInputHandler
	
	This input handler passes input events to the scripting system.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "input/inputhandler.h"

namespace ScriptingFeature
{
class ScriptingInputHandler : public Input::InputHandler
{
	__DeclareClass(ScriptingInputHandler);
public:
	/// constructor
	ScriptingInputHandler();
	/// destructor
	virtual ~ScriptingInputHandler();

	/// capture input to this event handler
	virtual void BeginCapture();
	/// end input capturing to this event handler
	virtual void EndCapture();

protected:

	/// called when an input event should be processed
	virtual bool OnEvent(const Input::InputEvent& inputEvent);
};
} // namespace ScriptingFeature