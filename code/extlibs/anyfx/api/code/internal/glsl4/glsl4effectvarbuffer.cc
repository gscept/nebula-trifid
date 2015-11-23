//------------------------------------------------------------------------------
//  glsl4effectvarbuffer.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "glsl4effectvarbuffer.h"
#include "glsl4effectprogram.h"
#include <assert.h>
#include "effectvarbuffer.h"

namespace AnyFX
{

unsigned InternalEffectVarbuffer::globalVarbufferCounter = 0;
//------------------------------------------------------------------------------
/**
*/
GLSL4EffectVarbuffer::GLSL4EffectVarbuffer()
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
	this->shaderStorageBlockBinding = globalVarbufferCounter++;
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
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarbuffer::SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarbuffer* master)
{
	InternalEffectVarbuffer::SetupSlave(programs, master);
	this->shaderStorageBlockBinding = globalVarbufferCounter++;

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
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarbuffer::Commit()
{
	if (this->currentLocation != GL_INVALID_INDEX)
	{
		EffectVarbuffer::OpenGLBufferBinding* buf = (EffectVarbuffer::OpenGLBufferBinding*)this->currentBufferHandle;
		if (buf != 0)
		{
			if (buf->bindRange)
			{
				GLSL4VarbufferRangeState state;
				state.buffer = buf->handle;
				state.offset = buf->offset;
				state.length = buf->size;
				//if (GLSL4VarbufferRangeStates[this->shaderStorageBlockBinding] != state)
				{
					GLSL4VarbufferRangeStates[this->shaderStorageBlockBinding] = state;
					glBindBufferRange(GL_SHADER_STORAGE_BUFFER, this->shaderStorageBlockBinding, buf->handle, buf->offset, buf->size);
				}
			}
			else
			{
				GLSL4VarbufferBaseState state;
				state.buffer = buf->handle;
				//if (GLSL4VarbufferBaseStates[this->shaderStorageBlockBinding] != state)
				{
					GLSL4VarbufferBaseStates[this->shaderStorageBlockBinding] = state;
					glBindBufferBase(GL_SHADER_STORAGE_BUFFER, this->shaderStorageBlockBinding, buf->handle);
				}
			}
		}
		else
		{
			GLSL4VarbufferBaseState state;
			state.buffer = 0;
			//if (GLSL4VarbufferBaseStates[this->shaderStorageBlockBinding] != state)
			{
				GLSL4VarbufferBaseStates[this->shaderStorageBlockBinding] = state;
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, this->shaderStorageBlockBinding, 0);
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
	this->activeProgram = opengl4Program->programHandle;
	this->currentLocation = this->activeMap[opengl4Program];
}

} // namespace AnyFX