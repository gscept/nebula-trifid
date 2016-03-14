#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::DisplayDeviceBase
  
    A DisplayDevice object represents the display where the RenderDevice
    presents the rendered frame. Use the display device object to 
    get information about available adapters and display modes, and
    to set the preferred display mode of a Nebula3 application.

    FIXME:
        This being a singleton isn't really ideal.
        We might actually want to have more than one display...
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "coregraphics/antialiasquality.h"
#include "coregraphics/adapter.h"
#include "coregraphics/displayeventhandler.h"
#include "coregraphics/displaymode.h"
#include "coregraphics/adapterinfo.h"
#include "util/blob.h"
#include "events/event.h"

//------------------------------------------------------------------------------
namespace Base
{
class DisplayDeviceBase : public Core::RefCounted
{
    __DeclareClass(DisplayDeviceBase);
    __DeclareSingleton(DisplayDeviceBase);
public:
    /// constructor
    DisplayDeviceBase();
    /// destructor
    virtual ~DisplayDeviceBase();

    /// return true if adapter exists
    bool AdapterExists(CoreGraphics::Adapter::Code adapter);
    /// get available display modes on given adapter
    Util::Array<CoreGraphics::DisplayMode> GetAvailableDisplayModes(CoreGraphics::Adapter::Code adapter, CoreGraphics::PixelFormat::Code pixelFormat);
    /// return true if a given display mode is supported
    bool SupportsDisplayMode(CoreGraphics::Adapter::Code adapter, const CoreGraphics::DisplayMode& requestedMode);
    /// get current adapter display mode (i.e. the desktop display mode)
    CoreGraphics::DisplayMode GetCurrentAdapterDisplayMode(CoreGraphics::Adapter::Code adapter);
    /// get general info about display adapter
    CoreGraphics::AdapterInfo GetAdapterInfo(CoreGraphics::Adapter::Code adapter);

    /// set display adapter (make sure adapter exists!)
    void SetAdapter(CoreGraphics::Adapter::Code a);
    /// get display adapter
    CoreGraphics::Adapter::Code GetAdapter() const;
    /// set display mode (make sure the display mode is supported!)
    void SetDisplayMode(const CoreGraphics::DisplayMode& m);
    /// get display mode
    const CoreGraphics::DisplayMode& GetDisplayMode() const;
    /// set antialias quality
    void SetAntiAliasQuality(CoreGraphics::AntiAliasQuality::Code aa);
    /// get antialias quality
    CoreGraphics::AntiAliasQuality::Code GetAntiAliasQuality() const;

    /// set windowed/fullscreen mode
    void SetFullscreen(bool b);
    /// get windowed/fullscreen mode
    bool IsFullscreen() const;
    /// enable display mode switch when running fullscreen (default is true);
    void SetDisplayModeSwitchEnabled(bool b);
    /// is display mode switch enabled for fullscreen?
    bool IsDisplayModeSwitchEnabled() const;
    /// enable triple buffer for fullscreen (default is double buffering)
    void SetTripleBufferingEnabled(bool b);
    /// is triple buffer enabled for fullscreen?
    bool IsTripleBufferingEnabled() const;
    /// set always-on-top behaviour
    void SetAlwaysOnTop(bool b);
    /// get always-on-top behaviour
    bool IsAlwaysOnTop() const;
    /// turn vertical sync on/off
    void SetVerticalSyncEnabled(bool b);
    /// get vertical sync flag
    bool IsVerticalSyncEnabled() const;
    /// set optional window icon resource name
    void SetIconName(const Util::String& s);
    /// get optional window icon resource name
    const Util::String& GetIconName() const;
    /// set optional window data (for alien windows)
    void SetWindowData(const Util::Blob& h);
    /// get optional window data
    const Util::Blob& GetWindowData() const;
	/// set optional embedding property
	void SetEmbedded(bool b);
	/// return optional embedding property
	bool IsEmbedded();
    /// set allow resize flag
    void SetResizable(bool enable);
    /// get allow resize flag
    bool IsResizable() const;
    /// set decorated flag
    void SetDecorated(bool enable);
    /// get decorated flag
    bool IsDecorated() const;

    /// set window title string (can be changed anytime)
    void SetWindowTitle(const Util::String& t);
    /// get window title string
    const Util::String& GetWindowTitle() const;

    /// open the display
    bool Open();
    /// close the display
    void Close();
    /// return true if display is currently open
    bool IsOpen() const;
    /// process window system messages, call this method once per frame
    void ProcessWindowMessages();
	/// reopens the display device which enables switching from display modes
	void Reopen();
    /// enables callbacks
    void EnableCallbacks();
    /// disables callbacks
    void DisableCallbacks();

	/// posts an external event to the display device
	void PostEvent(const Event& event);

    /// attach a display event handler
    void AttachEventHandler(const Ptr<CoreGraphics::DisplayEventHandler>& h);
    /// remove a display event handler
    void RemoveEventHandler(const Ptr<CoreGraphics::DisplayEventHandler>& h);
        
protected:
    /// notify event handlers about an event
    bool NotifyEventHandlers(const CoreGraphics::DisplayEvent& e);

    CoreGraphics::Adapter::Code adapter;
    CoreGraphics::DisplayMode displayMode;
    CoreGraphics::AntiAliasQuality::Code antiAliasQuality;

    bool fullscreen;
    bool modeSwitchEnabled;
    bool tripleBufferingEnabled;
    bool alwaysOnTop;
    bool verticalSync;
    bool isOpen;
	bool embedded;
    bool resizable;
    bool decorated;

    Util::String windowTitle;
    Util::String iconName;
    Util::Blob windowData;

    Util::Array<Ptr<CoreGraphics::DisplayEventHandler> > eventHandlers;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
DisplayDeviceBase::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetDisplayMode(const CoreGraphics::DisplayMode& m)
{
    this->displayMode = m;
}

//------------------------------------------------------------------------------
/**
*/
inline const CoreGraphics::DisplayMode& 
DisplayDeviceBase::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetAntiAliasQuality(CoreGraphics::AntiAliasQuality::Code aa)
{
    this->antiAliasQuality = aa;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::AntiAliasQuality::Code
DisplayDeviceBase::GetAntiAliasQuality() const
{
    return this->antiAliasQuality;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetAdapter(CoreGraphics::Adapter::Code a)
{
    this->adapter = a;
}

//------------------------------------------------------------------------------
/**
*/
inline CoreGraphics::Adapter::Code
DisplayDeviceBase::GetAdapter() const
{
    return this->adapter;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetFullscreen(bool b)
{
    this->fullscreen = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DisplayDeviceBase::IsFullscreen() const
{
    return this->fullscreen;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetDisplayModeSwitchEnabled(bool b)
{
    this->modeSwitchEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DisplayDeviceBase::IsDisplayModeSwitchEnabled() const
{
    return this->modeSwitchEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetTripleBufferingEnabled(bool b)
{
    this->tripleBufferingEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DisplayDeviceBase::IsTripleBufferingEnabled() const
{
    return this->tripleBufferingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetAlwaysOnTop(bool b)
{
    this->alwaysOnTop = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DisplayDeviceBase::IsAlwaysOnTop() const
{
    return this->alwaysOnTop;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetVerticalSyncEnabled(bool b)
{
    this->verticalSync = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
DisplayDeviceBase::IsVerticalSyncEnabled() const
{
    return this->verticalSync;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetIconName(const Util::String& s)
{
    this->iconName = s;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
DisplayDeviceBase::GetIconName() const
{
    return this->iconName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
DisplayDeviceBase::GetWindowTitle() const
{
    return this->windowTitle;
}

//------------------------------------------------------------------------------
/**
*/
inline void
DisplayDeviceBase::SetWindowData(const Util::Blob& blob)
{
    if (blob.IsValid())
    {
        this->windowData = blob;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Blob&
DisplayDeviceBase::GetWindowData() const
{
    return this->windowData;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
DisplayDeviceBase::SetEmbedded( bool b )
{
	this->embedded = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
DisplayDeviceBase::IsEmbedded()
{
	return this->embedded;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
DisplayDeviceBase::SetResizable( bool enable )
{
    this->resizable = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool 
DisplayDeviceBase::IsResizable() const
{
    return this->resizable;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
DisplayDeviceBase::SetDecorated( bool enable )
{
    this->decorated = enable;
}

//------------------------------------------------------------------------------
/**
*/
inline 
bool 
DisplayDeviceBase::IsDecorated() const
{
    return this->decorated;
}
} // namespace Base
//------------------------------------------------------------------------------

