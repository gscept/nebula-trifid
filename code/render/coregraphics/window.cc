//------------------------------------------------------------------------------
//  window.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/window.h"

namespace CoreGraphics
{
#if __DX11__
__ImplementClass(CoreGraphics::Window, 'DDVC', Direct3D11::D3D11Window);
#elif __OGL4__
__ImplementClass(CoreGraphics::Window, 'DDVC', OpenGL4::GLFWWindow);
#elif __DX9__
__ImplementClass(CoreGraphics::Window, 'DDVC', Direct3D9::D3D9Window);
#else
#error "Window class not implemented on this platform!"
#endif


} // namespace CoreGraphics

