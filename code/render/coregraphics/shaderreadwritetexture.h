#pragma once
//------------------------------------------------------------------------------
/**
	@class CoreGraphics::ShaderReadWriteTexture

	Behaves similar to a ShaderReadWriteBuffer in that it is made just for reading write to from within a shader.
	And not for attaching as a framebuffer object, use RenderTarget for that.

	(C) 2016 Individual contributors, see AUTHORS file
*/
#if __OGL4__
#include "coregraphics/ogl4/ogl4shaderimage.h"
namespace CoreGraphics
{
	class ShaderReadWriteTexture : public OpenGL4::OGL4ShaderImage
	{
		__DeclareClass(ShaderReadWriteTexture);
	};
}
#else
#error "ShaderReadWriteTexture class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
