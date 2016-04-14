#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::MemoryVertexBufferLoaderBase
    
    Base resource loader class for initializing an vertex buffer from
    data in memory.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
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
		CoreGraphics::VertexBuffer::Syncing syncing = CoreGraphics::VertexBuffer::SyncingSimple,
		bool createLayout = true);

protected:
    Util::Array<CoreGraphics::VertexComponent> vertexComponents;
    SizeT numVertices;
    void* vertexDataPtr;
    SizeT vertexDataSize;
	CoreGraphics::VertexBuffer::Usage usage;
	CoreGraphics::VertexBuffer::Access access;
	CoreGraphics::VertexBuffer::Syncing syncing;
	bool createLayout;
};

} // namespace Base
//------------------------------------------------------------------------------

