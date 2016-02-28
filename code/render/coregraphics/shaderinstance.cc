//------------------------------------------------------------------------------
//  shaderinstance.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/shaderinstance.h"

#if __DX11__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::ShaderInstance, 'SINS', Direct3D11::D3D11ShaderInstance);
}
#elif __OGL4__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::ShaderInstance, 'SINS', OpenGL4::OGL4ShaderInstance);
}
#elif __DX9__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::ShaderInstance, 'SINS', Direct3D9::D3D9ShaderInstance);
}
#else
#error "ShaderInstance class not implemented on this platform!"
#endif
