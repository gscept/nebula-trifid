//------------------------------------------------------------------------------
//  ogl4vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/ogl4/ogl4feedbackbuffer.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/ogl4/ogl4types.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4FeedbackBuffer, 'O4FB', Base::FeedbackBufferBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
OGL4FeedbackBuffer::OGL4FeedbackBuffer() :
	ogl4TransformFeedback(0),
	bufferIndex(0),
	bufferOffset(0),
    mapCount(0),
	ogl4TransformFeedbackBuffer(NumBuffers),
	layouts(NumBuffers)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
OGL4FeedbackBuffer::~OGL4FeedbackBuffer()
{
	n_assert(0 == this->ogl4TransformFeedback);
    n_assert(0 == this->mapCount);
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4FeedbackBuffer::Setup()
{
	FeedbackBufferBase::Setup();
	glGenBuffers(NumBuffers, &this->ogl4TransformFeedbackBuffer[0]);

	IndexT i;
	for (i = 0; i < NumBuffers; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->ogl4TransformFeedbackBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, this->size * this->numElements, NULL, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		this->layouts[i] = VertexLayout::Create();
		this->layouts[i]->SetStreamBuffer(0, this->ogl4TransformFeedbackBuffer[i]);
		this->layouts[i]->Setup(this->components);
	}
	

	glGenTransformFeedbacks(1, &this->ogl4TransformFeedback);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->ogl4TransformFeedback);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, this->ogl4TransformFeedbackBuffer[0]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	this->layout = this->layouts[0];

	// setup layout, we will treat our transform buffer as an array buffer to render with it
	//this->layout->SetStreamBuffer(0, this->ogl4TransformFeedbackBuffer);
	//this->layout->Setup(this->components);
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4FeedbackBuffer::Discard()
{
    n_assert(0 == this->mapCount);
	n_assert(0 != this->ogl4TransformFeedback);
	n_assert(0 != this->ogl4TransformFeedbackBuffer[0]);

	glDeleteTransformFeedbacks(1, &this->ogl4TransformFeedback);
	this->ogl4TransformFeedback = 0;
	glDeleteBuffers(NumBuffers, &this->ogl4TransformFeedbackBuffer[0]);
	this->ogl4TransformFeedbackBuffer.Clear();
	IndexT i;
	for (i = 0; i < this->layouts.Size(); i++)
	{
		this->layouts[i]->Discard();
	}
	this->layouts.Clear();
	//this->layout->Discard();
	this->layout = 0;
	this->ogl4TransformFeedback = 0;
}

//------------------------------------------------------------------------------
/**
*/
void*
OGL4FeedbackBuffer::Map(Base::ResourceBase::MapType mapType)
{
	n_assert(0 != this->ogl4TransformFeedbackBuffer[0]);
    this->mapCount++;
	GLenum mapFlags = 0;
	switch (mapType)
	{
	case Base::ResourceBase::MapRead:
		mapFlags = GL_MAP_READ_BIT;
		break;
	case Base::ResourceBase::MapWrite:
		mapFlags = GL_MAP_WRITE_BIT;
		break;
	case Base::ResourceBase::MapReadWrite:
		mapFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
		break;
	case Base::ResourceBase::MapWriteDiscard:
		mapFlags = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
		break;
	}

	// bind buffer prior to mapping
	glBindBuffer(GL_ARRAY_BUFFER, this->ogl4TransformFeedbackBuffer[this->bufferIndex]);

	// glMapBufferRange is a more modern way of mapping buffers, this can be done without any implicit synchronization, which is nice and fast!
	void* data = glMapBufferRange(GL_ARRAY_BUFFER, 0, this->size * this->numElements, mapFlags);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	n_assert(data);
	return data;
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4FeedbackBuffer::Unmap()
{
	n_assert(0 != this->ogl4TransformFeedback);
    n_assert(this->mapCount > 0);
	glBindBuffer(GL_ARRAY_BUFFER, this->ogl4TransformFeedbackBuffer[this->bufferIndex]);
	GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	n_assert(result);
    this->mapCount--;
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4FeedbackBuffer::Swap()
{
	this->bufferIndex = (this->bufferIndex + 1) % NumBuffers;
	this->bufferOffset = this->bufferIndex * this->size * this->numElements;
	//this->primGroup.SetBaseVertex(this->bufferIndex * this->size);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->ogl4TransformFeedback);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, this->ogl4TransformFeedbackBuffer[this->bufferIndex]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	this->layout = this->layouts[this->bufferIndex];
}

} // namespace OpenGL4
