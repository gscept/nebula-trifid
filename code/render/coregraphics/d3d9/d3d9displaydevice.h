#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9DisplayDevice
    
    Direct3D9 implementation of DisplayDevice class. Manages the application
    window.
    
    (C) 2006 Radon Labs GmbH
*/
#include "coregraphics/win32/win32displaydevice.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9DisplayDevice : public Win32::Win32DisplayDevice
{
    __DeclareClass(D3D9DisplayDevice);
    __DeclareSingleton(D3D9DisplayDevice);
public:
    /// constructor
    D3D9DisplayDevice();
    /// destructor
    virtual ~D3D9DisplayDevice();
    /// check if the adapter actually exists
    bool AdapterExists(CoreGraphics::Adapter::Code adapter);
    /// get available display modes on given adapter
    Util::Array<CoreGraphics::DisplayMode> GetAvailableDisplayModes(CoreGraphics::Adapter::Code adapter, CoreGraphics::PixelFormat::Code pixelFormat);
    /// return true if a given display mode is supported
    bool SupportsDisplayMode(CoreGraphics::Adapter::Code adapter, const CoreGraphics::DisplayMode& requestedMode);
    /// get current adapter display mode (i.e. the desktop display mode)
    CoreGraphics::DisplayMode GetCurrentAdapterDisplayMode(CoreGraphics::Adapter::Code adapter);
    /// get general info about display adapter
    CoreGraphics::AdapterInfo GetAdapterInfo(CoreGraphics::Adapter::Code adapter);

private:
    /// adjust window size taking client area into account
    virtual CoreGraphics::DisplayMode ComputeAdjustedWindowRect();
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
    