#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::MouseRenderDevice
  
    Platform-wrapper for mouse pointer rendering.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#if (__DX11__ || __DX9__ || __OGL4__)
#include "coregraphics/base/mouserenderdevicebase.h"
namespace CoreGraphics
{
class MouseRenderDevice : public Base::MouseRenderDeviceBase
{
    __DeclareClass(MouseRenderDevice);
    __DeclareSingleton(MouseRenderDevice);
public:
    /// constructor
    MouseRenderDevice();
    /// destructor
    virtual ~MouseRenderDevice();
};
} // namespace CoreGraphics
#else
#error "CoreGraphics::MouseRenderDevice not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
