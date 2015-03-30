//------------------------------------------------------------------------------
//  scriptinginputhandler.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptinginputhandler.h"
#include "input/inputserver.h"
#include "managers/focusmanager.h"
#include "properties/scriptingproperty.h"

namespace ScriptingFeature
{
__ImplementClass(ScriptingFeature::ScriptingInputHandler, 'SCIH', Input::InputHandler);

//------------------------------------------------------------------------------
/**
*/
ScriptingInputHandler::ScriptingInputHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ScriptingInputHandler::~ScriptingInputHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingInputHandler::BeginCapture()
{
	Input::InputServer::Instance()->ObtainMouseCapture(this);
	Input::InputServer::Instance()->ObtainKeyboardCapture(this);
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingInputHandler::EndCapture()
{
	Input::InputServer::Instance()->ReleaseMouseCapture(this);
	Input::InputServer::Instance()->ReleaseKeyboardCapture(this);
}

//------------------------------------------------------------------------------
/**
*/
bool
ScriptingInputHandler::OnEvent(const Input::InputEvent& inputEvent)
{
	Ptr<Game::Entity> entity = BaseGameFeature::FocusManager::Instance()->GetInputFocusEntity();
	if (entity.isvalid())
	{
		Ptr<ScriptingFeature::ScriptingProperty> prop = entity->FindProperty(ScriptingProperty::RTTI).cast<ScriptingFeature::ScriptingProperty>();
		if (prop.isvalid() && prop->onInput)
		{
			switch (inputEvent.GetType())
			{
			case Input::InputEvent::KeyDown:
			{
				prop->OnKeyDown(inputEvent.GetKey());
			}
			break;
			case Input::InputEvent::KeyUp:
			{
				prop->OnKeyUp(inputEvent.GetKey());
			}
			break;
			case Input::InputEvent::MouseButtonDown:
			{
				prop->OnMouseDown(inputEvent.GetMouseButton());
			}
			break;

			}
		}
	}
	return false;
}

} // namespace Dynui