#pragma once
//------------------------------------------------------------------------------
/**
	@class OpenGL4::OGL4UniformBuffer
	
	Wraps an OpenGL4 uniform buffer to enable updating and comitting shader variables in a buffered manner.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "coregraphics/shader.h"
#include "coregraphics/base/constantbufferbase.h"
#include "coregraphics/ogl4/ogl4bufferlock.h"
#include "afxapi.h"

#define OGL4_UNIFORM_BUFFER_ALWAYS_MAPPED (1)
//#define OGL4_BINDLESS (1)
namespace CoreGraphics
{
class BufferLock;
class Shader;
}

namespace OpenGL4
{
class OGL4UniformBuffer : public Base::ConstantBufferBase
{
	__DeclareClass(OGL4UniformBuffer);
public:
	/// constructor
	OGL4UniformBuffer();
	/// destructor
	virtual ~OGL4UniformBuffer();

    /// setup buffer
	void Setup(const SizeT numBackingBuffers = DefaultNumBackingBuffers);
    /// bind variables in a block with a name in a shader to this buffer
	void SetupFromBlockInShader(const Ptr<CoreGraphics::Shader>& shader, const Util::String& blockName, const SizeT numBackingBuffers = DefaultNumBackingBuffers);
    /// discard buffer
    void Discard();

    /// get the handle for this buffer
    void* GetHandle() const;
     
	/// begin updating a segment of the buffer, will effectively lock the buffer
	void BeginUpdateSync();
    /// update segment of buffer asynchronously, which might overwrite data if it hasn't been used yet
    void UpdateAsync(void* data, uint offset, uint size);
    /// update segment of buffer as array asynchronously, which might overwrite data if it hasn't been used yet
    void UpdateArrayAsync(void* data, uint offset, uint size, uint count);
    /// end updating asynchronously, which updates the GL buffer
    void EndUpdateSync();

    /// cycle buffers
    void CycleBuffers();

private:
    GLuint ogl4Buffer;
    AnyFX::OpenGLBufferBinding* handle;
	Ptr<OGL4BufferLock> bufferLock;
};

//------------------------------------------------------------------------------
/**
*/
inline void*
OGL4UniformBuffer::GetHandle() const
{
    return this->handle;
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4UniformBuffer::UpdateAsync(void* data, uint offset, uint size)
{
    n_assert(size <= this->size);
    GLubyte* currentBuf = (GLubyte*)this->buffer + this->handle->offset;
    memcpy(currentBuf + offset, data, size);
}

//------------------------------------------------------------------------------
/**
*/
inline void
OpenGL4::OGL4UniformBuffer::UpdateArrayAsync(void* data, uint offset, uint size, uint count)
{
    n_assert(size <= this->size);
    GLubyte* currentBuf = (GLubyte*)this->buffer + this->handle->offset;
    memcpy(currentBuf + offset, data, size);
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4UniformBuffer::BeginUpdateSync()
{
	ConstantBufferBase::BeginUpdateSync();
	if (!this->sync)
	{
		if (this->bufferIndex == 0) this->bufferLock->WaitForBuffer(0);
	}
}

//------------------------------------------------------------------------------
/**
*/
inline void
OGL4UniformBuffer::EndUpdateSync()
{
    // only sync if we made changes
    if (this->isDirty)
    {
		glInvalidateBufferSubData(this->ogl4Buffer, this->handle->offset, this->size);
#if OGL4_BINDLESS
        glNamedBufferSubData(this->ogl4Buffer, this->handle->offset, this->size, this->buffer);
#else
        glBindBuffer(GL_UNIFORM_BUFFER, this->ogl4Buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, this->handle->offset, this->size, this->buffer);
#endif
    }

	if (!this->sync)
	{
		if (this->bufferIndex == this->numBuffers - 1) this->bufferLock->LockBuffer(0);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ConstantBufferBase::EndUpdateSync();
}

} // namespace OpenGL4