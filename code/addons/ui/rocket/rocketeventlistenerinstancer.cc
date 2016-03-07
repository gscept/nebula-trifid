//------------------------------------------------------------------------------
//  rocketeventlistenerinsancer.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "memory/memory.h"
#include "rocketeventlistenerinstancer.h"
#include "util/array.h"
#include "rocketserver.h"
#include "rocketeventlistener.h"

using namespace Util;
namespace LibRocket
{

//------------------------------------------------------------------------------
/**
*/
RocketEventListenerInstancer::RocketEventListenerInstancer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RocketEventListenerInstancer::~RocketEventListenerInstancer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Rocket::Core::EventListener* 
RocketEventListenerInstancer::InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element)
{	
	Ptr<RocketServer> rocketServer = RocketServer::Instance();

	/// check the additionals first
	for (int i = 0;i < rocketServer->GetEventListenerInstancers().Size();i++)
	{
		Rocket::Core::EventListener* listener;
		listener = rocketServer->GetEventListenerInstancers()[i]->InstanceEventListener(value, element);
		if (listener)
		{
			return listener;
		}
	}

	String script = value.CString();

	Array<String> parts = script.Tokenize(".");

	// only handle our scripts
	if (parts.Size() == 2)
	{		
		return new RocketEventListener(value);				
	}
	else
	{
		return NULL;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketEventListenerInstancer::Release()
{
	// empty
}
} // namespace Rocket