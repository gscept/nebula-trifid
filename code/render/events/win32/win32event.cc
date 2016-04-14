//------------------------------------------------------------------------------
//  win32event.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "events/win32/win32event.h"

namespace Win32
{

//------------------------------------------------------------------------------
/**
*/
Win32Event::Win32Event()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Win32Event::~Win32Event()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Win32::Win32Event 
Win32Event::FromMSG( const MSG& msg )
{
	Win32Event event;
	event.hwnd = msg.hwnd;
	event.msg = msg.message;
	event.wparam = msg.wParam;
	event.lparam = msg.lParam;
	return event;
}

//------------------------------------------------------------------------------
/**
*/
MSG 
Win32Event::FromEvent( const Win32Event& event )
{
	MSG msg;
	msg.hwnd = event.hwnd;
	msg.message = event.msg;
	msg.lParam = event.lparam;
	msg.wParam = event.wparam;
	return msg;
}

} // namespace Win32