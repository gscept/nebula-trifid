//------------------------------------------------------------------------------
//  d3d9displaydevice.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/d3d9/d3d9displaydevice.h"
#include "coregraphics/d3d9/d3d9renderdevice.h"
#include "coregraphics/win360/d3d9types.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9DisplayDevice, 'D9DD', Win32::Win32DisplayDevice);
__ImplementSingleton(Direct3D9::D3D9DisplayDevice);

using namespace Util;
using namespace CoreGraphics;
using namespace Win360;

//------------------------------------------------------------------------------
/**
*/
D3D9DisplayDevice::D3D9DisplayDevice()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
D3D9DisplayDevice::~D3D9DisplayDevice()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    This method checks if the given adapter actually exists.
*/
bool
D3D9DisplayDevice::AdapterExists(Adapter::Code adapter)
{
    IDirect3D9* d3d9 = D3D9RenderDevice::GetDirect3D();
    return ((UINT)adapter) < d3d9->GetAdapterCount();
}

//------------------------------------------------------------------------------
/**
    Enumerate the available display modes on the given adapter in the
    given pixel format. If the adapter doesn't exist on this machine,
    an empty array is returned.
*/
Util::Array<DisplayMode>
D3D9DisplayDevice::GetAvailableDisplayModes(Adapter::Code adapter, PixelFormat::Code pixelFormat)
{
    n_assert(this->AdapterExists(adapter));
    HRESULT hr;
    IDirect3D9* d3d9 = D3D9RenderDevice::GetDirect3D();
    Array<DisplayMode> modeArray;
    D3DDISPLAYMODE d3dMode = { 0 };
    D3DFORMAT d3dPixelFormat = D3D9Types::AsD3D9PixelFormat(pixelFormat);
    UINT numModes = d3d9->GetAdapterModeCount(adapter, d3dPixelFormat);
    UINT i;
    for (i = 0; i < numModes; i++)
    {
        hr = d3d9->EnumAdapterModes(adapter, d3dPixelFormat, i, &d3dMode);
        DisplayMode mode;
        mode.SetWidth(d3dMode.Width);
        mode.SetHeight(d3dMode.Height);
        mode.SetPixelFormat(D3D9Types::AsNebulaPixelFormat(d3dMode.Format));
        if (InvalidIndex == modeArray.FindIndex(mode))
        {
            modeArray.Append(mode);
        }
    }
    return modeArray;
}

//------------------------------------------------------------------------------
/**
*/
bool
D3D9DisplayDevice::SupportsDisplayMode(Adapter::Code adapter, const DisplayMode& requestedMode)
{
    Util::Array<DisplayMode> modes = this->GetAvailableDisplayModes(adapter, requestedMode.GetPixelFormat());
    return InvalidIndex != modes.FindIndex(requestedMode);
}

//------------------------------------------------------------------------------
/**
*/
DisplayMode
D3D9DisplayDevice::GetCurrentAdapterDisplayMode(Adapter::Code adapter)
{
    n_assert(this->AdapterExists(adapter));
    IDirect3D9* d3d9 = D3D9RenderDevice::GetDirect3D();
    D3DDISPLAYMODE d3dMode = { 0 }; 
    HRESULT hr = d3d9->GetAdapterDisplayMode((UINT) adapter, &d3dMode);
    n_assert(SUCCEEDED(hr));
    DisplayMode mode(d3dMode.Width, d3dMode.Height, D3D9Types::AsNebulaPixelFormat(d3dMode.Format));
    return mode;
}

//------------------------------------------------------------------------------
/**
*/
AdapterInfo
D3D9DisplayDevice::GetAdapterInfo(Adapter::Code adapter)
{
    n_assert(this->AdapterExists(adapter));
    IDirect3D9* d3d9 = D3D9RenderDevice::GetDirect3D();
    D3DADAPTER_IDENTIFIER9 d3dInfo = { 0 };
    HRESULT hr = d3d9->GetAdapterIdentifier((UINT) adapter, 0, &d3dInfo);
    n_assert(SUCCEEDED(hr));

    AdapterInfo info;
    info.SetDriverName(d3dInfo.Driver);
    info.SetDescription(d3dInfo.Description);
    info.SetDeviceName(d3dInfo.DeviceName);
    info.SetDriverVersionLowPart(d3dInfo.DriverVersion.LowPart);
    info.SetDriverVersionHighPart(d3dInfo.DriverVersion.HighPart);
    info.SetVendorId(d3dInfo.VendorId);
    info.SetDeviceId(d3dInfo.DeviceId);
    info.SetSubSystemId(d3dInfo.SubSysId);
    info.SetRevision(d3dInfo.Revision);
    info.SetGuid(Guid((const unsigned char*) &(d3dInfo.DeviceIdentifier), sizeof(d3dInfo.DeviceIdentifier)));
    return info;
}

//------------------------------------------------------------------------------
/**
    Compute an adjusted window rectangle, taking monitor info, 
    windowed/fullscreen etc... into account.
*/
DisplayMode
D3D9DisplayDevice::ComputeAdjustedWindowRect()
{
    if (0 != this->parentWindow)
    {
        // if we're a child window, let parent class handle it
        return Win32DisplayDevice::ComputeAdjustedWindowRect();
    }
    else
    {
        // get monitor handle of selected adapter
        IDirect3D9* d3d9 = D3D9RenderDevice::GetDirect3D();
        n_assert(this->AdapterExists(this->adapter));
        HMONITOR hMonitor = d3d9->GetAdapterMonitor(this->adapter);
        MONITORINFO monitorInfo = { sizeof(monitorInfo), 0 };
        GetMonitorInfo(hMonitor, &monitorInfo);
        int monitorOriginX = monitorInfo.rcMonitor.left;
        int monitorOriginY = monitorInfo.rcMonitor.top;
        
        if (this->fullscreen)
        {
            // running in fullscreen mode
            if (!this->IsDisplayModeSwitchEnabled())
            {
                // running in "fake" fullscreen mode center the window on the desktop
                int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
                int monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
                int x = monitorOriginX + ((monitorWidth - this->displayMode.GetWidth()) / 2);
                int y = monitorOriginY + ((monitorHeight - this->displayMode.GetHeight()) / 2);
                return DisplayMode(x, y, this->displayMode.GetWidth(), this->displayMode.GetHeight(), this->displayMode.GetPixelFormat());
            }
            else
            {
                // running in normal fullscreen mode
                return DisplayMode(monitorOriginX, monitorOriginY, this->displayMode.GetWidth(), this->displayMode.GetHeight(), this->displayMode.GetPixelFormat());
            }
        }
        else
        {
            // need to make sure that the window client area is the requested width
            const DisplayMode& mode = this->displayMode;
            int adjXPos = monitorOriginX + mode.GetXPos();
            int adjYPos = monitorOriginY + mode.GetYPos();
            RECT adjRect;
            adjRect.left   = adjXPos;
            adjRect.right  = adjXPos + mode.GetWidth();
            adjRect.top    = adjYPos;
            adjRect.bottom = adjYPos + mode.GetHeight();
            AdjustWindowRect(&adjRect, this->windowedStyle, 0);
            int adjWidth = adjRect.right - adjRect.left;
            int adjHeight = adjRect.bottom - adjRect.top;
            return DisplayMode(adjXPos, adjYPos, adjWidth, adjHeight, this->displayMode.GetPixelFormat());
        }
    }
}

} // namespace CoreGraphics
