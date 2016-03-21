#pragma once
//------------------------------------------------------------------------------
/**
	@class CoreGraphics::Event

	An event is a callback given to a window

	(C) 2012-2016 Individual contributors, see AUTHORS file
*/

#if __WIN32__
#include "events/win32/win32event.h"
typedef Win32::Win32Event Event;
#elif linux
#include "events/posix/posixevent.h"
typedef Posix::PosixEvent Event;
#else
#error "Event not implemented on this platform!"
#endif
//------------------------------------------------------------------------------