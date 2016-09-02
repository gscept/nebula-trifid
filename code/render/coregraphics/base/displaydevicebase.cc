//------------------------------------------------------------------------------
//  displaydevicebase.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/displaydevicebase.h"
#include "coregraphics/renderdevice.h"

namespace Base
{
__ImplementClass(Base::DisplayDeviceBase, 'DSDB', Core::RefCounted);
__ImplementSingleton(Base::DisplayDeviceBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
DisplayDeviceBase::DisplayDeviceBase() :
    adapter(Adapter::Primary),
    displayMode(0, 0, 1024, 768, PixelFormat::X8R8G8B8),
    antiAliasQuality(AntiAliasQuality::None),
    fullscreen(false),
    modeSwitchEnabled(true),
    tripleBufferingEnabled(false),
    alwaysOnTop(false),
    verticalSync(true),
    isOpen(false),
    windowTitle("Nebula3 Application Window"),
    iconName("NebulaIcon"),
    windowData(0),
	embedded(false),
    resizable(true),
    decorated(true)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
DisplayDeviceBase::~DisplayDeviceBase()
{
    n_assert(!this->IsOpen());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Set the window title. An application should be able to change the
    window title at any time, that's why this is a virtual method, so that
    a subclass may override it.
*/
void
DisplayDeviceBase::SetWindowTitle(const Util::String& str)
{
    this->windowTitle = str;
}

//------------------------------------------------------------------------------
/**
    Open the display.
*/
bool
DisplayDeviceBase::Open()
{
    n_assert(!this->IsOpen());

    // notify all event handlers
    DisplayEvent openEvent(DisplayEvent::DisplayOpen);
    this->NotifyEventHandlers(openEvent);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the display.
*/
void
DisplayDeviceBase::Close()
{
    n_assert(this->IsOpen());

    // notify all event handlers
    DisplayEvent closeEvent(DisplayEvent::DisplayClose);
    this->NotifyEventHandlers(closeEvent);

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void 
DisplayDeviceBase::Reopen()
{
	n_assert(this->IsOpen());

	if (this->displayMode.GetWidth() != 0 && 
		this->displayMode.GetHeight() != 0)
	{
		// notify render device that our size has changed
		CoreGraphics::RenderDevice::Instance()->DisplayResized(this->displayMode.GetWidth(), this->displayMode.GetHeight());
	}	

	// notify all event handlers
	DisplayEvent closeEvent(DisplayEvent::DisplayReopen);
	this->NotifyEventHandlers(closeEvent);
}

//------------------------------------------------------------------------------
/**
*/
void 
DisplayDeviceBase::EnableCallbacks()
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
DisplayDeviceBase::DisableCallbacks()
{
    // implement in subclass
}

//------------------------------------------------------------------------------
/**
    Attach an event handler to the display device.
*/
void
DisplayDeviceBase::AttachEventHandler(const Ptr<DisplayEventHandler>& handler)
{
    n_assert(handler.isvalid());
    n_assert(InvalidIndex == this->eventHandlers.FindIndex(handler));
    this->eventHandlers.Append(handler);
    handler->OnAttach();
}

//------------------------------------------------------------------------------
/**
    Remove an event handler from the display device.
*/
void
DisplayDeviceBase::RemoveEventHandler(const Ptr<DisplayEventHandler>& handler)
{
    n_assert(handler.isvalid());
    IndexT index = this->eventHandlers.FindIndex(handler);
    n_assert(InvalidIndex != index);
    this->eventHandlers.EraseIndex(index);
    handler->OnRemove();
}

//------------------------------------------------------------------------------
/**
    Notify all event handlers about an event.
*/
bool
DisplayDeviceBase::NotifyEventHandlers(const DisplayEvent& event)
{
    bool handled = false;
    IndexT i;
    for (i = 0; i < this->eventHandlers.Size(); i++)
    {
        handled |= this->eventHandlers[i]->PutEvent(event);
    }
    return handled;
}

//------------------------------------------------------------------------------
/**
    Process window system messages. Override this method in a subclass.
*/
void
DisplayDeviceBase::ProcessWindowMessages()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Returns the display modes on the given adapter in the given pixel format.
*/
Util::Array<DisplayMode>
DisplayDeviceBase::GetAvailableDisplayModes(Adapter::Code adapter, PixelFormat::Code pixelFormat)
{
    // override this method in a subclass to do something useful
    Util::Array<DisplayMode> emptyArray;
    return emptyArray;
}

//------------------------------------------------------------------------------
/**
    This method checks the available display modes on the given adapter
    against the requested display modes and returns true if the display mode
    exists.
*/
bool
DisplayDeviceBase::SupportsDisplayMode(Adapter::Code adapter, const DisplayMode& requestedMode)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    This method returns the current adapter display mode. It can be used
    to get the current desktop display mode.
*/
DisplayMode
DisplayDeviceBase::GetCurrentAdapterDisplayMode(Adapter::Code adapter)
{
    DisplayMode emptyMode;
    return emptyMode;
}

//------------------------------------------------------------------------------
/**
    Checks if the given adapter exists.
*/
bool
DisplayDeviceBase::AdapterExists(Adapter::Code adapter)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    Returns information about the provided adapter.
*/
AdapterInfo
DisplayDeviceBase::GetAdapterInfo(Adapter::Code adapter)
{
    AdapterInfo emptyAdapterInfo;
    return emptyAdapterInfo;
}


} // namespace DisplayDevice
