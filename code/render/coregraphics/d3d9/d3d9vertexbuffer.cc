//------------------------------------------------------------------------------
//  d3d9vertexbuffer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/win360/d3d9vertexbuffer.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9VertexBuffer, 'D9VB', Base::VertexBufferBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
D3D9VertexBuffer::D3D9VertexBuffer() :
    d3d9VertexBuffer(0),
    mapCount(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
D3D9VertexBuffer::~D3D9VertexBuffer()
{
    n_assert(0 == this->d3d9VertexBuffer);
    n_assert(0 == this->mapCount);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9VertexBuffer::Unload()
{
    n_assert(0 == this->mapCount);
    if (0 != this->d3d9VertexBuffer)
    {
        this->d3d9VertexBuffer->Release();
        this->d3d9VertexBuffer = 0;
    }
    VertexBufferBase::Unload();
}

//------------------------------------------------------------------------------
/**
*/
void*
D3D9VertexBuffer::Map(MapType mapType)
{
    n_assert(0 != this->d3d9VertexBuffer);
    DWORD lockFlags = 0;
    switch (mapType)
    {
        case MapRead:
            // deactivated by Georg granted by Floh
            //n_assert(((UsageDynamic == this->usage) || (UsageCpu == this->usage)) && (AccessRead == this->access));
            break;

        case MapWrite:
            n_assert(((UsageDynamic == this->usage) || (UsageCpu == this->usage)) && (AccessWrite == this->access));
            break;

        case MapReadWrite:
            n_assert(((UsageDynamic == this->usage) || (UsageCpu == this->usage)) && (AccessReadWrite == this->access));
            break;

        case MapWriteDiscard:
            n_assert((UsageDynamic == this->usage) && (AccessWrite == this->access));
            #if __WIN32__
            lockFlags |= D3DLOCK_DISCARD;
            #endif
            break;

        case MapWriteNoOverwrite:
            n_assert((UsageDynamic == this->usage) && (AccessWrite == this->access));
            lockFlags |= D3DLOCK_NOOVERWRITE;
            break;
    }
    void* ptr = 0;
    HRESULT hr = this->d3d9VertexBuffer->Lock(0, 0, &ptr, lockFlags);
    n_assert(SUCCEEDED(hr));
    this->mapCount++;
    return ptr;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9VertexBuffer::Unmap()
{
    n_assert(0 != this->d3d9VertexBuffer);
    n_assert(this->mapCount > 0);
    HRESULT hr = this->d3d9VertexBuffer->Unlock();
    n_assert(SUCCEEDED(hr));
    this->mapCount--;
}

} // namespace Win360

