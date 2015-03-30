//------------------------------------------------------------------------------
//  rocketinterface.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "memory/memory.h"
#include "rocketinterface.h"
#include "framesync/framesynctimer.h"

namespace LibRocket
{

//------------------------------------------------------------------------------
/**
*/
RocketInterface::RocketInterface()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RocketInterface::~RocketInterface()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
float 
RocketInterface::GetElapsedTime()
{
	return (float)FrameSync::FrameSyncTimer::Instance()->GetTime();
}

#ifdef NEBULA3_DEBUG
//------------------------------------------------------------------------------
/**
*/
bool 
RocketInterface::LogMessage( Rocket::Core::Log::Type type, const Rocket::Core::String& message )
{
	switch (type)
	{
	case Rocket::Core::Log::LT_ASSERT:
	case Rocket::Core::Log::LT_ERROR:
		n_warning("RocketInterface error: %s\n", message.CString());
		break;
	case Rocket::Core::Log::LT_WARNING:
		n_warning("RocketInterface warning: %s\n", message.CString());
		break;
	case Rocket::Core::Log::LT_ALWAYS:
	default:
		n_printf("RocketInterface message: %s\n", message.CString());
		break;		
	}

	// always return false
	return false;
}
#endif

//------------------------------------------------------------------------------
/**
*/
void 
RocketInterface::ActivateKeyboard()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
RocketInterface::DeactivateKeyboard()
{
	// empty
}

} // namespace Rocket