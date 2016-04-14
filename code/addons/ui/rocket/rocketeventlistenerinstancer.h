#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketEventListenerInstancer
    
    Implements a Rocket event listener
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <Rocket/Core.h>
#include <Rocket/Core/EventListenerInstancer.h>
#include "Rocket/Core/Element.h"
namespace LibRocket
{
class RocketEventListenerInstancer : public Rocket::Core::EventListenerInstancer
{
public:
	/// constructor
	RocketEventListenerInstancer();
	/// destructor
	virtual ~RocketEventListenerInstancer();

	
	/// create instance of event
	virtual Rocket::Core::EventListener* InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element);

	/// releases instancer
	virtual void Release();
	
}; 
} // namespace Rocket
//------------------------------------------------------------------------------