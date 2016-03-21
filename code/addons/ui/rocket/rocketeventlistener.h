#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketEventListener
    
    Implements a Rocket event listener.
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <Rocket/Core.h>
#include <Rocket/Core/EventListener.h>
#include "util/string.h"
#include "util/stringatom.h"
namespace LibRocket
{
class RocketEventListener : public Rocket::Core::EventListener
{
public:
	/// constructor
	RocketEventListener(const Rocket::Core::String & value);
	/// destructor
	virtual ~RocketEventListener();

	///
	virtual void OnAttach(Rocket::Core::Element* element);
	
	/// Process the incoming Event
	virtual void ProcessEvent(Rocket::Core::Event& event);
protected:
	Rocket::Core::Element* element;
	Util::StringAtom layoutId;
	Util::String eventName;
	Util::String eventParameter;
};
}