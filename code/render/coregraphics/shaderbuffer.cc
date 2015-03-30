//------------------------------------------------------------------------------
//  feedbackbuffer.cc
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/shaderbuffer.h"

#if __OGL4__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::ShaderBuffer, 'SHBU', OpenGL4::OGL4ShaderBuffer);
}
#else
#error "ShaderBuffer class not implemented on this platform!"
#endif

