//------------------------------------------------------------------------------
//  scriptingtriggerproperty.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "scriptingtriggerproperty.h"
#include "scripting/scriptserver.h"
#include "physicsfeature/physicsprotocol.h"

using namespace Util;
using namespace Scripting;
namespace ScriptingFeature
{
__ImplementClass(ScriptingFeature::ScriptingTriggerProperty, 'SCTP', ScriptingProperty);

//------------------------------------------------------------------------------
/**
*/
ScriptingTriggerProperty::ScriptingTriggerProperty(): hasEnter(false),hasInside(false),hasExit(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ScriptingTriggerProperty::~ScriptingTriggerProperty()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingTriggerProperty::SetupAcceptedMessages()
{
	this->RegisterMessage(PhysicsFeature::EnteredTrigger::Id);
	this->RegisterMessage(PhysicsFeature::InsideTrigger::Id);
	this->RegisterMessage(PhysicsFeature::ExitTrigger::Id);
	ScriptingProperty::SetupAcceptedMessages();
}
//------------------------------------------------------------------------------
/**
*/
void
ScriptingTriggerProperty::OnActivate()
{
	ScriptingProperty::OnActivate();
}

//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. This will check attached scripts
*/
void
ScriptingTriggerProperty::OnDeactivate()
{
    Property::OnDeactivate();
	if (this->hasEnter)
	{
		ScriptServer::Instance()->UnregisterFunction("ontriggerenter_table",this->entity->GetUniqueId());
	}
	if (this->hasInside)
	{
		ScriptServer::Instance()->UnregisterFunction("ontriggerinside_table",this->entity->GetUniqueId());
	}
	if (this->hasExit)
	{
		ScriptServer::Instance()->UnregisterFunction("ontriggerexit_table",this->entity->GetUniqueId());
	}
	
}
//------------------------------------------------------------------------------
/**
	check if entity script contains functions for specific events and add them to the
	table in the lua server
*/
void
ScriptingTriggerProperty::DetectDefaultFunctions()
{
	this->hasEnter = ScriptServer::Instance()->ScriptHasFunction(this->entityScript,"onenter");	
	SCRIPT_ERROR_CHECK("RegisterFunction", "onenter");
	if (this->hasEnter)
	{
		ScriptServer::Instance()->RegisterFunction(this->entityScript,"onenter","ontriggerenter_table",this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}
	this->hasInside = ScriptServer::Instance()->ScriptHasFunction(this->entityScript,"oninside");	
	SCRIPT_ERROR_CHECK("RegisterFunction", "oninside");
	if (this->hasInside)
	{
		ScriptServer::Instance()->RegisterFunction(this->entityScript,"oninside","ontriggerinside_table",this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}
	this->hasExit = ScriptServer::Instance()->ScriptHasFunction(this->entityScript,"onexit");	
	SCRIPT_ERROR_CHECK("RegisterFunction", "onexit");
	if (this->hasExit)
	{
		ScriptServer::Instance()->RegisterFunction(this->entityScript,"onexit","ontriggerexit_table",this->entity->GetUniqueId(), this->entity->GetString(Attr::Id));
	}	
	ScriptingProperty::DetectDefaultFunctions();
}

//------------------------------------------------------------------------------
/**
*/
void
ScriptingTriggerProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
	if(msg->CheckId(PhysicsFeature::EnteredTrigger::Id))
	{
		if(this->hasEnter)
		{
			Ptr<PhysicsFeature::EnteredTrigger> emsg = msg.cast<PhysicsFeature::EnteredTrigger>();
			ScriptServer::Instance()->CallFunction("ontriggerenter_table",this->entity->GetUniqueId(),emsg->GetEntity()->GetUniqueId());
			SCRIPT_ERROR_CHECK("HandleMessage", "ontriggerenter");
		}
	}else if(msg->CheckId(PhysicsFeature::InsideTrigger::Id))
	{
		if(this->hasInside)
		{
			Ptr<PhysicsFeature::InsideTrigger> emsg = msg.cast<PhysicsFeature::InsideTrigger>();
			ScriptServer::Instance()->CallFunction("ontriggerinside_table",this->entity->GetUniqueId(),emsg->GetEntity()->GetUniqueId());
			SCRIPT_ERROR_CHECK("HandleMessage", "ontriggerinside");
		}
	}else if(msg->CheckId(PhysicsFeature::ExitTrigger::Id))
	{
		if(this->hasExit)
		{
			Ptr<PhysicsFeature::ExitTrigger> emsg = msg.cast<PhysicsFeature::ExitTrigger>();
			ScriptServer::Instance()->CallFunction("ontriggerexit_table",this->entity->GetUniqueId(),emsg->GetEntity()->GetUniqueId());
			SCRIPT_ERROR_CHECK("HandleMessage", "ontriggerexit");
		}
	}else
	{
		ScriptingProperty::HandleMessage(msg);
	}
}
} // namespace ScriptingFeature