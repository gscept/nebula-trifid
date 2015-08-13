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
	buffer(-1),
	activeProgram(-1),
	uniformBlockLocation(-1),
    uniformOffsets(NULL),
	auxBuffers(0)
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

	// release resources
	this->glBuffer = 0;
	if (this->masterBlock)
	{
		delete this->bufferLock;
		delete this->glBufferOffset;
		delete this->elementIndex;
	}
	else
	{
		this->bufferLock = 0;
		this->glBufferOffset = 0;
		this->elementIndex = 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarblock::Setup(eastl::vector<InternalEffectProgram*> programs)
{
	InternalEffectVarblock::Setup(programs);

    const char** names = new const char*[this->variables.size()];
    for (unsigned index = 0; index < this->variables.size(); index++) names[index] = this->variables[index]->GetName().c_str();
    GLint* offsets = new GLint[this->variables.size()];

    // create offset array
    this->uniformOffsets = new unsigned[this->variables.size()];
    memset(this->uniformOffsets, 0, this->variables.size() * sizeof(unsigned));

	this->uniformBlockBinding = globalVarblockCounter++;
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
			glUniformBlockBinding(opengl4Program->programHandle, location, this->uniformBlockBinding);	
            glGetActiveUniformBlockiv(opengl4Program->programHandle, location, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&this->size);

            // setup uniforms (since we have a shared layout, this should be consistent between ALL blocks)
            this->SetupUniformOffsets(opengl4Program, location);

            // make active
            this->active = true;
		}
	}

    delete [] names;
    delete [] offsets;

	// setup variable offsets and sizes
    for (i = 0; i < this->variables.size(); i++)
    {
        InternalEffectVariable* variable = this->variables[i];
        //variable->sharedByteOffset = &this->uniformOffsets[i];
    }
    
    // get alignment
    // GLint alignment;
    //glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    // calculate aligned size
    // this->size = (bufferSize + alignment - 1) - (bufferSize + alignment - 1) % alignment;

    /*
	// setup buffer
	glGenBuffers(1, &this->buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, this->buffer);
    GLenum flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_UNIFORM_BUFFER, this->alignedSize * this->numBackingBuffers * 3, NULL, flags | GL_DYNAMIC_STORAGE_BIT);
	this->glBuffer = (GLchar*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, this->alignedSize * this->numBackingBuffers * 3, flags);
	this->glBackingBuffer = new GLchar[this->alignedSize];
    this->glBufferOffset = new GLuint;
    *this->glBufferOffset = 0;
	this->elementIndex = new GLuint;
	*this->elementIndex = 0;
	this->ringLocks = new eastl::vector<GLboolean>();
	this->ringLocks->resize(3);

	for (i = 0; i < this->ringLocks->size(); i++)
	{
		this->ringLocks->at(i) = false;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// create buffer lock
	this->bufferLock = new GLSL4BufferLock;
	this->bufferLock->Setup(3);
    */

    // setup default values
    //InternalEffectVarblock::SetupDefaultValues();
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
    this->uniformOffsets = mainBlock->uniformOffsets;

	this->uniformBlockBinding = mainBlock->uniformBlockBinding;
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
			glUniformBlockBinding(opengl4Program->programHandle, location, this->uniformBlockBinding);
            glGetActiveUniformBlockiv(opengl4Program->programHandle, location, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&mainBlock->size);

            // setup uniforms (since we have a shared layout, this should be consistent between ALL blocks)
            this->SetupUniformOffsets(opengl4Program, location);

            // make active
            mainBlock->active = true;
		}
	}

    for (i = 0; i < this->variables.size(); i++)
    {
        InternalEffectVariable* variable = this->variables[i];

        // this byte offset should be shared by ALL blocks as long as we use the 'shared' qualifier
        // bind the shared offset to this location into the singleton offset array
        //variable->sharedByteOffset = &this->uniformOffsets[i];
    }

    delete [] names;
    delete [] offsets;

	// copy GL buffer
	this->buffer			= mainBlock->buffer;
	this->size              = mainBlock->size;
	this->elementIndex		= mainBlock->elementIndex;
	this->glBuffer			= mainBlock->glBuffer;
	this->glBackingBuffer	= mainBlock->glBackingBuffer;
	this->glBufferOffset	= mainBlock->glBufferOffset;
	this->bufferLock		= mainBlock->bufferLock;
	this->bufferSize		= mainBlock->bufferSize;
	this->ringLocks			= mainBlock->ringLocks;

    // setup default values
    //InternalEffectVarblock::SetupDefaultValues();
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::SetVariable(InternalEffectVariable* var, void* value)
{    
    /*
	if (this->manualFlushing)
	{
		char* data = (this->glBackingBuffer + var->byteOffset);
		memcpy(data, value, var->byteSize);
	}
	else
	{
		char* data = (this->glBuffer + *this->glBufferOffset + var->byteOffset);
		this->UnlockBuffer();
		memcpy(data, value, var->byteSize);
		this->isDirty = true;
	}
    */
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::SetVariableArray(InternalEffectVariable* var, void* value, size_t size)
{
    /*
	if (this->manualFlushing)
	{
		char* data = (this->glBackingBuffer + var->byteOffset);
		memcpy(data, value, size);
	}
	else
	{
		char* data = (this->glBuffer + *this->glBufferOffset + var->byteOffset);
		this->UnlockBuffer();
		memcpy(data, value, size);
		this->isDirty = true;
	}
    */
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::SetVariableIndexed(InternalEffectVariable* var, void* value, unsigned i)
{
    /*
	if (this->manualFlushing)
	{
		char* data = (this->glBackingBuffer + var->byteOffset + i * var->byteSize);
		memcpy(data, value, var->byteSize);
	}
	else
	{
		char* data = (this->glBuffer + *this->glBufferOffset + var->byteOffset + i * var->byteSize);
		this->UnlockBuffer();
		memcpy(data, value, var->byteSize);
		this->isDirty = true;
	}
    */
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::Apply()
{
	// bind buffer to binding point, piece of cake!
    //glBindBufferRange(GL_UNIFORM_BUFFER, this->uniformBlockBinding, this->buffers[0], *this->glBufferOffset, this->dataBlock->size);
	//glBindBufferBase(GL_UNIFORM_BUFFER, this->uniformBlockBinding, this->buffers[0]);
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::Commit()
{
	if (this->currentLocation != GL_INVALID_INDEX)
	{
        EffectVarblock::OpenGLBufferBinding* buf = (EffectVarblock::OpenGLBufferBinding*)*this->currentBufferHandle;
        if (buf != 0)
        {
            if (buf->bindRange)
            {
                GLSL4VarblockRangeState state;
                state.buffer = buf->handle;
                state.offset = buf->offset;
                state.length = buf->size;
                if (GLSL4VarblockRangeStates[this->uniformBlockBinding] != state)
                {
                    GLSL4VarblockRangeStates[this->uniformBlockBinding] = state;
                    glBindBufferRange(GL_UNIFORM_BUFFER, this->uniformBlockBinding, buf->handle, buf->offset, buf->size);
                }
            }
            else
            {
                GLSL4VarblockBaseState state;
                state.buffer = buf->handle;
                if (GLSL4VarblockBaseStates[this->uniformBlockBinding] != state)
                {
                    GLSL4VarblockBaseStates[this->uniformBlockBinding] = state;
                    glBindBufferBase(GL_UNIFORM_BUFFER, this->uniformBlockBinding, buf->handle);
                }
            }
        }
        else
        {
			GLSL4VarblockBaseState state;
			state.buffer = 0;
			if (GLSL4VarblockBaseStates[this->uniformBlockBinding] != state)
			{
				GLSL4VarblockBaseStates[this->uniformBlockBinding] = state;
				glBindBufferBase(GL_UNIFORM_BUFFER, this->uniformBlockBinding, 0);
			}
        }
        /*
		if (this->manualFlushing)
		{
			GLSL4VarblockBaseState state;
			GLuint ringIndex = *this->elementIndex / 3;
			state.buffer = this->auxBuffers[ringIndex];
			if (GLSL4VarblockBaseStates[this->uniformBlockBinding] != state)
			{
				// if we are flushing manually, then bind the entire buffer
				GLSL4VarblockBaseStates[this->uniformBlockBinding] = state;
				glBindBufferBase(GL_UNIFORM_BUFFER, this->uniformBlockBinding, state.buffer);
			}
		}
		else
		{
			GLSL4VarblockRangeState state;
			state.buffer = this->buffer;
			state.offset = *this->glBufferOffset;
			state.length = this->alignedSize;
			if (GLSL4VarblockRangeStates[this->uniformBlockBinding] != state)
			{
				GLSL4VarblockRangeStates[this->uniformBlockBinding] = state;
				glBindBufferRange(GL_UNIFORM_BUFFER, this->uniformBlockBinding, state.buffer, state.offset, state.length);
			}
		}
        */
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::PostDraw()
{
    /*
    if (this->isDirty)
    {        
        // only perform automatic locking if we haven't specified it to be manual
		if (!this->manualFlushing)
		{
			//this->bufferLock->LockAccumulatedRanges();
			this->LockBuffer();
		}			
        this->isDirty = false;

        // also make sure the master block is flagged as not dirty anymore
        if (this->masterBlock)
        {
            this->masterBlock->isDirty = false;
        }
    }
    */
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarblock::Activate(InternalEffectProgram* program)
{
	GLSL4EffectProgram* opengl4Program = dynamic_cast<GLSL4EffectProgram*>(program);
	assert(0 != opengl4Program);
	this->activeProgram = opengl4Program->programHandle;
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
    // setup indices and setup the uniforms
    GLint* indices = new GLint[this->variables.size()];
    glGetActiveUniformBlockiv(program->programHandle, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices);
    glGetActiveUniformsiv(program->programHandle, this->variables.size(), (GLuint*)indices, GL_UNIFORM_OFFSET, (GLint*)this->uniformOffsets);

	// this stuff assigns the variable offset to a variable
	unsigned i;
	for (i = 0; i < this->variables.size(); i++)
	{
		GLsizei length;
		glGetActiveUniformsiv(program->programHandle, 1, (const GLuint*)&indices[i], GL_UNIFORM_NAME_LENGTH, &length);
		GLchar* buf = new GLchar[length];
		glGetActiveUniformName(program->programHandle, indices[i], length, NULL, buf);
		eastl::string name(buf); 

		// ugh, need to remove [0] from arrays...
		size_t indexOfArray = name.find("[0]");
		if (indexOfArray != eastl::string::npos) name = name.substr(0, indexOfArray);
		this->variablesByName[name]->sharedByteOffset = &this->uniformOffsets[i];
		delete [] buf;
	}	
	delete[] indices;

}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::LockBuffer()
{
	// calculate the next element, it should reset when we reach the number of buffers * 3
	*this->elementIndex = (*this->elementIndex + 1) % (this->numBackingBuffers * 3);

	// the index for the bucket must be an integer division on the size of buckets
	GLint ringIndex = *this->elementIndex / this->numBackingBuffers;
	bool lastElementInRing = (*this->elementIndex % this->numBackingBuffers) == 0;

	// make sure we lock the PREVIOUS ring
	ringIndex = (ringIndex - 1) >= 0 ? (ringIndex - 1) : 2;
	if (!this->ringLocks->at(ringIndex) && lastElementInRing)
	{
		if (!this->noSync)
		{
			this->bufferLock->NextBuffer();
			this->bufferLock->LockRing();
		}		
		this->ringLocks->at(ringIndex) = true;
	}

	*this->glBufferOffset = *this->elementIndex * this->size;

	// move to next buffer
	//*this->ringIndex = ((*this->ringIndex + 1) / this->numBackingBuffers) % 3;
	//*this->glBufferOffset = *this->ringIndex * this->alignedSize;
}

//------------------------------------------------------------------------------
/**
*/
void 
GLSL4EffectVarblock::UnlockBuffer()
{
	// the index for the bucket must be an integer division on the size of buckets
	GLint ringIndex = *this->elementIndex / this->numBackingBuffers;

	// make sure we lock the PREVIOUS ring
	//ringIndex = ringIndex - 1 < 0 ? 2 : ringIndex;
	bool firstElementInRing = (*this->elementIndex % this->numBackingBuffers) == 0;
	if (this->ringLocks->at(ringIndex) && firstElementInRing)
	{
		if (!this->noSync)
		{
			this->bufferLock->WaitForRing();
		}
		this->ringLocks->at(ringIndex) = false;
	}
}

//------------------------------------------------------------------------------
/**
	This is basically a one-way street, once this is done, there is no going back :D
*/
void
GLSL4EffectVarblock::SetFlushManually(bool b)
{
    /*
	if (b)
	{
		// unmap persistent buffer
		glBindBuffer(GL_UNIFORM_BUFFER, this->buffer);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// get master block
		GLSL4EffectVarblock* mainBlock = dynamic_cast<GLSL4EffectVarblock*>(this->masterBlock);

		// create array for aux buffers
		mainBlock->glBuffer = 0;
		mainBlock->auxBuffers = new GLuint[this->numBackingBuffers];

		// generate buffers
		glGenBuffers(this->numBackingBuffers, mainBlock->auxBuffers);
		
		// iterate through buffers and setup
		unsigned i;
		for (i = 0; i < this->numBackingBuffers; i++)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, mainBlock->auxBuffers[i]);
			glBufferData(GL_UNIFORM_BUFFER, this->alignedSize, NULL, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		// iterate through children and set their aux buffer pointer
		for (i = 0; i < mainBlock->childBlocks.size(); i++)
		{
			GLSL4EffectVarblock* childBlock = dynamic_cast<GLSL4EffectVarblock*>(mainBlock->childBlocks[i]);
			childBlock->auxBuffers	= mainBlock->auxBuffers;
			childBlock->glBuffer	= mainBlock->glBuffer;
		}
	}
	else
	{
		// cleanup aux buffers
		GLSL4EffectVarblock* mainBlock = dynamic_cast<GLSL4EffectVarblock*>(this->masterBlock);
		glDeleteBuffers(this->numBackingBuffers, mainBlock->auxBuffers);
		delete[] mainBlock->auxBuffers;
		mainBlock->auxBuffers = 0;

		// remap persistent buffer
		glBindBuffer(GL_UNIFORM_BUFFER, this->buffer);
		GLenum flags = GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;
		mainBlock->glBuffer = (GLchar*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, this->alignedSize * this->numBackingBuffers * 3, flags);	// FIXME: apply to all children too!
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// iterate through children, update their aux buffer pointer and gl buffer pointer
		unsigned i;
		for (i = 0; i < mainBlock->childBlocks.size(); i++)
		{
			GLSL4EffectVarblock* childBlock = dynamic_cast<GLSL4EffectVarblock*>(mainBlock->childBlocks[i]);
			childBlock->auxBuffers	= mainBlock->auxBuffers;
			childBlock->glBuffer	= mainBlock->glBuffer;
		}
	}
	*/
	InternalEffectVarblock::SetFlushManually(b);
}

//------------------------------------------------------------------------------
/**
*/
void
GLSL4EffectVarblock::FlushBuffer()
{
    /*
	GLuint ringIndex = *this->elementIndex / this->numBackingBuffers;
	glBindBuffer(GL_UNIFORM_BUFFER, this->auxBuffers[ringIndex]);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, this->bufferSize, this->glBackingBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
    */
}

} // namespace AnyFX