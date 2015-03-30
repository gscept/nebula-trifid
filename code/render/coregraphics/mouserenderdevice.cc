//------------------------------------------------------------------------------
//  mouserenderdevice.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/mouserenderdevice.h"

namespace CoreGraphics
{
#if (__DX11__ || __DX9__ || __OGL4__)
__ImplementClass(CoreGraphics::MouseRenderDevice, 'MRDV', Base::MouseRenderDeviceBase);
__ImplementSingleton(CoreGraphics::MouseRenderDevice);
#else
#error "MouseRenderDevice class not implemented on this platform!"
#endif

//------------------------------------------------------------------------------
/**
*/
MouseRenderDevice::MouseRenderDevice()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MouseRenderDevice::~MouseRenderDevice()
{
    __DestructSingleton;
}

} // namespace CoreGraphics
