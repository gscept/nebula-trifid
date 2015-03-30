#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::MemoryVertexBufferLoaderBase
    
    Base resource loader class for initializing an vertex buffer from
    data in memory.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "resources/resourceloader.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/vertexbuffer.h"

//------------------------------------------------------------------------------
namespace Base
{
class MemoryVertexBufferLoaderBase : public Resources::ResourceLoader
{
    __DeclareClass(MemoryVertexBufferLoaderBase);
public:
    /// constructor
    MemoryVertexBufferLoaderBase();
    /// setup vertex buffer data, must remain valid until OnLoadRequested() is called!
    void Setup(const Util::Array<CoreGraphics::VertexComponent>& vertexComponents, 
		SizeT numVertices, 
		void* vertexDataPtr, 
		SizeT vertexDataSize, 
		CoreGraphics::VertexBuffer::Usage usage, 
		CoreGraphics::VertexBuffer::Access access, 
		CoreGraphics::VertexBuffer::BufferCount bufferCount = CoreGraphics::VertexBuffer::BufferSingle,
		CoreGraphics::VertexBuffer::Syncing syncing = CoreGraphics::VertexBuffer::SyncingSimple);

protected:
    Util::Array<CoreGraphics::VertexComponent> vertexComponents;
    SizeT numVertices;
    void* vertexDataPtr;
    SizeT vertexDataSize;
	CoreGraphics::VertexBuffer::Usage usage;
	CoreGraphics::VertexBuffer::Access access;
	CoreGraphics::VertexBuffer::BufferCount bufferCount;
	CoreGraphics::VertexBuffer::Syncing syncing;
};

} // namespace Base
//------------------------------------------------------------------------------

