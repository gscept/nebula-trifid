//------------------------------------------------------------------------------
//  d3d9memoryvertexbufferloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/vertexlayoutserver.h"
#include "coregraphics/win360/d3d9memoryvertexbufferloader.h"
#include "coregraphics/win360/d3d9types.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/vertexbuffer.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9MemoryVertexBufferLoader, 'DMVL', Base::MemoryVertexBufferLoaderBase);

using namespace Resources;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
    This will create a D3D9 vertex buffer and vertex declaration object
    from the data provided in the Setup() method and setup our resource
    object (which must be a D3D9VertexBuffer object).
*/
bool
D3D9MemoryVertexBufferLoader::OnLoadRequested()
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
    
    IDirect3DDevice9* d3d9Device = RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Device);

    // first setup the vertex layout (contains the D3D9 vertex declaration)
    Ptr<VertexLayout> vertexLayout = VertexLayoutServer::Instance()->CreateSharedVertexLayout(this->vertexComponents);
    if (0 != this->vertexDataPtr)
    {
        n_assert((this->numVertices * vertexLayout->GetVertexByteSize()) == this->vertexDataSize);
    }

    // create a d3d9 vertex buffer object
    DWORD d3dVertexBufferSize = this->numVertices * vertexLayout->GetVertexByteSize();
    D3DPOOL d3dPool     = D3D9Types::AsD3D9Pool(this->usage, this->access);
    DWORD d3dUsage      = D3D9Types::AsD3D9Usage(this->usage, this->access);
    IDirect3DVertexBuffer9* d3dVertexBuffer = 0;
    HRESULT hr = d3d9Device->CreateVertexBuffer(d3dVertexBufferSize,        // Length
                                                d3dUsage,                   // Usage
                                                0,                          // FVF
                                                d3dPool,                    // Pool
                                                &d3dVertexBuffer,           // ppVertexBuffer
                                                NULL);                      // pSharedHandle
    n_assert(SUCCEEDED(hr));
    n_assert(0 != d3dVertexBuffer);

	// setup initial data if provided
	if (0 != this->vertexDataPtr)
	{
		// copy vertex data to vertex buffer
		void* dstPtr = 0;
		hr = d3dVertexBuffer->Lock(0, 0, &dstPtr, D3DLOCK_NOSYSLOCK);
		n_assert(SUCCEEDED(hr));
		n_assert(0 != dstPtr);
		Memory::CopyToGraphicsMemory(this->vertexDataPtr, dstPtr, this->vertexDataSize);
		hr = d3dVertexBuffer->Unlock();
		n_assert(SUCCEEDED(hr));
	}

    // setup our resource object
    const Ptr<VertexBuffer>& res = this->resource.downcast<VertexBuffer>();
    n_assert(!res->IsLoaded());
	res->SetUsage(this->usage);
	res->SetAccess(this->access);
    res->SetVertexLayout(vertexLayout);
    res->SetNumVertices(this->numVertices);
    res->SetD3D9VertexBuffer(d3dVertexBuffer);

    // invalidate setup data (because we don't own our data)
    this->vertexDataPtr = 0;
    this->vertexDataSize = 0;

    this->SetState(Resource::Loaded);
    return true;
}

} // namespace Win360


#endif