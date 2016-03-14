//------------------------------------------------------------------------------
//  navigatedproperty.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "navigatedproperty.h"
#include "game/entity.h"
#include "navigation/navigationattributes.h"
#include "physicsfeature/physicsattr/physicsattributes.h"
#include "crowdmanager.h"
#include "messaging/staticmessagehandler.h"

using namespace Navigation;
using namespace Messaging;


__Handler(NavigatedProperty,UpdateAgentSpeed)
{ 
    CrowdManager::Instance()->UpdateAgentSpeed(obj->entity,msg->GetSpeed());    
}

__Handler(NavigatedProperty,UpdateAgentTarget)
{ 
    CrowdManager::Instance()->UpdateAgentTarget(obj->entity,msg->GetPosition());    
}

__Handler(NavigatedProperty,StopAgent)
{ 
    CrowdManager::Instance()->StopAgent(obj->entity);
}

__Handler(NavigatedProperty,EnableAgent)
{ 
	obj->EnableCrowdManagement(true);
}

__Handler(NavigatedProperty,DisableAgent)
{ 
	obj->EnableCrowdManagement(false);
}

__Dispatcher(NavigatedProperty)
{
    __Handle(NavigatedProperty, UpdateAgentTarget);
    __Handle(NavigatedProperty, UpdateAgentSpeed);    
	__Handle(NavigatedProperty, EnableAgent);    
	__Handle(NavigatedProperty, DisableAgent);
    __Handle(NavigatedProperty, StopAgent);
}

namespace Navigation
{
__ImplementClass(Navigation::NavigatedProperty, 'NGPR', BaseGameFeature::TransformableProperty);

using namespace Game;

//------------------------------------------------------------------------------
/**
*/
NavigatedProperty::NavigatedProperty():
	isRegistered(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
NavigatedProperty::~NavigatedProperty()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
NavigatedProperty::SetupCallbacks()
{    
     
}

//------------------------------------------------------------------------------
/**
*/
void
NavigatedProperty::SetupAcceptedMessages()
{
    this->RegisterMessage(UpdateAgentSpeed::Id);
    this->RegisterMessage(UpdateAgentTarget::Id);    
    this->RegisterMessage(StopAgent::Id);
	this->RegisterMessage(EnableAgent::Id);
	this->RegisterMessage(DisableAgent::Id);
    Game::Property::SetupAcceptedMessages();
}

//------------------------------------------------------------------------------
/**
*/
void
NavigatedProperty::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    __Dispatch(NavigatedProperty,this,msg);  
}


//------------------------------------------------------------------------------
/** 
    Called when property is attached to a game entity. Will register the entity 
    in the crowd manager that handles the updates
*/
void
NavigatedProperty::OnStart()
{
	Property::OnStart();
	this->EnableCrowdManagement(true);
    CrowdManager::Instance()->UpdateAgentTarget(this->entity, this->entity->GetFloat4(Attr::TargetPos));    
}

//------------------------------------------------------------------------------
/** 
    Called when property is going to be removed from its game entity.
    This will remove the entity from the crowd server
*/
void
NavigatedProperty::OnDeactivate()
{
    this->EnableCrowdManagement(false);
    Property::OnDeactivate();
}

//------------------------------------------------------------------------------
/**     
*/
void
NavigatedProperty::EnableCrowdManagement(bool enable)
{
	if(enable != this->isRegistered)
	{
		if(enable)
		{
			Util::String nav = entity->GetString(Attr::AgentNavMesh);
			if(nav.Length())
			{
				CrowdManager::Instance()->RegisterAgent(nav,this->entity);
			}
			else
			{
				CrowdManager::Instance()->RegisterAgent(this->entity);
			}
		}
		else
		{
			CrowdManager::Instance()->DeregisterAgent(this->entity);
		}
	}
	this->isRegistered = enable;
}


}; // namespace Navigation
