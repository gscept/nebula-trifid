//------------------------------------------------------------------------------
//  glsl4effectvarblock.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "glsl4effectvarblock.h"
#include "glsl4effectprogram.h"
#include <assert.h>
#include "internal/internaleffectvariable.h"
#include "effectvarblock.h"

#define a_max(x, y) (x > y ? x : y)
namespace AnyFX
{

unsigned InternalEffectVarblock::globalVarblockCounter = 0;
//------------------------------------------------------------------------------
/**
*/
GLSL4EffectVarblock::GLSL4EffectVarblock() :
	activeProgramHandle(-1),
    uniformOffsets(NULL),
	uniformBlockBinding(GL_INVALID_INDEX)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GLSL4EffectVarblock::~GLSL4EffectVarblock()
{
    /*
	// unmap stuff
    glBindBuffer(GL_UNIFORM_BUFFER, this->buffer);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
	glDeleteBuffers(1, &this->buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    */
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarblock::Setup(eastl::vector<InternalEffectProgram*> programs)
{
	InternalEffectVarblock::Setup(programs);
	*this->bufferHandle = new OpenGLBufferBinding;

    const char** names = new const char*[this->variables.size()];
    for (unsigned index = 0; index < this->variables.size(); index++) names[index] = this->variables[index]->GetName().c_str();
    GLint* offsets = new GLint[this->variables.size()];

    // create offset array
    this->uniformOffsets = new unsigned[this->variables.size()];
    memset(this->uniformOffsets, 0, this->variables.size() * sizeof(unsigned));
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &this->offsetAlignment);

	//this->uniformBlockBinding = globalVarblockCounter++;
	unsigned i;
	for (i = 0; i < programs.size(); i++)
	{
		GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(programs[i]);
		assert(0 != opengl4Program);

		GLuint location = glGetUniformBlockIndex(opengl4Program->programHandle, this->name.c_str());
		this->activeMap[opengl4Program] = location;
		if (location != GL_INVALID_INDEX)
		{
			// now tell the program in which binding slot this buffer should be 
			//glUniformBlockBinding(opengl4Program->programHandle, location, this->uniformBlockBinding);	
			glGetActiveUniformBlockiv(opengl4Program->programHandle, location, GL_UNIFORM_BLOCK_BINDING, (GLint*)&this->uniformBlockBinding);
            glGetActiveUniformBlockiv(opengl4Program->programHandle, location, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&this->size);

            // setup uniforms (since we have a shared layout, this should be consistent between ALL blocks)
            this->SetupUniformOffsets(opengl4Program, location);

            // make active
            this->active = true;
		}
#if GL4_MULTIBIND
		opengl4Program->glsl4Varblocks.push_back(this);
		opengl4Program->varblockBindsCount = a_max(this->uniformBlockBinding + 1, opengl4Program->varblockBindsCount);
#endif
	}

    delete [] names;
    delete [] offsets;

	// setup variable offsets and sizes
    for (i = 0; i < this->variables.size(); i++)
    {
        InternalEffectVariable* variable = this->variables[i];
        //variable->sharedByteOffset = &this->uniformOffsets[i];
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarblock::SetupSlave(eastl::vector<InternalEffectProgram*> programs, InternalEffectVarblock* master)
{
	InternalEffectVarblock::SetupSlave(programs, master);

    const char** names = new const char*[this->variables.size()];
    for (unsigned index = 0; index < this->variables.size(); index++) names[index] = this->variables[index]->GetName().c_str();
    GLint* offsets = new GLint[this->variables.size()];

	// assert the master block is of same backend
	GLSL4EffectVarblock* mainBlock = dynamic_cast<GLSL4EffectVarblock*>(master);
	assert(0 != mainBlock);

	//this->uniformBlockBinding = mainBlock->uniformBlockBinding;
	//this->uniformBlockBinding = globalVarblockCounter++;
	unsigned i;
	for (i = 0; i < programs.size(); i++)
	{
		GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(programs[i]);
		assert(0 != opengl4Program);

		GLuint location = glGetUniformBlockIndex(opengl4Program->programHandle, this->name.c_str());
		this->activeMap[opengl4Program] = location;
		if (location != GL_INVALID_INDEX)
		{
			// now tell the program in which binding slot this buffer should be 
			//glUniformBlockBinding(opengl4Program->programHandle, location, this->uniformBlockBinding);
			// hmm, this will set the block binding to the same thing each time, but it should be identical for this entire program...
			glGetActiveUniformBlockiv(opengl4Program->programHandle, location, GL_UNIFORM_BLOCK_BINDING, (GLint*)&this->uniformBlockBinding);
            glGetActiveUniformBlockiv(opengl4Program->programHandle, location, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&mainBlock->size);

            // setup uniforms (since we have a shared layout, this should be consistent between ALL blocks)
            this->SetupUniformOffsets(opengl4Program, location);

            // make active
            mainBlock->active = true;
		}
#if GL4_MULTIBIND
		opengl4Program->glsl4Varblocks.push_back(this);
		opengl4Program->varblockBindsCount = a_max(this->uniformBlockBinding + 1, opengl4Program->varblockBindsCount);
#endif
	}
	this->uniformOffsets = mainBlock->uniformOffsets;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &this->offsetAlignment);

    for (i = 0; i < this->variables.size(); i++)
    {
        InternalEffectVariable* variable = this->variables[i];

        // this byte offset should be shared by ALL blocks as long as we use the 'shared' qualifier
        // bind the shared offset to this location into the singleton offset array
		variable->sharedByteOffset = mainBlock->variables[i]->sharedByteOffset;
    }

    delete [] names;
    delete [] offsets;

	// copy GL buffer
	this->size              = mainBlock->size;
	this->bufferSize		= mainBlock->bufferSize;
	this->ringLocks			= mainBlock->ringLocks;

    // setup default values
    //InternalEffectVarblock::SetupDefaultValues();
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarblock::SetBuffer(void* handle)
{
	*this->bufferHandle = handle;
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::Commit()
{
	if (this->currentLocation != GL_INVALID_INDEX && this->uniformBlockBinding != GL_INVALID_INDEX)
	{
        OpenGLBufferBinding* buf = (OpenGLBufferBinding*)*this->bufferHandle;
        if (buf != 0)
        {
            if (buf->bindRange)
            {
#ifdef GL4_MULTIBIND
				this->activeProgram->varblockRangeBindBuffers[this->uniformBlockBinding] = buf->handle;
				this->activeProgram->varblockRangeBindOffsets[this->uniformBlockBinding] = buf->offset;
				this->activeProgram->varblockRangeBindSizes[this->uniformBlockBinding] = buf->size;
#else
                GLSL4VarblockRangeState state;
                state.buffer = buf->handle;
                state.offset = buf->offset;
                state.length = buf->size;
				if (GLSL4VarblockRangeStates[this->uniformBlockBinding] != state)
                {
					//assert(state.offset % this->offsetAlignment == 0);
					GLSL4VarblockRangeStates[this->uniformBlockBinding] = state;
					glBindBufferRange(GL_UNIFORM_BUFFER, this->uniformBlockBinding, state.buffer, state.offset, state.length);
                }
#endif
            }
            else
            {
#ifdef GL4_MULTIBIND
				this->activeProgram->varblockRangeBindBuffers[this->uniformBlockBinding] = buf->handle;
				this->activeProgram->varblockRangeBindOffsets[this->uniformBlockBinding] = 0;
				this->activeProgram->varblockRangeBindSizes[this->uniformBlockBinding] = buf->size;
#else
                GLSL4VarblockBaseState state;
                state.buffer = buf->handle;
				if (GLSL4VarblockBaseStates[this->uniformBlockBinding] != state)
                {
					GLSL4VarblockBaseStates[this->uniformBlockBinding] = state;
					glBindBufferBase(GL_UNIFORM_BUFFER, this->uniformBlockBinding, state.buffer);
                }
#endif
            }
        }
		else
		{
			this->activeProgram->varblockRangeBindBuffers[this->uniformBlockBinding] = 0;
			this->activeProgram->varblockRangeBindOffsets[this->uniformBlockBinding] = 0;
			this->activeProgram->varblockRangeBindSizes[this->uniformBlockBinding] = 0;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarblock::Activate(InternalEffectProgram* program)
{
	GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(program);
	assert(0 != opengl4Program);
	this->activeProgram = opengl4Program;
	this->activeProgramHandle = opengl4Program->programHandle;
	this->currentLocation = this->activeMap[opengl4Program];
}

//------------------------------------------------------------------------------
/**
	Hmm, we literally do this for each program everywhere, despite using the shared qualifier.

	Although this will give us a unique offset per each program and block, even though the variable use a shared byte offset...
*/
void
GLSL4EffectVarblock::SetupUniformOffsets(GLSL4EffectProgram* program, GLuint blockIndex)
{
	// get masterblock
	GLSL4EffectVarblock* masterBlock = static_cast<GLSL4EffectVarblock*>(this->masterBlock);

    // setup indices and setup the uniforms
    GLint* indices = new GLint[this->variables.size()];
	glGetActiveUniformBlockiv(program->programHandle, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices);
	glGetActiveUniformsiv(program->programHandle, this->variables.size(), (GLuint*)indices, GL_UNIFORM_OFFSET, (GLint*)masterBlock->uniformOffsets);

	// this stuff assigns the variable offset to a variable
	unsigned i;
	for (i = 0; i < masterBlock->variables.size(); i++)
	{
		//if (indices[i] == GL_INVALID_INDEX) continue;
		GLsizei length;
		glGetActiveUniformsiv(program->programHandle, 1, (const GLuint*)&indices[i], GL_UNIFORM_NAME_LENGTH, &length);
		GLchar* buf = new GLchar[length];
		glGetActiveUniformName(program->programHandle, indices[i], length, NULL, buf);
		eastl::string name(buf); 

		// ugh, need to remove [0] from arrays...
		size_t indexOfArray = name.find("[0]");
		if (indexOfArray != eastl::string::npos) name = name.substr(0, indexOfArray);
		masterBlock->variablesByName[name]->sharedByteOffset = &masterBlock->uniformOffsets[i];
		delete [] buf;
	}	
	delete[] indices;

}

} // namespace AnyFX