//------------------------------------------------------------------------------
//  rendertargetcube.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/rendertargetcube.h"
#if __DX11__
namespace CoreGraphics
{
 __ImplementClass(CoreGraphics::RenderTargetCube, 'RTGC', Direct3D11::D3D11RenderTargetCube);
}
#elif __OGL4__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::RenderTargetCube, 'RTGC', OpenGL4::OGL4RenderTargetCube);
}
#elif __VULKAN__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::RenderTargetCube, 'RTGC', Vulkan::VkRenderTargetCube);
}
#elif __DX9__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::RenderTargetCube, 'RTGC', Direct3D9::D3D9RenderTargetCube);
}
#else
#error "RenderTarget class not implemented on this platform!"
#endif
