//------------------------------------------------------------------------------
//  uieventhandler.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "uieventhandler.h"
#include "uievent.h"
#include "uilayout.h"
#include "uiserver.h"
#include "scripting/scriptserver.h"

using namespace Scripting;

namespace UI
{
__ImplementClass(UiEventHandler, 'UIH', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
UiEventHandler::UiEventHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
UiEventHandler::~UiEventHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
UiEventHandler::HandleEvent(const UiEvent& e)
{
	if (ScriptServer::HasInstance())
	{
		ScriptServer* scriptServer = ScriptServer::Instance();
		const Util::String& layoutId = e.GetLayout();
		const Util::String& event = e.GetEventName();

		ScriptServer::Instance()->Eval(e.GetEventScript());
		if (ScriptServer::Instance()->HasError())
		{
			n_printf("Error evaluating event script %s: %s\n", event.AsCharPtr(),ScriptServer::Instance()->GetError().AsCharPtr()); 
		}
	}
}

}; // namespace UI