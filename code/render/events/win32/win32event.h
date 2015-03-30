#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::Win32Event
    
    Encapsulates a win32 event
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Win32
{
class Win32Event
{
public:
	/// constructor
	Win32Event();
	/// destructor
	virtual ~Win32Event();

	/// returns hwnd
	const HWND& GetHWND() const;
	/// returns message
	const UINT& GetMessage() const;
	/// returns wparam
	const WPARAM& GetWParam() const;
	/// returns lparam
	const LPARAM& GetLParam() const;

	/// constructs win32 event from MSG class
	static Win32Event FromMSG(const MSG& msg);
	/// constructs MSG from Win32Event
	static MSG FromEvent(const Win32Event& event);

private:
	HWND hwnd;
	UINT msg;
	WPARAM wparam;
	LPARAM lparam;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const HWND& 
Win32Event::GetHWND() const
{
	return this->hwnd;
}

//------------------------------------------------------------------------------
/**
*/
inline const UINT& 
Win32Event::GetMessage() const
{
	return this->msg;
}

//------------------------------------------------------------------------------
/**
*/
inline const WPARAM& 
Win32Event::GetWParam() const
{
	return this->wparam;
}

//------------------------------------------------------------------------------
/**
*/
inline const LPARAM& 
Win32Event::GetLParam() const
{
	return this->lparam;
}

} // namespace CoreGraphics
//------------------------------------------------------------------------------