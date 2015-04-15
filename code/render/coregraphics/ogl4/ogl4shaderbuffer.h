#pragma once
//------------------------------------------------------------------------------
/**
	@class OpenGL4::OGL4ShaderBuffer
	
	Implements an OpenGL4 shader buffer, which is basically a shader storage buffer.

	Hmm, if one defines the OGL4_SHADER_BUFFER_ALWAYS_MAPPED, one must also make sure to lock the buffer after rendering is done.
	
	(C) 2012-2014 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------

//#define OGL4_SHADER_BUFFER_ALWAYS_MAPPED

#include "coregraphics/base/shaderbufferbase.h"
#include "coregraphics/bufferlock.h"
#include "afxapi.h"
namespace OpenGL4
{
class OGL4ShaderBuffer : public Base::ShaderBufferBase
{
	__DeclareClass(OGL4ShaderBuffer);
public:
	/// constructor
	OGL4ShaderBuffer();
	/// destructor
	virtual ~OGL4ShaderBuffer();

	/// setup buffer
	void Setup();
	/// discard buffer
	void Discard();

	/// update buffer region (offset = 0, length = size means the entire buffer)
	void UpdateBuffer(void* data, SizeT offset, SizeT length);

	/// return handle
	void* GetHandle() const;

	static const int NumBuffers = 3;

private:

	AnyFX::EffectVarbuffer::OpenGLBuffer* handle;

#ifdef OGL4_SHADER_BUFFER_ALWAYS_MAPPED

	GLubyte* buf;
	GLsync sync;
	Ptr<CoreGraphics::BufferLock> bufferLock;
#endif
	GLuint ogl4Buffer;
	
	IndexT bufferIndex;
	SizeT alignedSize;
};

//------------------------------------------------------------------------------
/**
*/
inline void*
OGL4ShaderBuffer::GetHandle() const
{
	return this->handle;
}

} // namespace OpenGL4