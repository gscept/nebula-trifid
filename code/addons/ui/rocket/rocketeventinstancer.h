#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketEventInstancer
    
    Implements a Rocket event listener
    
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <Rocket/Core.h>
#include <Rocket/Core/EventInstancerDefault.h>
namespace LibRocket
{
class RocketEventInstancer : public Rocket::Core::EventInstancerDefault
{
public:
	/// constructor
	RocketEventInstancer();
	/// destructor
	virtual ~RocketEventInstancer();

	
	/// create instance of event
	Rocket::Core::Event* InstanceEvent(Rocket::Core::Element* target, const Rocket::Core::String& name, const Rocket::Core::Dictionary& parameters, bool interruptible);
	/// release event
	void ReleaseEvent(Rocket::Core::Event* event);
	/// releases instancer
	void Release();
	
}; 
} // namespace Rocket
//------------------------------------------------------------------------------