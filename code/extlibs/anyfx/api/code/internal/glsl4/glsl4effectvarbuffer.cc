//------------------------------------------------------------------------------
//  glsl4effectvarbuffer.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "glsl4effectvarbuffer.h"
#include "glsl4effectprogram.h"
#include <assert.h>
#include "effectvarbuffer.h"

#define a_max(x, y) (x > y ? x : y)
namespace AnyFX
{

unsigned InternalEffectVarbuffer::globalVarbufferCounter = 0;
//------------------------------------------------------------------------------
/**
*/
GLSL4EffectVarbuffer::GLSL4EffectVarbuffer() :
	shaderStorageBlockBinding(GL_INVALID_INDEX)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GLSL4EffectVarbuffer::~GLSL4EffectVarbuffer()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarbuffer::Setup(eastl::vector<InternalEffectProgram*> programs)
{
	InternalEffectVarbuffer::Setup(programs);
	this->shaderStorageBlockBinding = globalVarbufferCounter++;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &this->offsetAlignment);

	unsigned i;
	for (i = 0; i < programs.size(); i++)
	{
		GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(programs[i]);
		assert(0 != opengl4Program);

		GLuint location = glGetProgramResourceIndex(opengl4Program->programHandle, GL_SHADER_STORAGE_BLOCK, this->name.c_str());
		this->activeMap[opengl4Program] = location;
		if (location != GL_INVALID_INDEX)
		{
			glShaderStorageBlockBinding(opengl4Program->programHandle, location, this->shaderStorageBlockBinding);
			this->active = true;
		}
#if GL4_MULTIBIND
		opengl4Program->glsl4Varbuffers.push_back(this);
		opengl4Program->varbufferBindsCount = a_max(this->shaderStorageBlockBinding + 1, opengl4Program->varbufferBindsCount);
#endif
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarbuffer::SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarbuffer* master)
{
	InternalEffectVarbuffer::SetupSlave(programs, master);

	GLSL4EffectVarbuffer* mainBuffer = dynamic_cast<GLSL4EffectVarbuffer*>(master);
	assert(0 != mainBuffer);

	//this->shaderStorageBlockBinding = mainBuffer->shaderStorageBlockBinding;
	this->shaderStorageBlockBinding = globalVarbufferCounter++;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &this->offsetAlignment);

	unsigned i;
	for (i = 0; i < programs.size(); i++)
	{
		GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(programs[i]);
		assert(0 != opengl4Program);

		GLuint location = glGetProgramResourceIndex(opengl4Program->programHandle, GL_SHADER_STORAGE_BLOCK, this->name.c_str());
		this->activeMap[opengl4Program] = location;
		if (location != GL_INVALID_INDEX)
		{
			glShaderStorageBlockBinding(opengl4Program->programHandle, location, this->shaderStorageBlockBinding);
			mainBuffer->active = true;
		}
#if GL4_MULTIBIND
		opengl4Program->glsl4Varbuffers.push_back(this);
		opengl4Program->varbufferBindsCount = a_max(this->shaderStorageBlockBinding + 1, opengl4Program->varbufferBindsCount);
#endif
	}
}

//------------------------------------------------------------------------------
/**
	Eh, can we even base bind 
*/
void
GLSL4EffectVarbuffer::Commit()
{
	if (this->currentLocation != GL_INVALID_INDEX)
	{
		EffectVarbuffer::OpenGLBufferBinding* buf = (EffectVarbuffer::OpenGLBufferBinding*)*this->currentBufferHandle;
		if (buf != 0)
		{
			if (buf->bindRange)
			{
#if GL4_MULTIBIND
				this->activeProgram->varbufferRangeBindBuffers[this->shaderStorageBlockBinding] = buf->handle;
				this->activeProgram->varbufferRangeBindOffsets[this->shaderStorageBlockBinding] = buf->offset;
				this->activeProgram->varbufferRangeBindSizes[this->shaderStorageBlockBinding] = buf->size;
#else
				GLSL4VarbufferRangeState state;
				state.buffer = buf->handle;
				state.offset = buf->offset;
				state.length = buf->size;
				GLuint binding = this->shaderStorageBlockBinding;
				if (GLSL4VarbufferRangeStates[binding] != state)
				{
					//if (state.length % this->offsetAlignment != 0) printf("WOW, THIS OFFSET IS NOT GOOD! %d\n", state.length);
					//assert(state.length % this->offsetAlignment == 0);
					GLSL4VarbufferRangeStates[binding] = state;
					glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, state.buffer, state.offset, state.length);
				}
#endif
			}
			else
			{
#if GL4_MULTIBIND
				this->activeProgram->varbufferRangeBindBuffers[this->shaderStorageBlockBinding] = buf->handle;
				this->activeProgram->varbufferRangeBindOffsets[this->shaderStorageBlockBinding] = 0;
				this->activeProgram->varbufferRangeBindSizes[this->shaderStorageBlockBinding] = buf->size;
#else
				GLSL4VarbufferBaseState state;
				state.buffer = buf->handle;
				GLuint binding = this->shaderStorageBlockBinding;
				if (GLSL4VarbufferBaseStates[binding] != state)
				{
					GLSL4VarbufferBaseStates[binding] = state;
					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, state.buffer);
				}
#endif
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarbuffer::Activate(InternalEffectProgram* program)
{
	GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(program);
	assert(0 != opengl4Program);
	this->activeProgram = opengl4Program;
	this->activeProgramHandle = opengl4Program->programHandle;
	this->currentLocation = this->activeMap[opengl4Program];
}

} // namespace AnyFX