#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::RenderTarget
    
    A render targets wraps up to 4 color buffers and an optional depth/stencil 
    buffer into a C++ object. The special default render target represents the 
    backbuffer and default depth/stencil surface.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#if __DX11__
#include "coregraphics/d3d11/d3d11rendertarget.h"
namespace CoreGraphics
{
class RenderTarget : public Direct3D11::D3D11RenderTarget
{
	__DeclareClass(RenderTarget);
};
}
#elif __OGL4__
#include "coregraphics/ogl4/ogl4rendertarget.h"
namespace CoreGraphics
{
class RenderTarget : public OpenGL4::OGL4RenderTarget
{
	__DeclareClass(RenderTarget);
};
}
#elif __DX9__
#include "coregraphics/d3d9/d3d9rendertarget.h"
namespace CoreGraphics
{
class RenderTarget : public Direct3D9::D3D9RenderTarget
{
    __DeclareClass(RenderTarget);
};
}
#else
#error "RenderTarget class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------

