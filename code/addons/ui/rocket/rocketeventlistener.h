#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketEventListener
    
    Implements a Rocket event listener.
    
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <Rocket/Core.h>
#include <Rocket/Core/EventListener.h>
#include "util/string.h"
namespace LibRocket
{
class RocketEventListener : public Rocket::Core::EventListener
{
public:
	/// constructor
	RocketEventListener(const Rocket::Core::String & value);
	/// destructor
	virtual ~RocketEventListener();

	/// Process the incoming Event
	virtual void ProcessEvent(Rocket::Core::Event& event);
protected:
	Util::String layoutId;
	Util::String eventName;
	Util::String eventParameter;
};
}