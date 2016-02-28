#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::Display
    
    The Display object is used to access DisplayDevice functionality
    from a different thread. Usually only the main thread creates a 
    display object.

    FIXME:
        Hmmm, consider merging this with the display device.
        Shouldn't really be a singleton, and should have a list of views.        

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/threadsafedisplayeventhandler.h"
#include "coregraphics/threadsaferendereventhandler.h"
#include "resources/resourcemapper.h"
#include "graphics/displaysettings.h"
#include "util/blob.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class Display : public Core::RefCounted
{
    __DeclareClass(Display);
    __DeclareSingleton(Display);
public:
    /// constructor  
    Display();
    /// destructor
    virtual ~Display();

    /// access to display settings
    DisplaySettings& Settings();
    /// set optional window data (on Win32 this may be a HWND, on Linux this might be an X11 struct)
    void SetWindowData(const Util::Blob& data);
    /// get optional window data
    const Util::Blob& GetWindowData() const;
    /// set resource mappers (NOTE: there is not Getter for this to prevent messing around with render-thread objects!)
    void SetResourceMappers(const Util::Array<Ptr<Resources::ResourceMapper> >& resourceMappers);

    /// open the display (waits for completion)
    void Open();
    /// close the display (waits for completion)
    void Close();
	/// reopens display, useful for changing display modes
	void Reopen();
    /// return true if display is currently open
    bool IsOpen() const;

    /// return true if adapter exists (waits for completion)
    bool AdapterExists(CoreGraphics::Adapter::Code adapter);
    /// get available display modes on given adapter (waits for completion)
    Util::Array<CoreGraphics::DisplayMode> GetAvailableDisplayModes(CoreGraphics::Adapter::Code adapter, CoreGraphics::PixelFormat::Code pixelFormat);
    /// return true if a given display mode is supported (waits for completion)
    bool SupportsDisplayMode(CoreGraphics::Adapter::Code adapter, const CoreGraphics::DisplayMode& requestedMode);
    /// get current adapter display mode (i.e. the desktop display mode) (waits for completion)
    CoreGraphics::DisplayMode GetCurrentAdapterDisplayMode(CoreGraphics::Adapter::Code adapter);
    /// get general info about display adapter (waits for completion)
    CoreGraphics::AdapterInfo GetAdapterInfo(CoreGraphics::Adapter::Code adapter);
    /// attach a display event handler
    void AttachDisplayEventHandler(const Ptr<CoreGraphics::DisplayEventHandler>& displayEventHandler);
    /// remove a display event handler 
    void RemoveDisplayEventHandler(const Ptr<CoreGraphics::DisplayEventHandler>& displayEventHandler);
    /// attach a render event handler
    void AttachRenderEventHandler(const Ptr<CoreGraphics::RenderEventHandler>& renderEventHandler);
    /// remove a render event handler
    void RemoveRenderEventHandler(const Ptr<CoreGraphics::RenderEventHandler>& renderEventHandler);

private:
    DisplaySettings settings;
    bool isOpen;
    Util::Blob windowData;
    Util::Array<Ptr<Resources::ResourceMapper> > resourceMappers;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
Display::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline DisplaySettings&
Display::Settings()
{
    return this->settings;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Display::SetWindowData(const Util::Blob& data)
{
    this->windowData = data;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Blob&
Display::GetWindowData() const
{
    return this->windowData;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Display::SetResourceMappers(const Util::Array<Ptr<Resources::ResourceMapper> >& mappers)
{
    this->resourceMappers = mappers;
}

} // namespace Graphics
//------------------------------------------------------------------------------

    