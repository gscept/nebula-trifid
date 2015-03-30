#pragma once
//------------------------------------------------------------------------------
/** 
    @class Win360::D3D9VertexBuffer
  
    D3D9/Xbox360 implementation of VertexBuffer.
    
    (C) 2007 Radon Labs GmbH
*/    
#include "coregraphics/base/vertexbufferbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9VertexBuffer : public Base::VertexBufferBase
{
    __DeclareClass(D3D9VertexBuffer);
public:
    /// constructor
    D3D9VertexBuffer();
    /// destructor
    virtual ~D3D9VertexBuffer();

    /// unload the resource, or cancel the pending load
    virtual void Unload();
    /// map the vertices for CPU access
    void* Map(MapType mapType);
    /// unmap the resource
    void Unmap();

    /// set d3d9 vertex buffer pointer
    void SetD3D9VertexBuffer(IDirect3DVertexBuffer9* ptr);
    /// get pointer to d3d9 vertex buffer object
    IDirect3DVertexBuffer9* GetD3D9VertexBuffer() const;

private:
    IDirect3DVertexBuffer9* d3d9VertexBuffer;
    int mapCount;
};

//------------------------------------------------------------------------------
/**
*/
inline void
D3D9VertexBuffer::SetD3D9VertexBuffer(IDirect3DVertexBuffer9* ptr)
{
    n_assert(0 != ptr);
    n_assert(0 == this->d3d9VertexBuffer);
    this->d3d9VertexBuffer = ptr;
}

//------------------------------------------------------------------------------
/**
*/
inline IDirect3DVertexBuffer9*
D3D9VertexBuffer::GetD3D9VertexBuffer() const
{
    n_assert(0 != this->d3d9VertexBuffer);
    n_assert(0 == this->mapCount);
    return this->d3d9VertexBuffer;
}

} // namespace Direct3D9
//------------------------------------------------------------------------------

