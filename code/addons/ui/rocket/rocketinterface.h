#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketInterface
    
    Implements a Rocket system interface
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <Rocket/Core.h>
namespace LibRocket
{
class RocketInterface : public Rocket::Core::SystemInterface
{
public:
	/// constructor
	RocketInterface();
	/// destructor
	virtual ~RocketInterface();

	/// gets the time elapsed since the start of the application
	virtual float GetElapsedTime();

	/// activate keyboard
	void ActivateKeyboard();
	/// deactivate keyboard
	void DeactivateKeyboard();
	
#ifdef NEBULA3_DEBUG
	/// logs specific message
	bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message);
#endif
}; 
} // namespace Rocket
//------------------------------------------------------------------------------