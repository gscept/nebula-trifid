//------------------------------------------------------------------------------
//  d3d9memoryindexbufferloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/win360/d3d9memoryindexbufferloader.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/win360/d3d9types.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9MemoryIndexBufferLoader, 'DMIL', Base::MemoryIndexBufferLoaderBase);

using namespace Resources;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
    This will create a D3D9 IndexBuffer using the data provided by our
    Setup() method and set our resource object (which must be a
    D3D9IndexBuffer object).
*/
bool
D3D9MemoryIndexBufferLoader::OnLoadRequested()
{
    n_assert(this->GetState() == Resource::Initial);
    n_assert(this->resource.isvalid());
    n_assert(!this->resource->IsAsyncEnabled());
    n_assert(this->indexType != IndexType::None);
    n_assert(this->numIndices > 0);
    if (IndexBuffer::UsageImmutable == this->usage)
    {
        n_assert(this->indexDataSize == (this->numIndices * IndexType::SizeOf(this->indexType)));
        n_assert(0 != this->indexDataPtr);
        n_assert(0 < this->indexDataSize);
    }

    IDirect3DDevice9* d3d9Device = RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Device);
   
	// create a D3D9 index buffer object
    DWORD d3dIndexBufferSize = this->numIndices * IndexType::SizeOf(this->indexType);
    D3DPOOL d3dPool     = D3D9Types::AsD3D9Pool(this->usage, this->access);
    DWORD d3dUsage      = D3D9Types::AsD3D9Usage(this->usage, this->access);
    D3DFORMAT d3dFormat = D3D9Types::IndexTypeAsD3D9Format(this->indexType);
    IDirect3DIndexBuffer9* d3dIndexBuffer = 0;
    HRESULT hr = d3d9Device->CreateIndexBuffer(d3dIndexBufferSize,     // Length,
                                               d3dUsage,                // Usage
                                               d3dFormat,               // Format
                                               d3dPool,                 // Pool
                                               &d3dIndexBuffer,         // ppIndexBuffer
                                               NULL);                   // pSharedHandle
    n_assert(SUCCEEDED(hr));
    n_assert(0 != d3dIndexBuffer);

	// setup initial data if provided
	if (0 != this->indexDataPtr)
	{
		// copy data to d3d9 index buffer
		void* dstPtr = 0;
		hr = d3dIndexBuffer->Lock(0, 0, &dstPtr, D3DLOCK_NOSYSLOCK);
		n_assert(SUCCEEDED(hr));
		n_assert(0 != dstPtr);
		Memory::CopyToGraphicsMemory(this->indexDataPtr, dstPtr, this->indexDataSize);
		hr = d3dIndexBuffer->Unlock();
		n_assert(SUCCEEDED(hr));
	}

    // setup our IndexBuffer resource
    const Ptr<IndexBuffer>& res = this->resource.downcast<IndexBuffer>();
    n_assert(!res->IsLoaded());
	res->SetUsage(this->usage);
	res->SetAccess(this->access);
    res->SetIndexType(this->indexType);
    res->SetNumIndices(this->numIndices);
    res->SetD3D9IndexBuffer(d3dIndexBuffer);

    // invalidate setup data (because we don't own our data)
    this->indexDataPtr = 0;
    this->indexDataSize = 0;

    this->SetState(Resource::Loaded);
    return true;
}

} // namespace Direct3D9
