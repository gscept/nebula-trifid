//------------------------------------------------------------------------------
//  ogl4memoryvertexbufferloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/vertexlayoutserver.h"
#include "coregraphics/ogl4/ogl4memoryvertexbufferloader.h"
#include "coregraphics/ogl4/ogl4types.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/vertexbuffer.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4MemoryVertexBufferLoader, 'DMVL', Base::MemoryVertexBufferLoaderBase);

using namespace Resources;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
    This will create a OGL4 vertex buffer and vertex declaration object
    from the data provided in the Setup() method and setup our resource
    object (which must be a OGL4VertexBuffer object).
*/
bool
OGL4MemoryVertexBufferLoader::OnLoadRequested()
{
    n_assert(this->GetState() == Resource::Initial);
    n_assert(this->resource.isvalid());
    n_assert(!this->resource->IsAsyncEnabled());
    n_assert(this->numVertices > 0);
    if (VertexBuffer::UsageImmutable == this->usage)
    {
        n_assert(0 != this->vertexDataPtr);
        n_assert(0 < this->vertexDataSize);
    }

	SizeT vertexSize = VertexLayoutServer::Instance()->CalculateVertexSize(this->vertexComponents);
	GLuint ogl4VertexBuffer;
	GLenum usage = OGL4Types::AsOGL4Usage(this->usage);
	GLenum sync = OGL4Types::AsOGL4Syncing(this->syncing);
	glGenBuffers(1, &ogl4VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ogl4VertexBuffer);
	if (this->syncing == VertexBuffer::SyncingSimple)	glBufferData(GL_ARRAY_BUFFER, this->numVertices * vertexSize, this->vertexDataPtr, usage | sync);
	else												glBufferStorage(GL_ARRAY_BUFFER, this->numVertices * vertexSize, this->vertexDataPtr, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | sync);
	Ptr<VertexLayout> vertexLayout = VertexLayout::Create();
	vertexLayout->SetStreamBuffer(0, ogl4VertexBuffer);
	vertexLayout->Setup(this->vertexComponents);
	if (0 != this->vertexDataPtr)
	{
		n_assert((this->numVertices * vertexLayout->GetVertexByteSize()) == this->vertexDataSize);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	n_assert(GLSUCCESS);

    // setup our resource object
    const Ptr<VertexBuffer>& res = this->resource.downcast<VertexBuffer>();
    n_assert(!res->IsLoaded());
	res->SetUsage(this->usage);
	res->SetAccess(this->access);
	res->SetSyncing(this->syncing);
    res->SetVertexLayout(vertexLayout);
    res->SetNumVertices(this->numVertices);
    res->SetOGL4VertexBuffer(ogl4VertexBuffer);

    // invalidate setup data (because we don't own our data)
    this->vertexDataPtr = 0;
    this->vertexDataSize = 0;

    this->SetState(Resource::Loaded);
    return true;
}

} // namespace OpenGL4
