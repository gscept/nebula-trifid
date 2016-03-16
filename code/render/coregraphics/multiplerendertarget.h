#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::MultipleRenderTarget
    
    A multiple render targets wraps up to 4 rendertargets into a C++ object. The special default render target represents the 
    backbuffer and default depth/stencil surface.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#if __DX9__
#include "coregraphics/base/multiplerendertargetbase.h"
namespace CoreGraphics
{
class MultipleRenderTarget : public Base::MultipleRenderTargetBase
{
    __DeclareClass(MultipleRenderTarget);
};
}
#elif __DX11__
#include "coregraphics/d3d11/d3d11multiplerendertarget.h"
namespace CoreGraphics
{
class MultipleRenderTarget : public Direct3D11::D3D11MultipleRenderTarget
{
	__DeclareClass(MultipleRenderTarget);
};
}
#elif __OGL4__
#include "coregraphics/ogl4/ogl4multiplerendertarget.h"
namespace CoreGraphics
{
class MultipleRenderTarget : public OpenGL4::OGL4MultipleRenderTarget
{
	__DeclareClass(MultipleRenderTarget);
};
}
#else
#error "MultipleRenderTarget class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
