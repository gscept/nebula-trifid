//------------------------------------------------------------------------------
//  constantbuffer.cc
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/constantbuffer.h"

#if __OGL4__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::ConstantBuffer, 'COBU', OpenGL4::OGL4UniformBuffer);
}
#else
#error "ShaderBuffer class not implemented on this platform!"
#endif

