//------------------------------------------------------------------------------
//  physicsfeature/properties/interactproperty.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsfeature/properties/interactproperty.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "scriptingfeature/properties/scriptingproperty.h"
#include "input/inputserver.h"
#include "input/keyboard.h"


namespace PhysicsFeature
{
	__ImplementClass(PhysicsFeature::InteractProperty, 'ITPR', ScriptingFeature::ScriptingProperty);

using namespace Scripting;
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
InteractProperty::InteractProperty() :
	onMouseButton(false),
	onMouseEnter(false),
	onMouseLeave(false)	
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
InteractProperty::~InteractProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
InteractProperty::SetupAcceptedMessages()
{
	ScriptingProperty::SetupAcceptedMessages();
	this->RegisterMessage(GraphicsFeature::MouseButtonEvent::Id);
	this->RegisterMessage(GraphicsFeature::MouseEnter::Id);
	this->RegisterMessage(GraphicsFeature::MouseLeave::Id);
}

//------------------------------------------------------------------------------
/** 

*/
void
InteractProperty::OnDeactivate()
{
    Property::OnDeactivate();
	if (this->onMouseEnter)
	{
		ScriptServer::Instance()->UnregisterFunction("onmouseenter_table",this->entity->GetUniqueId());
	}
	if (this->onMouseLeave)
	{
		ScriptServer::Instance()->UnregisterFunction("onmouseleave_table",this->entity->GetUniqueId());
	}
	if (this->onMouseButton)
	{
		ScriptServer::Instance()->UnregisterFunction("onmousebutton_table", this->entity->GetUniqueId());
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InteractProperty::OnActivate()
{
	ScriptingProperty::OnActivate();
	this->DetectInteractFunctions();
}

//------------------------------------------------------------------------------
/**
*/
void
InteractProperty::OnStart()
{
	ScriptingProperty::OnStart();		
	//FIXME register with envquerymanager
	BaseGameFeature::EnvQueryManager::Instance()->SetMouseMessages(true);
}

//------------------------------------------------------------------------------
/**
*/
void
InteractProperty::DetectInteractFunctions()
{
    ScriptServer* server = ScriptServer::Instance();
	if (this->entityScript.IsEmpty())
	{
		return;
	}
	this->onMouseEnter = server->ScriptHasFunction(this->entityScript, "onmouseenter");
	SCRIPT_ERROR_CHECK("RegisterFunction", "onmouseenter");
	if (this->onMouseEnter)
	{
		server->RegisterFunction(this->entityScript, "onmouseenter", "onmouseenter_table", this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}

	this->onMouseLeave = server->ScriptHasFunction(this->entityScript, "onmouseleave");
	SCRIPT_ERROR_CHECK("RegisterFunction", "onmouseleave");
	if (this->onMouseLeave)
	{
		server->RegisterFunction(this->entityScript, "onmouseleave", "onmouseleave_table", this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}	
	
	this->onMouseButton = server->ScriptHasFunction(this->entityScript, "onmousebutton");
	SCRIPT_ERROR_CHECK("RegisterFunction", "onmousebutton");
	if (this->onMouseButton)
	{
		server->RegisterFunction(this->entityScript, "onmousebutton", "onmousebutton_table", this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}
}

//------------------------------------------------------------------------------
/**
*/
void
InteractProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	if (msg->CheckId(GraphicsFeature::MouseEnter::Id))
	{
		if (this->onMouseEnter)
		{			
			ScriptServer::Instance()->CallFunction("onmouseenter_table", this->entity->GetUniqueId());
			SCRIPT_ERROR_CHECK("HandleMessage", "MouseEnter");

		}
	}
	else if (msg->CheckId(GraphicsFeature::MouseLeave::Id))
	{
		if (this->onMouseLeave)
		{
			ScriptServer::Instance()->CallFunction("onmouseleave_table", this->entity->GetUniqueId());
			SCRIPT_ERROR_CHECK("HandleMessage", "MouseLeave");
		}
	}
	else if (msg->CheckId(GraphicsFeature::MouseButtonEvent::Id))
	{
		if (this->onMouseButton)
		{
			Ptr<GraphicsFeature::MouseButtonEvent> message = msg.downcast<GraphicsFeature::MouseButtonEvent>();
			Util::Array<Util::Variant> args;
			args.Append(Input::MouseButton::ToString(message->GetButton()));
			args.Append(message->GetDown());
			ScriptServer::Instance()->CallFunction("onmousebutton_table", this->entity->GetUniqueId(), args);
		}
	}
}

}; // namespace PhysicsFeature
