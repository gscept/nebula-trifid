//------------------------------------------------------------------------------
//  display.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/display.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsinterface.h"

namespace Graphics
{
__ImplementClass(Graphics::Display, 'DISP', Core::RefCounted);
__ImplementSingleton(Graphics::Display);

using namespace Util;
using namespace CoreGraphics;
using namespace Messaging;

//------------------------------------------------------------------------------
/**
*/
Display::Display() :
    isOpen(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
Display::~Display()
{
    n_assert(!this->isOpen);
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
Display::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->resourceMappers.IsEmpty());
	this->isOpen = true;

    Ptr<Graphics::SetupGraphics> msg = Graphics::SetupGraphics::Create();
    msg->SetAdapter(this->settings.GetAdapter());
    msg->SetDisplayMode(this->settings.DisplayMode());
    msg->SetAntiAliasQuality(this->settings.GetAntiAliasQuality());
    msg->SetFullscreen(this->settings.IsFullscreen());
    msg->SetDisplayModeSwitchEnabled(this->settings.IsDisplayModeSwitchEnabled());
    msg->SetTripleBufferingEnabled(this->settings.IsTripleBufferingEnabled());
    msg->SetAlwaysOnTop(this->settings.IsAlwaysOnTop());
    msg->SetVerticalSyncEnabled(this->settings.IsVerticalSyncEnabled());
    msg->SetIconName(this->settings.GetIconName());
    msg->SetWindowTitle(this->settings.GetWindowTitle());
    msg->SetWindowData(this->windowData);
	msg->SetEmbedded(this->settings.IsEmbedded());
    msg->SetResizable(this->settings.IsResizable());
    msg->SetDecorated(this->settings.IsDecorated());
    msg->SetResourceMappers(this->resourceMappers);

    // clear resource mappers array, so there's no danger of accessing them
    // from outside the render thread
    this->resourceMappers.Clear();
    
	__StaticSend(GraphicsInterface, msg);    
    n_assert(msg->Handled());

    // update our display mode with the actual display mode
    this->settings.DisplayMode() = msg->GetActualDisplayMode();
    this->settings.SetFullscreen(msg->GetActualFullscreen());
    this->settings.SetAdapter(msg->GetActualAdapter());
    
}

//------------------------------------------------------------------------------
/**
*/
void
Display::Close()
{
    n_assert(this->isOpen);

    // FIXME: hmm, closing/reopening the display not currently supported
    this->isOpen = false;
}


//------------------------------------------------------------------------------
/**
*/
void 
Display::Reopen()
{
	Ptr<Graphics::UpdateWindow> updDisp = Graphics::UpdateWindow::Create();
	updDisp->SetAntiAliasQuality(this->settings.GetAntiAliasQuality());
	updDisp->SetDisplayMode(this->settings.DisplayMode());
	updDisp->SetFullscreen(this->settings.IsFullscreen());
	updDisp->SetWindowData(this->windowData);
	updDisp->SetTripleBufferingEnabled(this->settings.IsTripleBufferingEnabled());
	__StaticSend(GraphicsInterface, updDisp);	
}

//------------------------------------------------------------------------------
/**
*/
bool
Display::AdapterExists(CoreGraphics::Adapter::Code adapter)
{
    Ptr<Graphics::AdapterExists> msg = Graphics::AdapterExists::Create();
    msg->SetAdapter(adapter);
	__StaticSend(GraphicsInterface, msg);
    n_assert(msg->Handled());
    return msg->GetResult();
}

//------------------------------------------------------------------------------
/**
*/
Array<DisplayMode>
Display::GetAvailableDisplayModes(Adapter::Code adapter, PixelFormat::Code pixelFormat)
{
    Ptr<Graphics::GetAvailableDisplayModes> msg = Graphics::GetAvailableDisplayModes::Create();
    msg->SetAdapter(adapter);
    msg->SetPixelFormat(pixelFormat);
	__StaticSend(GraphicsInterface, msg);
    n_assert(msg->Handled());
    return msg->GetResult();
}

//------------------------------------------------------------------------------
/**
*/
bool
Display::SupportsDisplayMode(Adapter::Code adapter, const DisplayMode& requestedMode)
{
    Ptr<Graphics::SupportsDisplayMode> msg = Graphics::SupportsDisplayMode::Create();
    msg->SetAdapter(adapter);
    msg->SetDisplayMode(requestedMode);
    __StaticSend(GraphicsInterface, msg);
    n_assert(msg->Handled());
    return msg->GetResult();
}

//------------------------------------------------------------------------------
/**
*/
DisplayMode
Display::GetCurrentAdapterDisplayMode(Adapter::Code adapter)
{
    Ptr<Graphics::GetCurrentAdapterDisplayMode> msg = Graphics::GetCurrentAdapterDisplayMode::Create();
    msg->SetAdapter(adapter);
    __StaticSend(GraphicsInterface, msg);
    n_assert(msg->Handled());
    return msg->GetResult();
}

//------------------------------------------------------------------------------
/**
*/
AdapterInfo
Display::GetAdapterInfo(Adapter::Code adapter)
{
    Ptr<Graphics::GetAdapterInfo> msg = Graphics::GetAdapterInfo::Create();
    msg->SetAdapter(adapter);
    __StaticSend(GraphicsInterface, msg);
    n_assert(msg->Handled());
    return msg->GetResult();
}

//------------------------------------------------------------------------------
/**
*/
void
Display::AttachDisplayEventHandler(const Ptr<DisplayEventHandler>& handler)
{
    n_assert(this->isOpen);
    Ptr<Graphics::AttachDisplayEventHandler> msg = Graphics::AttachDisplayEventHandler::Create();
    msg->SetHandler(handler);
    __StaticSend(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Display::RemoveDisplayEventHandler(const Ptr<DisplayEventHandler>& handler)
{
    n_assert(this->isOpen);
    Ptr<Graphics::RemoveDisplayEventHandler> msg = Graphics::RemoveDisplayEventHandler::Create();
    msg->SetHandler(handler);
    __StaticSend(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Display::AttachRenderEventHandler(const Ptr<RenderEventHandler>& handler)
{
    n_assert(this->isOpen);
    Ptr<Graphics::AttachRenderEventHandler> msg = Graphics::AttachRenderEventHandler::Create();
    msg->SetHandler(handler);
    __StaticSend(GraphicsInterface, msg);
}

//------------------------------------------------------------------------------
/**
*/
void
Display::RemoveRenderEventHandler(const Ptr<RenderEventHandler>& handler)
{
    n_assert(this->isOpen);
    Ptr<Graphics::RemoveRenderEventHandler> msg = Graphics::RemoveRenderEventHandler::Create();
    msg->SetHandler(handler);
    __StaticSend(GraphicsInterface, msg);
}

} // namespace Graphics
