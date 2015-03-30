//------------------------------------------------------------------------------
//  multiplerendertarget.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/multiplerendertarget.h"
#if __DX9__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::MultipleRenderTarget, 'MRTG', Base::MultipleRenderTargetBase);
}
#elif __DX11__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::MultipleRenderTarget, 'MRTG', Direct3D11::D3D11MultipleRenderTarget);
}
#elif __OGL4__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::MultipleRenderTarget, 'MRTG', OpenGL4::OGL4MultipleRenderTarget);
}
#else
#error "MultipleRenderTarget class not implemented on this platform!"
#endif
