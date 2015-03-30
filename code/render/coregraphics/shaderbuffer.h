#pragma once
//------------------------------------------------------------------------------
/**
	@class CoreGraphics::ShaderBuffer

	A resource which stores shader accessible read/write data.

	(C) 2015 Individual contributors, see AUTHORS file
*/
#if __OGL4__
#include "coregraphics/ogl4/ogl4shaderbuffer.h"
namespace CoreGraphics
{
class ShaderBuffer : public OpenGL4::OGL4ShaderBuffer
{
	__DeclareClass(ShaderBuffer);
};
}
#else
#error "ShaderBuffer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
