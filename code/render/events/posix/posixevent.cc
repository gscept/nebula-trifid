//------------------------------------------------------------------------------
//  posixevent.cc
//  (C) 2012-2013 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "events/posix/posixevent.h"

namespace Posix
{

//------------------------------------------------------------------------------
/**
*/
PosixEvent::PosixEvent()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PosixEvent::~PosixEvent()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Posix::PosixEvent
PosixEvent::FromEvent( XEvent* event )
{
    PosixEvent ret;
    ret.event = event;
    return ret;
}

} // namespace Posix