//------------------------------------------------------------------------------
// viewdisplayhandler.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "viewdisplayhandler.h"
#include "graphics/view.h"
#include "coregraphics/displaydevice.h"
#include "coregraphics/window.h"

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
	{
		IndexT windowId = event.GetWindowId();
		const Ptr<CoreGraphics::Window>& window = CoreGraphics::DisplayDevice::Instance()->GetWindow(windowId);
		const CoreGraphics::DisplayMode& mode = window->GetDisplayMode();
		this->view->OnWindowResized(windowId);
		RenderModules::RTPluginRegistry::Instance()->OnWindowResized(windowId, mode.GetWidth(), mode.GetHeight());
		return true;
	}
	default:
		return DisplayEventHandler::HandleEvent(event);
	}
	return false;
}

} // namespace Graphics