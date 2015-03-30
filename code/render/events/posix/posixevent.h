#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::PosixEvent

    Encapsulates an X11 event

    (C) 2012-2013 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

typedef union _XEvent XEvent;

//------------------------------------------------------------------------------
namespace Posix
{
class PosixEvent
{
public:
    /// constructor
    PosixEvent();
    /// destructor
    virtual ~PosixEvent();

    /// returns XEvent
    XEvent* GetEvent() const;

    /// constructs event from XEvent
    static PosixEvent FromEvent(XEvent* event);
private:
    XEvent* event;
};

//------------------------------------------------------------------------------
/**
*/
inline XEvent*
PosixEvent::GetEvent() const
{
    return this->event;
}


} // namespace Posix
//------------------------------------------------------------------------------