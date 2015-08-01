#pragma once
//------------------------------------------------------------------------------
/**
	@class CoreGraphics::ShaderReadWriteBuffer

	A shader read write buffer is a buffer bound in a shader which can be
    written to and read from the shader, as well as the CPU.

	(C) 2015 Individual contributors, see AUTHORS file
*/
#if __OGL4__
#include "coregraphics/ogl4/ogl4shaderstoragebuffer.h"
namespace CoreGraphics
{
class ShaderReadWriteBuffer : public OpenGL4::OGL4ShaderStorageBuffer
{
	__DeclareClass(ShaderReadWriteBuffer);
};
}
#else
#error "ShaderBuffer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
