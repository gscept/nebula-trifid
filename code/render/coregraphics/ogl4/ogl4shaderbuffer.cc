//------------------------------------------------------------------------------
//  ogl4shaderbuffer.cc
//  (C) 2012-2014 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ogl4shaderbuffer.h"



namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4ShaderBuffer, 'O4SB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderBuffer::OGL4ShaderBuffer() :
	bufferIndex(0)
#ifdef OGL4_SHADER_BUFFER_ALWAYS_MAPPED
	,sync(0)
#endif
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4ShaderBuffer::~OGL4ShaderBuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderBuffer::Setup()
{
	ShaderBufferBase::Setup();
	glGenBuffers(1, &this->ogl4Buffer);

	GLint alignment;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);

	// calculate aligned size
	this->alignedSize = (this->size + alignment - 1) - (this->size + alignment - 1) % alignment;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->ogl4Buffer);
#ifdef OGL4_SHADER_BUFFER_ALWAYS_MAPPED
	GLenum mapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, this->alignedSize * this->NumBuffers, NULL, mapFlags | GL_DYNAMIC_STORAGE_BIT);
	this->buf = (GLubyte*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, this->alignedSize * this->NumBuffers, mapFlags);
	this->bufferLock = BufferLock::Create();
#else
	glBufferData(GL_SHADER_STORAGE_BUFFER, this->alignedSize * this->NumBuffers, NULL, GL_STREAM_DRAW);
#endif
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	n_assert(GLSUCCESS);

	// setup handle
	this->handle = new AnyFX::EffectVarbuffer::OpenGLBuffer;
	this->handle->size = this->alignedSize;
	this->handle->handle = this->ogl4Buffer;
	this->handle->bindRange = true;
	this->handle->offset = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderBuffer::Discard()
{
#ifdef OGL4_SHADER_BUFFER_ALWAYS_MAPPED
	this->bufferLock = 0;
#endif
	glDeleteBuffers(1, &this->ogl4Buffer);
	delete this->handle;
	ShaderBufferBase::Discard();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShaderBuffer::UpdateBuffer(void* data, SizeT offset, SizeT length)
{
	n_assert(length <= this->size);
	ShaderBufferBase::UpdateBuffer(data, offset, length);
	
 	this->bufferIndex = (this->bufferIndex + 1) % NumBuffers;
	this->handle->offset = this->alignedSize * this->bufferIndex;

#ifdef OGL4_SHADER_BUFFER_ALWAYS_MAPPED
	this->bufferLock->WaitForRange(0, length);
	GLubyte* currentBuf = this->buf + this->handle->offset;
	memcpy(currentBuf + offset, data, length);
#else
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->ogl4Buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, this->handle->offset, length, data);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif
}

} // namespace OpenGL4