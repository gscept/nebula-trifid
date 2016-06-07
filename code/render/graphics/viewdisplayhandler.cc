//------------------------------------------------------------------------------
// viewdisplayhandler.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "viewdisplayhandler.h"
#include "graphics/view.h"

namespace Graphics
{

__ImplementClass(Graphics::ViewDisplayHandler, 'VWDH', CoreGraphics::DisplayEventHandler);
//------------------------------------------------------------------------------
/**
*/
ViewDisplayHandler::ViewDisplayHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ViewDisplayHandler::~ViewDisplayHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ViewDisplayHandler::HandleEvent(const CoreGraphics::DisplayEvent& event)
{
	switch (event.GetEventCode())
	{
	case CoreGraphics::DisplayEvent::WindowResized:
		this->view->OnWindowResized(event.GetWindowId());
		return true;
	default:
		return DisplayEventHandler::HandleEvent(event);
	}
	return false;
}

} // namespace Graphics