#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::RenderTargetCube
    
    A render target cube holds 6 individual render targets,
    which can be used to render to a cube map. This is useful for point light 
    shadowing or realtime reflection mapping.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#if __DX11__
#include "coregraphics/d3d11/d3d11rendertargetcube.h"
namespace CoreGraphics
{
class RenderTargetCube : public Direct3D11::D3D11RenderTargetCube
{
	__DeclareClass(RenderTargetCube);
};
}
#elif __OGL4__
#include "coregraphics/ogl4/ogl4rendertargetcube.h"
namespace CoreGraphics
{
class RenderTargetCube : public OpenGL4::OGL4RenderTargetCube
{
	__DeclareClass(RenderTargetCube);
};
}
#elif __VULKAN__
#include "coregraphics/vk/vkrendertargetcube.h"
namespace CoreGraphics
{
class RenderTargetCube : public Vulkan::VkRenderTargetCube
{
	__DeclareClass(RenderTargetCube);
};
}
#elif __DX9__
#include "coregraphics/d3d9/d3d9rendertargetcube.h"
namespace CoreGraphics
{
class RenderTargetCube : public Direct3D9::D3D9RenderTargetCube
{
    __DeclareClass(RenderTargetCube);
};
}
#else
#error "RenderTarget class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------

