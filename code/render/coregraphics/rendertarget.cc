//------------------------------------------------------------------------------
//  rendertarget.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/rendertarget.h"
#if __DX11__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::RenderTarget, 'RTGT', Direct3D11::D3D11RenderTarget);
}
#elif __OGL4__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::RenderTarget, 'RTGT', OpenGL4::OGL4RenderTarget);
}
#elif __DX9__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::RenderTarget, 'RTGT', Direct3D9::D3D9RenderTarget);
}
#else
#error "RenderTarget class not implemented on this platform!"
#endif
